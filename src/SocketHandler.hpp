/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   SocketHandler.hpp                                  :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/07/21 17:17:49 by abobas        #+#    #+#                 */
/*   Updated: 2020/08/26 22:04:02 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Socket.hpp"
#include "Json.hpp"
#include "ResponseHandler.hpp"
#include "HttpParser.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include <vector>
#include <map>
#include <algorithm>
#include <string>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <iostream>

class SocketHandler
{

public:
	SocketHandler(
		const Json::Json &config) : config(config) {}

	~SocketHandler() {}

	void init()
	{
		int new_socket;
		int enable = 1;
		sockaddr_in new_address;

		for (size_t i = 0; i < config["http"]["servers"].array_items().size(); i++)
		{
			new_socket = socket(AF_INET, SOCK_STREAM, 0);
			setsockopt(new_socket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));
			new_address.sin_family = AF_INET;
			new_address.sin_addr.s_addr = INADDR_ANY;
			new_address.sin_port = htons(config["http"]["servers"][i]["listen"].number_value());
			for (size_t i = 0; i < sizeof(new_address.sin_zero); i++)
				new_address.sin_zero[i] = 0;
			bind(new_socket, reinterpret_cast<sockaddr *>(&new_address), sizeof(new_address));
			listen(new_socket, SOMAXCONN);
			this->addSocket(Socket("server", new_socket));
		}
	}

	void handleSockets()
	{
		int select = this->selectCall();
		if (select > 0)
			this->handleOperations(select);
	}

private:
	std::vector<Socket> sockets;
	std::map<Socket, std::string> requests;
	Json::Json config;
	struct timeval tv
	{
		tv.tv_sec = 1, tv.tv_usec = 0
	};
	fd_set read_set;
	fd_set write_set;

	void fillSets()
	{
		FD_ZERO(&this->read_set);
		FD_ZERO(&this->write_set);
		for (auto socket : this->sockets)
		{
			if (socket.getType() == "server")
				FD_SET(socket.getSocket(), &this->read_set);
			else if (socket.getType() == "read")
				FD_SET(socket.getSocket(), &this->read_set);
			else if (socket.getType() == "write")
				FD_SET(socket.getSocket(), &this->write_set);
		}
	}

	int getRange()
	{
		int max = 0;
		for (auto socket : this->sockets)
		{
			if (socket.getSocket() + 1 > max)
				max = socket.getSocket() + 1;
		}
		return (max);
	}

	int selectCall()
	{
		this->fillSets();
		return (select(this->getRange(), &this->read_set, &this->write_set, NULL, &this->tv));
	}

	void handleOperations(int select)
	{
		for (auto socket : this->sockets)
		{
			if (socket.getType() == "server")
			{
				if (FD_ISSET(socket.getSocket(), &this->read_set))
				{
					this->acceptClient(socket);
					select--;
				}
			}
			else if (socket.getType() == "read")
			{
				if (FD_ISSET(socket.getSocket(), &this->read_set))
				{
					std::cout << "Client is ready to read" << std::endl;
					this->readClient(socket);
					select--;
				}
			}
			else if (socket.getType() == "write")
			{
				if (FD_ISSET(socket.getSocket(), &this->write_set))
				{
					this->writeClient(socket);
					select--;
				}
			}
			if (select == 0)
				return;
		}
	}

	void acceptClient(Socket server)
	{
		int client;
		struct sockaddr client_address;
		unsigned int client_address_length;

		client = accept(server.getSocket(), &client_address, &client_address_length);
		this->addSocket(Socket("read", client));
		std::cout << "Accepted client" << std::endl;
	}

	void readClient(Socket client)
	{
		char buf[257];
		std::string buffer;

		while (1)
		{
			auto ret = read(client.getSocket(), buf, 256);
			buf[ret] = '\0';
			buffer += buf;
			if (ret < 256)
				break;
		}
		this->addRequest(client, buffer);
		this->transformClient(client);
		std::cout << "Received request from client" << std::endl;
	}
	
	void writeClient(Socket client)
	{
		ResponseHandler response(client, this->config, this->requests[client]);
		response.sendResponse();
		std::cout << "Sent response to client" << std::endl;
		this->deleteRequest(client);
		this->disconnectClient(client);
	}

	void transformClient(Socket client)
	{
		if (client.getType() == "read")
		{
			this->deleteSocket(client);
			this->addSocket(Socket("write", client.getSocket()));
		}
		else if (client.getType() == "write")
		{
			this->deleteSocket(client);
			this->addSocket(Socket("read", client.getSocket()));
		}
	}

	void disconnectClient(Socket client)
	{
		close(client.getSocket());
		this->deleteSocket(client);
		std::cout << "Disconnected client" << std::endl;
	}

	void addSocket(Socket insert)
	{
		this->sockets.push_back(insert);
	}

	void deleteSocket(Socket erase)
	{
		this->sockets.erase(std::find(this->sockets.begin(), this->sockets.end(), erase));
	}

	void addRequest(Socket client, std::string request)
	{
		this->requests.insert({client, request});
	}

	void deleteRequest(Socket client)
	{
		this->requests.erase(client);
	}
};
