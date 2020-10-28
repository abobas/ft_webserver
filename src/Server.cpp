/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Server.cpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/08/27 17:11:43 by abobas        #+#    #+#                 */
/*   Updated: 2020/10/28 04:07:21 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server(Json &&config) : config(config) 
{
	try
	{
		createListenSockets();
		createLogs("./logs/logs.txt");
		while (1)
			runtime();
	}
	catch (const char *e)
	{
		logs << getTime() << e << ": " << strerror(errno) << std::endl;
	}
}

std::string Server::getTime()
{
	struct timeval time;
	struct tm *tmp;
	char string[128];

	if (gettimeofday(&time, NULL))
		return "";
	tmp = localtime(&time.tv_sec);
	strftime(string, 128, "%x %X", tmp);
	return std::string(string) + " ";
}

void Server::runtime()
{
	int select = selectCall();
	if (select < 0)
		throw "select()";
	if (select > 0)
		handleOperations();
}

void Server::createLogs(std::string path)
{
	logs.open(path.c_str(), std::ios::out | std::ios::trunc);
	if (!logs.is_open())
		throw "is_open()";
}

void Server::createListenSockets()
{
	int new_socket;
	int enable = 1;
	sockaddr_in new_address;

	for (auto server : config["http"]["servers"].array_items())
	{
		memset(&new_address, 0, sizeof(new_address));
		new_socket = socket(AF_INET, SOCK_STREAM, 0);
		if (setsockopt(new_socket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
			throw "setsockopt()";
		new_address.sin_family = AF_INET;
		new_address.sin_addr.s_addr = INADDR_ANY;
		new_address.sin_port = htons(server["listen"].number_value());
		if (bind(new_socket, reinterpret_cast<sockaddr *>(&new_address), sizeof(new_address)) < 0)
			throw "bind()";
		if (listen(new_socket, SOMAXCONN) < 0)
			throw "listen()";
		addSocket(Socket("listen", new_socket));
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
	int client;
	struct sockaddr client_address;
	unsigned int client_address_length = 0;

	memset(&client_address, 0, sizeof(client_address));
	client = accept(listen.getSocket(), &client_address, &client_address_length);
	if (client < 0)
	{
		logs << getTime() << " accept(): " << strerror(errno) << std::endl;
		return;
	}
	addSocket(Socket("client_read", client));
	logs << getTime() << "accepted client " << client << std::endl;
}

void Server::readClient(Socket &client)
{
	addMessage(client, client.receive());
	logs << getTime() << "read client " << client.getSocket() << std::endl;
	transformSocket(client);
}

void Server::writeClient(Socket &client)
{
	Response response(Data(client, config, messages[client]));
	deleteMessage(client);
	if (response.isProxySet())
	{
		Socket proxy = response.getProxySocket();
		addSocket(proxy);
		addMessage(proxy, response.getProxyRequest());
		addPair(client, sockets.back());
		logs << getTime() << "connected with proxy " << proxy.getSocket() << std::endl;
		transformSocket(client);
	}
	else
	{
		logs << getTime() << "wrote client " << client.getSocket() << std::endl;
		disconnectSocket(client);
	}
}

void Server::writeProxy(Socket &proxy)
{
	proxy.sendData(messages[proxy]);
	logs << getTime() << "wrote proxy " << proxy.getSocket() << std::endl;
	deleteMessage(proxy);
	transformSocket(proxy);
}

void Server::readProxy(Socket &proxy)
{
	addMessage(proxy, proxy.receive());
	logs << getTime() << "read proxy " << proxy.getSocket() << std::endl;
	transformSocket(proxy);
}

void Server::writeWaitingClient(Socket &client)
{
	// debugging
	logs << getTime() << "pairs[client].getType() " << pairs[client].getType() << std::endl;
	if (pairs[client].getType() != "proxy_done")
		return;
	client.sendData(messages[pairs[client]]);
	logs << getTime() << "wrote client " << client.getSocket() << std::endl;
	deleteMessage(pairs[client]);
	disconnectSocket(pairs[client]);
	deletePair(client);
	disconnectSocket(client);
}

void Server::transformSocket(Socket &socket)
{
	logs << getTime() << "transformed socket " << socket.getSocket() << " from " << socket.getType();
	if (socket.getType() == "client_read")
		socket.setType("client_write");
	else if (socket.getType() == "client_write")
		socket.setType("waiting_client_write");
	else if (socket.getType() == "proxy_write")
		socket.setType("proxy_read");
	else if (socket.getType() == "proxy_read")
		socket.setType("proxy_done");
	logs << " to " << socket.getType() << std::endl;
}

void Server::disconnectSocket(Socket &socket)
{
	close(socket.getSocket());
	deleteSocket(socket);
	logs << getTime() << "disconnected socket " << socket.getSocket() << std::endl;}

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

void Server::addPair(Socket &key, Socket &value)
{
	pairs.insert({key, value});
}

void Server::deletePair(Socket &key)
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