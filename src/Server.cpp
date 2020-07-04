/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Server.cpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/07/04 14:18:19 by abobas        #+#    #+#                 */
/*   Updated: 2020/07/04 21:42:25 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "includes/Server.hpp"
#include <iostream>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#define PORT 80

Server::Server()
{
	FD_ZERO(&this->read_set);
	FD_ZERO(&this->write_set);
	this->create_listen_socket();
	this->accept_connections();
}

/*  
	listen_socket() creates the main socket which will listen on specified port(s)
	for incoming requests and handles connections by opening new sockets
*/

void Server::create_listen_socket()
{
	if ((this->listen_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		throw strerror(errno);
	else
		std::cout << "Socket succesfully created" << std::endl;
	sockaddr_in main_address;
	main_address.sin_family = AF_INET; 
	main_address.sin_addr.s_addr = INADDR_ANY;                               // no need to convert to big-endian because all 0 bytes
	main_address.sin_port = ((PORT & 0xFF) << 8);                            // converted to big-endian
	for (uint32_t i = 0; i < sizeof(main_address.sin_zero); i++)
		main_address.sin_zero[i] = 0;
	if (bind(this->listen_socket, reinterpret_cast<sockaddr*>(&main_address), sizeof(main_address)) < 0) 
		throw strerror(errno);
	else
		std::cout << "Socket was succesfully assigned an address" << std::endl;
	if (listen(this->listen_socket, SOMAXCONN) < 0) 
		throw strerror(errno);
	else
		std::cout << "Socket is now listening to incoming connections" << std::endl;
	FD_SET(this->listen_socket, &this->read_set);
	this->read_sockets.push_back(this->listen_socket);
	this->set_max_fd();
}

void Server::accept_connections()
{
	int ret;
	while (1)
	{
		std::cout << "Looking for ready sockets" << std::endl;
		ret = select(this->fd_max, &this->read_set, &this->write_set, NULL, NULL);
		if (ret == -1)
			throw strerror(errno);
		else if (ret > 0)
			this->check_sets();
	}
}

void Server::check_sets()
{
	std::cout << "amount of sockets in read " << this->read_sockets.size() << std::endl;
	for (u_int32_t i = 0; i < this->read_sockets.size(); i++)
	{
		if (FD_ISSET(this->read_sockets[i], &this->read_set))
		{
			if (this->read_sockets[i] == this->listen_socket)
			{
				this->accept_client();
				return ;
			}
			else
			{
				this->receive_data(i);
			}
		}
	}
	std::cout << "Returning to accept_connections()" << std::endl;
}

void Server::accept_client()
{
	std::cout << "Accepting new client" << std::endl;
	int client_socket;
	struct sockaddr client_address;
	unsigned int client_address_length;
	if ((client_socket = accept(this->listen_socket, &client_address, &client_address_length)) < 0)
		throw strerror(errno);
	FD_SET(client_socket, &this->read_set);
	this->read_sockets.push_back(client_socket);
	std::cout << "New client socket opened on FD " << client_socket << std::endl;
	set_max_fd();
}

void Server::receive_data(int socket)
{
	char buf[6000];
	int ret = read(socket, buf, 6000);
	buf[ret] = '\0';
	std::cout << buf << std::endl;
}

void Server::set_max_fd()
{
	int max = 0;
	for (u_int32_t i = 0; i < this->read_sockets.size(); i++)
	{
		if (this->read_sockets[i] + 1 > max)
			max = this->read_sockets[i] + 1;
	}
	for (u_int32_t i = 0; i < this->write_sockets.size(); i++)
	{
		if (this->write_sockets[i] + 1 > max)
			max = this->write_sockets[i] + 1;
	}
	this->fd_max = max;
	std::cout << "Checking sockets from 0 up to " << max - 1 << std::endl;
}

Server::~Server()
{
	close(this->listen_socket);
}


// https://stackoverflow.com/questions/36262070/what-does-htons-do-on-a-big-endian-system
