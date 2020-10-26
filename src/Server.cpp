/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Server.cpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/08/27 17:11:43 by abobas        #+#    #+#                 */
/*   Updated: 2020/10/26 01:31:46 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server(const Json &&config) : config(config)
{
	createServerSockets();
	while (1)
		runtime();
}

void Server::createServerSockets()
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
		addSocket(Socket("server", new_socket));
	}
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

void Server::fillSets()
{
	FD_ZERO(&read_set);
	FD_ZERO(&write_set);
	for (auto socket : sockets)
	{
		if (socket.getType() == "server")
			FD_SET(socket.getSocket(), &read_set);
		else if (socket.getType() == "read")
			FD_SET(socket.getSocket(), &read_set);
		else if (socket.getType() == "write")
			FD_SET(socket.getSocket(), &write_set);
	}
}

int Server::getRange()
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
	fillSets();
	return (select(getRange(), &read_set, &write_set, NULL, &tv));
}

void Server::handleOperations(int select)
{
	for (auto socket : sockets)
	{
		if (socket.getType() == "server")
		{
			if (FD_ISSET(socket.getSocket(), &read_set))
			{
				acceptClient(socket);
				select--;
			}
		}
		else if (socket.getType() == "read")
		{
			if (FD_ISSET(socket.getSocket(), &read_set))
			{
				readClient(socket);
				select--;
			}
		}
		else if (socket.getType() == "write")
		{
			if (FD_ISSET(socket.getSocket(), &write_set))
			{
				writeClient(socket);
				select--;
			}
		}
		if (select == 0)
			return;
	}
}

void Server::acceptClient(Socket &server)
{
	int client;
	struct sockaddr client_address;
	unsigned int client_address_length = 0;

	memset(&client_address, 0, sizeof(client_address));
	client = accept(server.getSocket(), &client_address, &client_address_length);
	if (client < 0)
	{
		perror("accept()");
		throw;
	}
	addSocket(Socket("read", client));
}

void Server::readClient(Socket &client)
{
	addRequest(client, client.receive());
	transformClient(client);
}

void Server::writeClient(Socket &client)
{
	Response response(getData(client, config, requests[client]));
	deleteRequest(client);
	disconnectClient(client);
}

void Server::transformClient(Socket &client)
{
	if (client.getType() == "read")
	{
		deleteSocket(client);
		addSocket(Socket("write", client.getSocket()));
	}
	else if (client.getType() == "write")
	{
		deleteSocket(client);
		addSocket(Socket("read", client.getSocket()));
	}
}

void Server::disconnectClient(Socket &client)
{
	close(client.getSocket());
	deleteSocket(client);
}

void Server::addSocket(Socket &&insert)
{
	sockets.push_back(insert);
}

void Server::deleteSocket(Socket &erase)
{
	sockets.erase(std::find(sockets.begin(), sockets.end(), erase));
}

void Server::addRequest(Socket &client, std::string &&request)
{
	requests.insert({client, request});
}

void Server::deleteRequest(Socket &client)
{
	requests.erase(client);
}