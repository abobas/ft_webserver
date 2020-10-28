/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Socket.cpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/08/26 19:00:35 by abobas        #+#    #+#                 */
/*   Updated: 2020/10/29 00:39:47 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Socket.hpp"

Socket::Socket()
{
}

Socket::Socket(std::string type, int socket) : type(type), socket_fd(socket)
{
	log = Log::getInstance();
}

int Socket::getListenSocket(int port)
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

bool Socket::closedClient()
{
	char buf[10];
	int ret = recv(socket_fd, buf, 10, MSG_PEEK);
	if (ret < 0)
		log->logError("recv()");
	else if (ret > 0)
		return false;
	return true;
}

int Socket::acceptClient()
{
	struct sockaddr client_address;
	unsigned int client_address_length = 0;

	memset(&client_address, 0, sizeof(client_address));
	return accept(getSocket(), &client_address, &client_address_length);
}

void Socket::sendData(std::string &value)
{
	if (send(socket_fd, value.c_str(), value.size(), MSG_NOSIGNAL) < 0)
		log->logError("send()");
	log->logPlain(value, 0);
}

void Socket::sendData(std::string &&value)
{
	if (send(socket_fd, value.c_str(), value.size(), MSG_NOSIGNAL) < 0)
		log->logError("send()");
	log->logPlain(value, 0);
}

void Socket::sendFile(std::string &path)
{
	char buf[257];
	std::string buffer;

	int fd = open(path.c_str(), O_RDONLY);
	if (fd < 0)
	{
		log->logError("open()");
		return;
	}
	while (1)
	{
		int ret = read(fd, buf, 256);
		if (ret < 0)
		{
			log->logError("read()");
			break ;
		}
		buf[ret] = '\0';
		buffer += buf;
		if (ret < 256)
			break;
	}
	close(fd);
	sendData(buffer);
}

std::string Socket::receive()
{
	char buf[257];
	std::string buffer;

	while (1)
	{
		int ret = recv(socket_fd, buf, 256, 0);
		if (ret <= 0)
		{
			log->logError("recv()");
			break ;
		}
		buf[ret] = '\0';
		buffer += buf;
		if (ret < 256)
			break;
	}
	log->logEntry("bytes received", buffer.size());
	return buffer;
}

std::string Socket::getType() const
{
	return type;
}

void Socket::setType(std::string new_type)
{
	type = new_type;
}

int Socket::getSocket() const
{
	return socket_fd;
}
