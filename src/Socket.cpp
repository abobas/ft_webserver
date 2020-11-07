/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Socket.cpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/08/26 19:00:35 by abobas        #+#    #+#                 */
/*   Updated: 2020/11/07 13:22:52 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Socket.hpp"

Log *Socket::log = Log::getInstance();
Json Socket::config = Json();

void Socket::initializeSocket(Json &config)
{
	Socket::config = config;
	Evaluator::initializeEvaluator(config);
}

Socket::Socket()
{
}

Socket::Socket(std::string type, int socket) : type(type), socket(socket)
{
}

void Socket::handleIncoming()
{
	receiver = Receiver::getInstance(socket);
	if (!receiver->headersReceived())
		receiver->receiveHeaders();
	if (receiver->headersReceived())
	{
		evaluator = Evaluator::getInstance(socket);
		if (!evaluator->isEvaluated())
			evaluator->evaluateHeaders(receiver->getHeaders());
		if (evaluator->isEvaluated())
		{
			if (!evaluator->isProcessed())
				evaluator->processRequest();
			if (evaluator->isProcessed())
			{
				log->logEntry("processed socket", socket);
				Receiver::deleteInstance(socket);
				setType("client_write");
				return;
			}
		}
	}
}

void Socket::handleOutgoing()
{
	resolver = Resolver::getInstance(socket, evaluator);
	if (!resolver->isResolved())
		resolver->resolveRequest();
	if (resolver->isResolved())
	{
		log->logEntry("resolved client", socket);
		Resolver::deleteInstance(socket);
		setType("client_read");
	}
}

bool Socket::isAlive()
{
	char buf[1];
	int ret = recv(socket, buf, 1, MSG_PEEK);
	if (ret < 0)
		log->logError("recv()");
	else if (ret > 0)
		return true;
	return false;
}

std::string Socket::getType()
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
