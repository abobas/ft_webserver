/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Server.cpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/07/04 14:18:19 by abobas        #+#    #+#                 */
/*   Updated: 2020/07/05 16:09:53 by abobas        ########   odam.nl         */
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

/*  
	listen_socket() creates the main socket which will listen on specified port(s)
	for incoming requests and handles connections by opening new sockets
*/

void Server::CreateServerSocket()
{
	int new_socket;
	sockaddr_in new_address;
	
	for (int i = 0; i < SERVER_SOCKET_COUNT; i++)
	{
		if ((new_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
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
	}
}

void Server::AcceptConnections()
{
	std::cout << "Server is now accepting connections" << std::endl;
	while (1)
	{
		this->InitializeSets();
		this->select_value = select(this->GetSocketRange(), &this->read_set, &this->write_set, NULL, NULL);
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
	for (u_int32_t i = 0; i < this->server_sockets.size(); i++)
	{
		if (FD_ISSET(this->server_sockets[i], &this->read_set))
		{
			this->AcceptClient(this->server_sockets[i]);
			FD_CLR(this->server_sockets[i], &this->read_set);
			this->select_value--;
			if (this->select_value == 0)
				return ;
		}
	}
	for (u_int32_t i = 0; i < this->client_sockets_r.size(); i++)
	{
		if (FD_ISSET(this->client_sockets_r[i], &this->read_set))
		{
			this->ReceiveRequest(this->client_sockets_r[i]);
			FD_CLR(this->client_sockets_r[i], &this->read_set);
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
}

void Server::ReceiveRequest(int client_socket)
{
	char buf[513];
	std::string buffer;
	int ret = 1;
	while (ret > 0)
	{
		ret = read(client_socket, buf, 512);
		if (ret == -1)
			throw strerror(errno);
		buf[ret] = '\0';
		buffer += buf;
	}
	this->requests.insert(std::pair<int, std::string>(client_socket, buffer));
	this->TransformClient(client_socket);
	std::cout << "amount of requests: " << this->requests.size() << std::endl;
	for (std::map<int, std::string>::iterator it = this->requests.begin(); it != this->requests.end(); it++)
    	std::cout << it->first << " => " << it->second << '\n';
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
	for (u_int32_t i = 0; i < this->client_sockets_r.size(); i++)
		close(this->client_sockets_r[i]);
	for (u_int32_t i = 0; i < this->client_sockets_w.size(); i++)
		close(this->client_sockets_w[i]);
}

Server::~Server()
{
	this->CloseSockets();
}
