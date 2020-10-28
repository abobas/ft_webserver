/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Socket.cpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/08/26 19:00:35 by abobas        #+#    #+#                 */
/*   Updated: 2020/10/28 01:36:43 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Socket.hpp"

Socket::Socket()
{
}

Socket::Socket(std::string type, int socket) : type(type), socket(socket)
{
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
	return socket;
}

void Socket::sendData(std::string &value)
{
	if (send(socket, value.c_str(), value.size(), MSG_NOSIGNAL) < 0)
		perror("send()");
}

void Socket::sendData(std::string &&value)
{
	if (send(socket, value.c_str(), value.size(), MSG_NOSIGNAL) < 0)
		perror("send()");
}

void Socket::sendFile(std::string &path)
{
	char buf[257];
	std::string buffer;

	int fd = open(path.c_str(), O_RDONLY);
	if (fd < 0)
	{
		perror("open()");
		return;
	}
	while (1)
	{
		int ret = read(fd, buf, 256);
		if (ret == -1)
		{
			perror("read()");
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
		int ret = recv(socket, buf, 256, 0);
		if (ret == -1)
		{
			perror("recv()");
			break ;
		}
		buf[ret] = '\0';
		buffer += buf;
		if (ret < 256)
			break;
	}
	return buffer;
}