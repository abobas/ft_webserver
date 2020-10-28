/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Server.cpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/08/27 17:11:43 by abobas        #+#    #+#                 */
/*   Updated: 2020/10/28 17:41:24 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server(Json &&config) : config(config)
{
	try
	{
		log.createLogFile("./logs/logs.txt");
		createListenSockets();
		while (1)
			runtime();
	}
	catch (const char *e)
	{
		log.logError(e);
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
		int listen = getListenSocket(server["listen"].number_value());
		addSocket(Socket("listen", listen));
		log.logSocket("created socket", listen);
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
		else if (socket.getType() == "waiting_client_write")
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
		else if (socket.getType() == "waiting_client_write")
		{
			if (FD_ISSET(socket.getSocket(), &write_set))
				writeWaitingClient(socket);
		}
	}
}

void Server::acceptClient(Socket &listen)
{
	int client = getAcceptedClient(listen.getSocket());
	if (client < 0)
	{
		log.logError("accept()");
		return;
	}
	addSocket(Socket("client_read", client));
	log.logSocket("accepted client", client);
}

void Server::readClient(Socket &client)
{
	addMessage(client, client.receive());
	log.logSocket("read client", client);
	transformSocket(client);
}

void Server::writeClient(Socket &client)
{
	Response response(Data(client, config, messages[client]));
	deleteMessage(client);
	if (response.isProxySet())
	{
		addSocket(response.getProxySocket());
		addMessage(response.getProxySocket(), response.getProxyRequest());
		addPair(client.getSocket(), sockets.back().getSocket());
		log.logSocket("connected with proxy", response.getProxySocket());
		transformSocket(client);
	}
	else
	{
		log.logSocket("wrote client", client);
		disconnectSocket(client);
	}
}

void Server::writeProxy(Socket &proxy)
{
	proxy.sendData(messages[proxy]);
	log.logSocket("wrote proxy", proxy);
	deleteMessage(proxy);
	transformSocket(proxy);
}

void Server::readProxy(Socket &proxy)
{
	addMessage(proxy, proxy.receive());
	log.logSocket("read proxy", proxy);
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
	log.logSocket("wrote client", client);
	deleteMessage(proxy);
	disconnectSocket(proxy);
	deletePair(client.getSocket());
	disconnectSocket(client);
}

void Server::transformSocket(Socket &socket)
{
	if (socket.getType() == "client_read")
		socket.setType("client_write");
	else if (socket.getType() == "client_write")
		socket.setType("waiting_client_write");
	else if (socket.getType() == "proxy_write")
		socket.setType("proxy_read");
	else if (socket.getType() == "proxy_read")
		socket.setType("proxy_done");
	log.logSocket("transformed socket", socket);
}

void Server::disconnectSocket(Socket &socket)
{
	close(socket.getSocket());
	deleteSocket(socket);
	log.logSocket("disconnected socket", socket);
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