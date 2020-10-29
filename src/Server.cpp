/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Server.cpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/08/27 17:11:43 by abobas        #+#    #+#                 */
/*   Updated: 2020/10/29 14:12:01 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server(Json &&config) : config(config)
{
	log = Log::getInstance();
	try
	{
		createListenSockets();
		while (1)
			runtime();
	}
	catch (const char *e)
	{
		log->logError(e);
	}
}

void Server::runtime()
{
	int select = selectCall();
	if (select < 0)
		throw "select()";
	if (select > 0)
		handleOperations();
}

void Server::createListenSockets()
{
	for (auto server : config["http"]["servers"].array_items())
	{
		int listen = Socket::getListenSocket(server["listen"].number_value());
		addSocket(Socket("listen", listen));
		log->logEntry("created socket", listen);
	}
}

void Server::fillSelectSets()
{
	FD_ZERO(&read_set);
	FD_ZERO(&write_set);
	for (auto socket : sockets)
	{
		if (socket.getType() == "listen")
			FD_SET(socket.getSocket(), &read_set);
		else if (socket.getType() == "client_read")
			FD_SET(socket.getSocket(), &read_set);
		else if (socket.getType() == "client_write")
			FD_SET(socket.getSocket(), &write_set);
		else if (socket.getType() == "proxy_read")
			FD_SET(socket.getSocket(), &read_set);
		else if (socket.getType() == "proxy_write")
			FD_SET(socket.getSocket(), &write_set);
		else if (socket.getType() == "wait_client_write")
			FD_SET(socket.getSocket(), &write_set);
	}
}

int Server::getSelectRange()
{
	int max = 0;
	for (auto socket : sockets)
	{
		if (socket.getSocket() > max)
			max = socket.getSocket();
	}
	return (max + 1);
}

int Server::selectCall()
{
	fillSelectSets();
	return (select(getSelectRange(), &read_set, &write_set, NULL, &tv));
}

void Server::handleOperations()
{
	for (auto &socket : sockets)
	{
		if (socket.getType() == "listen")
		{
			if (FD_ISSET(socket.getSocket(), &read_set))
				acceptClient(socket);
		}
		else if (socket.getType() == "client_read")
		{
			if (FD_ISSET(socket.getSocket(), &read_set))
				readClient(socket);
		}
		else if (socket.getType() == "client_write")
		{
			if (FD_ISSET(socket.getSocket(), &write_set))
				writeClient(socket);
		}
		else if (socket.getType() == "proxy_read")
		{
			if (FD_ISSET(socket.getSocket(), &read_set))
				readProxy(socket);
		}
		else if (socket.getType() == "proxy_write")
		{
			if (FD_ISSET(socket.getSocket(), &write_set))
				writeProxy(socket);
		}
		else if (socket.getType() == "wait_client_write")
		{
			if (FD_ISSET(socket.getSocket(), &write_set))
				writeWaitingClient(socket);
		}
	}
}

void Server::acceptClient(Socket &listen)
{
	int client = listen.acceptClient();
	if (client < 0)
	{
		log->logError("accept()");
		return;
	}
	addSocket(Socket("client_read", client));
	log->logEntry("accepted client", client);
}

void Server::readClient(Socket &client)
{
	if (client.closedClient())
		disconnectSocket(client);
	else
	{
		client.receiveData();
		if (!client.getEndOfFile())
		{
			log->logEntry("read part of client", client.getSocket());
			return;
		}
		log->logEntry("fully read client", client.getSocket());
		log->logBlock(client.getMessage());
		addMessage(client, client.getMessage());
		transformSocket(client);
	}
}

void Server::writeClient(Socket &client)
{
	Response response(Data(client, config, messages[client]));
	deleteMessage(client);
	if (response.getProxyValue())
	{
		addSocket(response.getProxySocket());
		addMessage(response.getProxySocket(), response.getProxyRequest());
		addPair(client.getSocket(), sockets.back().getSocket());
		log->logEntry("connected with proxy", response.getProxySocket().getSocket());
		client.setType("wait_client_write");
		log->logEntry("transformed socket", client.getSocket());
	}
	else
	{
		log->logEntry("wrote client", client.getSocket());
		transformSocket(client);
	}
}

void Server::writeProxy(Socket &proxy)
{
	proxy.sendData(messages[proxy]);
	log->logEntry("wrote proxy", proxy.getSocket());
	deleteMessage(proxy);
	transformSocket(proxy);
}

void Server::readProxy(Socket &proxy)
{
	proxy.receiveData();
	if (!proxy.getEndOfFile())
	{
		log->logEntry("read part of proxy", proxy.getSocket());
		//log->logBlock(proxy.getMessage());
		return;
	}
	log->logEntry("fully read proxy", proxy.getSocket());
	//log->logBlock(proxy.getMessage());
	addMessage(proxy, proxy.getMessage());
	transformSocket(proxy);
}

Socket &Server::findPair(Socket &client)
{
	for (auto &socket : sockets)
	{
		if (socket.getSocket() == pairs[client.getSocket()])
			return socket;
	}
	return client;
}

void Server::writeWaitingClient(Socket &client)
{
	Socket proxy = findPair(client);
	if (proxy == client || proxy.getType() != "proxy_done")
		return;
	client.sendData(messages[proxy]);
	log->logEntry("wrote client", client.getSocket());
	deleteMessage(proxy);
	disconnectSocket(proxy);
	deletePair(client.getSocket());
	transformSocket(client);
}

void Server::transformSocket(Socket &socket)
{
	if (socket.getType() == "client_read")
		socket.setType("client_write");
	else if (socket.getType() == "client_write")
	{
		socket.setType("client_read");
		socket.cleanSocket();
	}
	else if (socket.getType() == "wait_client_write")
	{
		socket.setType("client_read");
		socket.cleanSocket();
	}
	else if (socket.getType() == "proxy_write")
		socket.setType("proxy_read");
	else if (socket.getType() == "proxy_read")
		socket.setType("proxy_done");
	log->logEntry("transformed socket", socket.getSocket());
}

void Server::disconnectSocket(Socket &socket)
{
	if (close(socket.getSocket() < 0))
		log->logError("close()");
	deleteSocket(socket);
	log->logEntry("disconnected socket", socket.getSocket());
}

void Server::addSocket(Socket &insert)
{
	sockets.push_back(insert);
}

void Server::addSocket(Socket &&insert)
{
	sockets.push_back(insert);
}

void Server::deleteSocket(Socket &erase)
{
	sockets.erase(std::find(sockets.begin(), sockets.end(), erase));
}

void Server::addPair(int key, int value)
{
	pairs.insert({key, value});
}

void Server::deletePair(int key)
{
	pairs.erase(key);
}

void Server::addMessage(Socket &socket, std::string &&message)
{
	messages.insert({socket, message});
}

void Server::addMessage(Socket &&socket, std::string &&message)
{
	messages.insert({socket, message});
}

void Server::deleteMessage(Socket &socket)
{
	messages.erase(socket);
}