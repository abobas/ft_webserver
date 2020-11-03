/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Server.cpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/08/27 17:11:43 by abobas        #+#    #+#                 */
/*   Updated: 2020/11/03 02:34:30 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Log *Server::log = Log::getInstance();
Json Server::config = Config::getConfig();
timeval Server::tv
{
	tv.tv_sec = 0,
	tv.tv_usec = 0
};


Server::Server()
{
	try
	{
		createListenSockets();
		while (1)
			mainLoop();
	}
	catch (const char *e)
	{
		log->logError(e);
	}
}

void Server::createListenSockets()
{
	for (auto server : config["http"]["servers"].array_items())
	{
		int listen = getListenSocket(server["listen"].number_value());
		addSocket(Socket("listen", listen));
		log->logEntry("created socket", listen);
	}
}

int Server::getListenSocket(int port)
{
	int new_socket;
	int enable = 1;
	sockaddr_in new_address;

	memset(&new_address, 0, sizeof(new_address));
	new_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (setsockopt(new_socket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
		throw "setsockopt()";
	new_address.sin_family = AF_INET;
	new_address.sin_addr.s_addr = INADDR_ANY;
	new_address.sin_port = htons(port);
	if (bind(new_socket, reinterpret_cast<sockaddr *>(&new_address), sizeof(new_address)) < 0)
		throw "bind()";
	if (listen(new_socket, SOMAXCONN) < 0)
		throw "listen()";
	return new_socket;
}

void Server::mainLoop()
{
	int select = selectCall();
	if (select < 0)
		throw "select()";
	if (select > 0)
		handleOperations(select);
}

int Server::selectCall()
{
	fillSelectSets();
	return (select(getSelectRange(), &read_set, &write_set, NULL, &tv));
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

void Server::handleOperations(int select)
{
	for (auto &socket : sockets)
	{
		if (select < 1)
			break;
		if (FD_ISSET(socket.getSocket(), getSet(socket)))
		{
			executeOperation(socket);
			select--;
		}
	}
}

fd_set *Server::getSet(Socket &socket)
{
	if (socket.getType() == "listen")
		return &read_set;
	else if (socket.getType() == "client_read")
		return &read_set;
	else if (socket.getType() == "client_write")
		return &write_set;
	else if (socket.getType() == "proxy_read")
		return &read_set;
	else if (socket.getType() == "proxy_write")
		return &write_set;
	else if (socket.getType() == "wait_client_write")
		return &write_set;
	return NULL;
}

void Server::executeOperation(Socket &socket)
{
	if (socket.getType() == "listen")
		acceptClient(socket);
	else if (socket.getType() == "client_read")
		readClient(socket);
	else if (socket.getType() == "client_write")
		writeClient(socket);
	else if (socket.getType() == "proxy_read")
		readProxy(socket);
	else if (socket.getType() == "proxy_write")
		writeProxy(socket);
	else if (socket.getType() == "wait_client_write")
		writeWaitingClient(socket);
}

void Server::acceptClient(Socket &listen)
{
	int client;
	struct sockaddr client_address;
	unsigned int client_address_length = 0;

	memset(&client_address, 0, sizeof(client_address));
	client = accept(listen.getSocket(), &client_address, &client_address_length);
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
	if (!client.isAlive())
	{
		disconnectSocket(client);
		return;
	}
	client.receiveMessage();
	if (!client.isReady())
		return;
	log->logEntry("read client", client.getSocket());
	addMessage(client, client.getMessage());
	transformSocket(client);
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
		log->logEntry("client is now waiting", client.getSocket());
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
	proxy.receiveMessage();
	if (!proxy.isReady())
		return;
	log->logEntry("fully read proxy", proxy.getSocket());
	log->logBlock(proxy.getMessage());
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
	//log->logEntry("transforming from " + socket.getType(), socket.getSocket());
	if (socket.getType() == "client_read")
		socket.setType("client_write");
	else if (socket.getType() == "client_write")
		socket.setType("client_read");
	else if (socket.getType() == "wait_client_write")
		socket.setType("client_read");
	else if (socket.getType() == "proxy_write")
		socket.setType("proxy_read");
	else if (socket.getType() == "proxy_read")
		socket.setType("proxy_done");
	//log->logEntry("transformed into " + socket.getType(), socket.getSocket());
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