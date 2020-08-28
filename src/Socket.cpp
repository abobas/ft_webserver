/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Socket.cpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/08/26 19:00:35 by abobas        #+#    #+#                 */
/*   Updated: 2020/08/28 20:31:10 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Socket.hpp"
#include <unistd.h>
#include <string>
#include <unistd.h>
#include <fcntl.h>

Socket::Socket(const std::string type, int socket) : type(type), socket(socket) {}

Socket::~Socket() {}

std::string Socket::getType() const
{
    return this->type;
}

void Socket::setType(const std::string new_type)
{
    this->type = new_type;
}

int Socket::getSocket() const
{
    return this->socket;
}

void Socket::sendData(std::string &value)
{
    write(this->socket, value.c_str(), value.size());
}

void Socket::sendData(std::string &&value)
{
    write(this->socket, value.c_str(), value.size());
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
    this->sendData(buffer);
}

std::string Socket::receive()
{
    char buf[257];
    std::string buffer;

    while (1)
    {
        int ret = read(this->socket, buf, 256);
        buf[ret] = '\0';
        buffer += buf;
        if (ret < 256)
            break;
    }
    return buffer;
}
