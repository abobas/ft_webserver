/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Server.cpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/07/04 14:18:19 by abobas        #+#    #+#                 */
/*   Updated: 2020/07/07 20:26:43 by abobas        ########   odam.nl         */
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

/*
	When Server object is created it creates the basic listening sockets which will be ready for accepting client requests
*/

Server::Server()
{
	this->createServerSocket();
}

/*
	Creates server sockets and binds them to specified ports which are set in the server configuration file
	All server sockets (file descriptors of type int) are saved in server_sockets vector
*/

void Server::createServerSocket()
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
		new_address.sin_addr.s_addr = INADDR_ANY;                              							 		// no need to convert to big-endian because all 0 bytes // need to convert to big-endian otherwise
		new_address.sin_port = ((PORT & 0xFF) << 8);                            								// converted to big-endian
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

/*
	This public methods starts the runtime of the server
	Every iteration it adds all open sockets (server sockets, client read sockets and client write sockets) to their respective fd_sets (read_set and write_set)
	The select() function checks these fd_sets for which sockets are ready for read/write operations
	The return value of select is the amount of sockets that are ready, without specifying which ones are ready
*/

void Server::acceptConnections()
{
	std::cout << "Server is now accepting connections" << std::endl;
	while (1)
	{
		this->initializeSets();
		this->select_value = select(this->getSocketRange(), &this->read_set, &this->write_set, NULL, NULL);
		std::cout << this->select_value << " sockets are ready for operations" << std::endl;
		if (this->select_value == -1)
			throw strerror(errno);
		else if (this->select_value > 0)
			this->handleConnections();
	}
}

/*
	Every iteration of the runtime all open sockets have to be added to the fd_sets (read_set and write_set)
	The sets are first cleaned (FD_ZERO), and then all open sockets are added (server_sockets, client read sockets, client write sockets) to either read fd_set or write fd_set
*/

void Server::initializeSets()
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

/*
	For select() to work properly you have to specify a range of sockets (file descriptors) to check
	This is simply a range from 0 to the highest value open file descriptor + 1
	getSocketRange() checks all server sockets, client read sockets and client write sockets for the highest open file descriptor and returns it
*/

int Server::getSocketRange()
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

/*
	When select() returns the amount of sockets ready for operations we have to find which sockets are ready for either read or write operations with individual FD_ISSET calls
	Depending on which socket is ready to read / write we have to determine our course of action
	The read and write fd_sets consist of three different type of sockets: server sockets and client sockets which are either read or write
	The read fd_set consists of the server sockets and the client read sockets
	The write fd_set only consists of client write sockets
	1) Server sockets are the sockets which accept incoming connections, by accepting an incoming connection a new client socket is created
	2) Client sockets start out as read client sockets, as we first have to retrieve their HTTP request
	3) After receiving the request from a client socket, we transform the client read socket into client write socket, waiting for our response to be written into the socket
	Depending on the HTTP request header we either close the client socket after sending our response or transform the client write socket back to a client read socket to receive the next request
*/

void Server::handleConnections()
{
	for (u_int32_t i = 0; i < this->client_sockets_w.size(); i++)
	{
		std::cout << "Checking if flag is set for writing to client FD " <<  this->client_sockets_w[i] << std::endl;
		if (FD_ISSET(this->client_sockets_w[i], &this->write_set))
		{
			std::cout << "Flag is set for writing to client FD " <<  this->client_sockets_w[i] << std::endl;
			this->sendResponse(this->client_sockets_w[i]);
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
			this->receiveRequest(this->client_sockets_r[i]);
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
			this->acceptClient(this->server_sockets[i]);
			FD_CLR(this->server_sockets[i], &this->read_set);
			this->select_value--;
			if (this->select_value == 0)
				return ;
		}
	}
}

/*
	Here we accept an incoming connection which is queued on a server socket
	By accepting an incoming connection a new client read socket is created
	The newly created client socket (file descriptor of type int) is pushed into the client_sockets_r vector
*/

void Server::acceptClient(int server_socket)
{
	int client;
	struct sockaddr client_address;
	unsigned int client_address_length;
	if ((client = accept(server_socket, &client_address, &client_address_length)) < 0)
		throw strerror(errno);
	this->client_sockets_r.push_back(client);
	std::cout << "Connection established with client. Client socket is FD " << client << std::endl;
}

/*
	When a client read socket is ready for read operations the HTTP request is read from its socket
	The client read socket is transformed into a client write socket (erased from read vector and added to write vector)
	When the socket becomes ready for write operations it will show during the next iteration of select()
	The request is written into a map as a string, paired with its socket identifier (file descriptor int)
*/

void Server::receiveRequest(int client_socket)
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
	this->transformClient(client_socket);
	std::cout << "Received request from client socket FD " << client_socket << std::endl;
	std::cout << "\nRECEIVED REQUEST >>>>>>\n" << buffer << std::endl;
}

/*
	When the client write socket is ready for write operations we send our response to its request
	We either close the socket or keep it alive if specified by transforming it back to a client read socket
*/

void Server::sendResponse(int client_socket)
{
	std::string StandardReply("HTTP/1.1 200 OK\nContent-Length: 12\nContent-Type: text/html\n\nHello World\n");
	write(client_socket, StandardReply.c_str(), StandardReply.size());
	std::cout << "Sent response to client socket FD " << client_socket << std::endl;
	close(client_socket);
	this->client_sockets_w.erase(std::find(this->client_sockets_w.begin(), this->client_sockets_w.end(), client_socket));
}

/*
	Helper function to transform a client socket from read client write client socket by erasing it from the read client vector and adding it to the write client vector
*/

void Server::transformClient(int client_socket)
{
	this->client_sockets_r.erase(std::find(this->client_sockets_r.begin(), this->client_sockets_r.end(), client_socket));
	this->client_sockets_w.push_back(client_socket);
}

/*
	On termination of the program we want to close all open sockets
*/

void Server::closeSockets()
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
	this->closeSockets();
}
