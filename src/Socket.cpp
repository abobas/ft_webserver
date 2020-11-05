/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Socket.cpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/08/26 19:00:35 by abobas        #+#    #+#                 */
/*   Updated: 2020/11/05 16:37:46 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Socket.hpp"

Log *Socket::log = Log::getInstance();
Json Socket::config = Json();

Socket::Socket()
{
}

Socket::Socket(std::string type, int socket) : type(type), socket(socket)
{
}

Socket::~Socket()
{
	log->logEntry("deleting socket", socket);
	Receiver::deleteInstance(socket);
	Evaluator::deleteInstance(socket);
	Processor::deleteInstance(socket);	
}

void Socket::initializeSocket(Json &config)
{
	Socket::config = config;
}

void Socket::handleIncoming()
{
	receiver = Receiver::getInstance(socket);
	if (!receiver->headersReceived())
	{
		receiver->receiveHeaders();
		log->logEntry("received headers socket", socket);
		return ;
	}
	if (receiver->headersReceived())
	{
		evaluator = Evaluator::getInstance(socket);
		if (!evaluator->isEvaluated())
		{
			evaluator->evaluateHeaders(receiver->getHeaders());
			log->logEntry("evaluated socket", socket);
		}
		if (evaluator->isEvaluated())
		{
			if (evaluator->isProcessed())
			{
				log->logEntry("processed socket", socket);
				Receiver::deleteInstance(socket);
				Processor::deleteInstance(socket);
				// transformSocket(); // op socket niveau ipv server?
				return;
			}
			log->logEntry("processing socket", socket);
			evaluator->processRequest();
		}
	}
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

// void Socket::receiveMessage()
// {
// 	//log->logEntry("receiveMessage called socket", socket);
// 	receiver = Receiver::getInstance(socket);
// 	receiver->receiveMessage();
// 	received = receiver->isReady();
// 	if (received)
// 		receiver->consumeInstance(message);
// }

// bool Socket::isAlive()
// {
// 	char buf[1];
// 	int ret = recv(socket, buf, 1, MSG_PEEK);
// 	if (ret < 0)
// 		log->logError("recv()");
// 	else if (ret > 0)
// 		return true;
// 	return false;
// }

// std::string Socket::getMessage()
// {
// 	return message;
// }

// bool Socket::isReady()
// {
// 	return received;
// }

