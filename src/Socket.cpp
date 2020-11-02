/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Socket.cpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/08/26 19:00:35 by abobas        #+#    #+#                 */
/*   Updated: 2020/11/02 22:32:23 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Socket.hpp"

Log *Socket::log = Log::getInstance();

Socket::Socket()
{
}

Socket::Socket(std::string type, int socket) : type(type), socket_fd(socket)
{
}

bool Socket::isAlive()
{
	char buf[1];
	int ret = recv(socket_fd, buf, 1, MSG_PEEK);
	if (ret < 0)
		log->logError("recv()");
	else if (ret > 0)
		return true;
	return false;
}

void Socket::receiveMessage()
{
	log->logEntry("receiveMessage called socket", socket_fd);
	receiver = Receiver::getInstance(socket_fd);
	receiver->receiveMessage();
	if (receiver->isReady())
	{
		received = true;
		receiver->getMessage(message);
		receiver->deleteInstance(socket_fd);
	}
}

void Socket::sendData(std::string &value)
{
	if (send(socket_fd, value.c_str(), value.size(), MSG_NOSIGNAL) < 0)
		log->logError("send()");
	//log->logBlock(value);
}

void Socket::sendData(std::string &&value)
{
	if (send(socket_fd, value.c_str(), value.size(), MSG_NOSIGNAL) < 0)
		log->logError("send()");
	//log->logBlock(value);
}

std::string Socket::getMessage()
{
	return message;
}

bool Socket::isReady()
{
	return received;
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

