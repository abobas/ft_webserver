/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Server.cpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/08/27 17:11:43 by abobas        #+#    #+#                 */
/*   Updated: 2020/10/26 20:44:12 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include <iostream>

Server::Server(const Json &&config) : config(config)
{
	createListenSockets();
	while (1)
		runtime();
}

void Server::runtime()
{
	int select = selectCall();
	if (select < 0)
	{
		perror("select()");
		throw;
	}
	if (select > 0)
		handleOperations(select);
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
		{
			perror("setsockopt()");
			throw;
		}
		new_address.sin_family = AF_INET;
		new_address.sin_addr.s_addr = INADDR_ANY;
		new_address.sin_port = htons(server["listen"].number_value());
		if (bind(new_socket, reinterpret_cast<sockaddr *>(&new_address), sizeof(new_address)) < 0)
		{
			perror("bind()");
			throw;
		}
		if (listen(new_socket, SOMAXCONN) < 0)
		{
			perror("listen()");
			throw;
		}
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
		if (socket.getSocket() + 1 > max)
			max = socket.getSocket() + 1;
	}
	return (max);
}

int Server::selectCall()
{
	fillSelectSets();
	return (select(getSelectRange(), &read_set, &write_set, NULL, &tv));
}

void Server::handleOperations(int select)
{
	for (auto &socket : sockets)
	{
		if (socket.getType() == "listen")
		{
			if (FD_ISSET(socket.getSocket(), &read_set))
			{
				acceptClient(socket);
				select--;
			}
		}
		else if (socket.getType() == "client_read")
		{
			if (FD_ISSET(socket.getSocket(), &read_set))
			{
				readClient(socket);
				select--;
			}
		}
		else if (socket.getType() == "client_write")
		{
			if (FD_ISSET(socket.getSocket(), &write_set))
			{
				writeClient(socket);
				select--;
			}
		}
		else if (socket.getType() == "proxy_read")
		{
			if (FD_ISSET(socket.getSocket(), &read_set))
			{
				readProxy(socket);
				select--;
			}
		}
		else if (socket.getType() == "proxy_write")
		{
			if (FD_ISSET(socket.getSocket(), &write_set))
			{
				writeProxy(socket);
				select--;
			}
		}
		else if (socket.getType() == "waiting_client_write")
		{
			if (FD_ISSET(socket.getSocket(), &write_set))
			{
				writeWaitingClient(socket);
				select--;
			}
		}
		if (select == 0)
			return;
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
		perror("accept()");
		throw;
	}
	addSocket(Socket("client_read", client));
	std::cout << "acceptClient " << client << std::endl;
}

void Server::readClient(Socket &client)
{
	addMessage(client, client.receive());
	std::cout << "readClient1 " << client.getType() << " " << client.getSocket() << std::endl;
	transformSocket(client);
	std::cout << "readClient2 " << client.getType() << " " << client.getSocket() << std::endl;
}

void Server::writeClient(Socket &client)
{
	std::cout << "writeClient1 " << client.getType() << " " << client.getSocket() << std::endl;
	Response response(getData(client, config, messages[client]));
	std::cout << "writeClient2 " << client.getType() << " " << client.getSocket() << std::endl;
	deleteMessage(client);
	if (response.isProxy())
	{
		std::cout << "ProxyAdded " << response.getProxySocket().getType() << " " << response.getProxySocket().getSocket() << std::endl;
		addSocket(response.getProxySocket());
		addMessage(response.getProxySocket(), response.getProxyRequest());
		addPair(client, response.getProxySocket());
		transformSocket(client);
	}
	else
		disconnectSocket(client);
}

void Server::writeProxy(Socket &proxy)
{
	std::cout << "writeProxy1 " << proxy.getType() << " " << proxy.getSocket() << std::endl;
	std::cout << "---------------MESSAGE TO PROXY--------" << std::endl;
	std::cout << messages[proxy] << std::endl;
	std::cout << "---------------END OF MESSAGE-----------" << std::endl;
	
	proxy.sendData(messages[proxy]);
	
	std::cout << "SUCCESFULLY WRITTEN TO PROXY" << std::endl;
	
	deleteMessage(proxy);
	transformSocket(proxy);
	
	std::cout << "writeProxy2 " << proxy.getType() << " " << proxy.getSocket() << std::endl;
}

void Server::readProxy(Socket &proxy)
{
	std::cout << "readProxy1 " << proxy.getType() << " " << proxy.getSocket() << std::endl;
	addMessage(proxy, proxy.receive());
	transformSocket(proxy);
	std::cout << "readProxy1 " << proxy.getType() << " " << proxy.getSocket() << std::endl;
}

void Server::writeWaitingClient(Socket &client)
{
	if (pairs[client].getType() != "proxy_done")
		return;
	std::cout << "writeWaitClient1 " << client.getType() << " " << client.getSocket() << std::endl;	
	client.sendData(messages[pairs[client]]);
	deleteMessage(pairs[client]);
	disconnectSocket(pairs[client]);
	deletePair(client);
	disconnectSocket(client);
	std::cout << "writeWaitClient2 " << client.getType() << " " << client.getSocket() << std::endl;
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
}

void Server::disconnectSocket(Socket &socket)
{
	close(socket.getSocket());
	deleteSocket(socket);
}

void Server::addSocket(Socket &&insert)
{
	sockets.push_back(insert);
}

void Server::deleteSocket(Socket &erase)
{
	sockets.erase(std::find(sockets.begin(), sockets.end(), erase));
}

void Server::addPair(Socket &key, Socket &&value)
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