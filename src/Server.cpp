/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Server.cpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/07/04 14:18:19 by abobas        #+#    #+#                 */
/*   Updated: 2020/07/07 17:16:47 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "includes/Server.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#define PORT 80
#define SERVER_SOCKET_COUNT 1

Server::Server()
{
	this->CreateServerSocket();
}

void Server::CreateServerSocket()
{
	int new_socket;
	sockaddr_in new_address;
	
	for (int i = 0; i < SERVER_SOCKET_COUNT; i++)
	{
		if ((new_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
			throw strerror(errno);
		int enable = 1;
		if (setsockopt(new_socket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
			throw strerror(errno);
		new_address.sin_family = AF_INET; 
		new_address.sin_addr.s_addr = INADDR_ANY;                               // no need to convert to big-endian because all 0 bytes
		new_address.sin_port = ((PORT & 0xFF) << 8);                            // converted to big-endian
		for (uint32_t i = 0; i < sizeof(new_address.sin_zero); i++)
				new_address.sin_zero[i] = 0;
		if (bind(new_socket, reinterpret_cast<sockaddr*>(&new_address), sizeof(new_address)) < 0) 
			throw strerror(errno);
		if (listen(new_socket, SOMAXCONN) < 0) 
				throw strerror(errno);
		this->server_sockets.push_back(new_socket);
		std::cout << "Opened a server socket on FD " << new_socket << " which is listening on Port " << PORT << std::endl;
	}
}

void Server::AcceptConnections()
{
	std::cout << "Server is now accepting connections" << std::endl;
	while (1)
	{
		this->InitializeSets();
		this->select_value = select(this->GetSocketRange(), &this->read_set, &this->write_set, NULL, NULL);
		std::cout << this->select_value << " sockets are ready for operations" << std::endl;
		if (this->select_value == -1)
			throw strerror(errno);
		else if (this->select_value > 0)
			this->HandleConnections();
	}
}

void Server::InitializeSets()
{
	FD_ZERO(&this->read_set);
	FD_ZERO(&this->write_set);
	for (u_int32_t i = 0; i < this->server_sockets.size(); i++)
		FD_SET(this->server_sockets[i], &this->read_set);
	for (u_int32_t i = 0; i < this->client_sockets_r.size(); i++)
		FD_SET(this->client_sockets_r[i], &this->read_set);
	for (u_int32_t i = 0; i < this->client_sockets_w.size(); i++)
		FD_SET(this->client_sockets_w[i], &this->write_set);
}

int Server::GetSocketRange()
{
	int max = 0;
	for (u_int32_t i = 0; i < this->server_sockets.size(); i++)
	{
		if (this->server_sockets[i] + 1 > max)
			max = this->server_sockets[i] + 1;
	}
	for (u_int32_t i = 0; i < this->client_sockets_r.size(); i++)
	{
		if (this->client_sockets_r[i] + 1 > max)
			max = this->client_sockets_r[i] + 1;
	}
	for (u_int32_t i = 0; i < this->client_sockets_w.size(); i++)
	{
		if (this->client_sockets_w[i] + 1 > max)
			max = this->client_sockets_w[i] + 1;
	}
	return (max);
}

void Server::HandleConnections()
{
	for (u_int32_t i = 0; i < this->client_sockets_w.size(); i++)
	{
		std::cout << "Checking if flag is set for writing to client FD " <<  this->client_sockets_w[i] << std::endl;
		if (FD_ISSET(this->client_sockets_w[i], &this->write_set))
		{
			std::cout << "Flag is set for writing to client FD " <<  this->client_sockets_w[i] << std::endl;
			this->SendResponse(this->client_sockets_w[i]);
			FD_CLR(this->client_sockets_w[i], &this->write_set);
			this->select_value--;
			if (this->select_value == 0)
				return ;
		}
	}
	for (u_int32_t i = 0; i < this->client_sockets_r.size(); i++)
	{
		std::cout << "Checking if flag is set for reading from client FD " <<  this->client_sockets_r[i] << std::endl;
		if (FD_ISSET(this->client_sockets_r[i], &this->read_set))
		{
			std::cout << "Flag is set for reading from client FD " <<  this->client_sockets_r[i] << std::endl;
			this->ReceiveRequest(this->client_sockets_r[i]);
			FD_CLR(this->client_sockets_r[i], &this->read_set);
			this->select_value--;
			if (this->select_value == 0)
				return ;
		}
	}
	for (u_int32_t i = 0; i < this->server_sockets.size(); i++)
	{
		std::cout << "Checking if flag is set for server socket FD " <<  this->server_sockets[i] << std::endl;
		if (FD_ISSET(this->server_sockets[i], &this->read_set))
		{
			std::cout << "Flag is set for server FD " <<  this->server_sockets[i] << std::endl;
			this->AcceptClient(this->server_sockets[i]);
			FD_CLR(this->server_sockets[i], &this->read_set);
			this->select_value--;
			if (this->select_value == 0)
				return ;
		}
	}
}

void Server::AcceptClient(int server_socket)
{
	int client;
	struct sockaddr client_address;
	unsigned int client_address_length;
	if ((client = accept(server_socket, &client_address, &client_address_length)) < 0)
		throw strerror(errno);
	this->client_sockets_r.push_back(client);
	std::cout << "Connection established with client. Client socket is FD " << client << std::endl;
}

void Server::ReceiveRequest(int client_socket)
{
	char buf[513];
	std::string buffer;
	int ret = 1;
	while (1)
	{
		ret = read(client_socket, buf, 512);
		if (ret == -1)
			throw strerror(errno);
		buf[ret] = '\0';
		buffer += buf;
		if (ret < 512)
			break ;
	}
	this->requests.insert(std::pair<int, std::string>(client_socket, buffer));
	this->TransformClient(client_socket);
	std::cout << "Received request from client socket FD " << client_socket << std::endl;
	std::cout << "\nRECEIVED REQUEST >>>>>>\n" << buffer << std::endl;
}

void Server::SendResponse(int client_socket)
{
	/*
	Bullshit response sent just to check out socket connectivity
	Connection is always closed after response, but have to implement longevity of connection based on HTTP request (keep-alive / close)
	 */
	std::string StandardReply("HTTP/1.1 200 OK\nContent-Length: 12\nContent-Type: text/html\n\nHello World\n");
	write(client_socket, StandardReply.c_str(), StandardReply.size());
	std::cout << "Sent response to client socket FD " << client_socket << std::endl;
	close(client_socket);
	this->client_sockets_w.erase(std::find(this->client_sockets_w.begin(), this->client_sockets_w.end(), client_socket));
}

void Server::PrintRequests()
{
	for (std::map<int, std::string>::iterator it = this->requests.begin(); it != this->requests.end(); it++)
		std::cout << "Client FD " << it->first << " sent the following request:\n\n" << it->second << std::endl;
}

void Server::TransformClient(int client_socket)
{
	this->client_sockets_r.erase(std::find(this->client_sockets_r.begin(), this->client_sockets_r.end(), client_socket));
	this->client_sockets_w.push_back(client_socket);
}

void Server::CloseSockets()
{
	for (u_int32_t i = 0; i < this->server_sockets.size(); i++)
		close(this->server_sockets[i]);
	this->server_sockets.clear();
	for (u_int32_t i = 0; i < this->client_sockets_r.size(); i++)
		close(this->client_sockets_r[i]);
	this->client_sockets_r.clear();
	for (u_int32_t i = 0; i < this->client_sockets_w.size(); i++)
		close(this->client_sockets_w[i]);
	this->client_sockets_w.clear();
}

Server::~Server()
{
	this->CloseSockets();
}
