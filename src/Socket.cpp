/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Socket.cpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/08/26 19:00:35 by abobas        #+#    #+#                 */
/*   Updated: 2020/10/27 23:09:33 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Socket.hpp"
#include <iostream>

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
	std::cout << "entered socket " << socket << " write()" << std::endl;
	if (write(socket, value.c_str(), value.size()) < 0)
		perror("write()");
	std::cout << "finished socket " << socket << " write()" << std::endl;
}

void Socket::sendData(std::string &&value)
{
	std::cout << "entered socket " << socket << " write()" << std::endl;
	if (write(socket, value.c_str(), value.size()) < 0)
		perror("write()");
	std::cout << "finished socket " << socket << " write()" << std::endl;
}

void Socket::sendFile(std::string &path)
{
	char buf[257];
	std::string buffer;

	int fd = open(path.c_str(), O_RDONLY);
	while (1)
	{
		int ret = read(fd, buf, 256);
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
		int ret = read(socket, buf, 256);
		buf[ret] = '\0';
		buffer += buf;
		if (ret < 256)
			break;
	}
	return buffer;
}