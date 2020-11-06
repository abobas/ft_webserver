/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Receiver.cpp                                       :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/11/01 23:35:17 by abobas        #+#    #+#                 */
/*   Updated: 2020/11/06 00:56:28 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Receiver.hpp"

#define IO_SIZE 1048576

std::map<int, Receiver *> Receiver::receivers;
Log *Receiver::log = Log::getInstance();
Json Receiver::config = Json();

void Receiver::initializeReceiver(Json &config)
{
	Receiver::config = config;
}

Receiver::Receiver(int socket) : socket(socket)
{
	content_received = 0;
}

Receiver *Receiver::getInstance(int socket) noexcept
{
	if (!receivers[socket])
	{
		receivers[socket] = new Receiver(socket);
		log->logEntry("created receiver", socket);
	}
	return receivers[socket];
}

void Receiver::deleteInstance(int socket)
{
	if (receivers[socket])
	{
		delete receivers[socket];
		receivers[socket] = NULL;
		log->logEntry("deleted receiver", socket);
	}
}

void Receiver::receiveBody()
{
	std::string buffer;

	readSocket(buffer);
	body_part.append(buffer);
	if (content)
	{
		body_data = std::move(body_part);
		content_received += body_data.size();
		log->logEntry("content received", content_received);
		if (content_received == content_length)
		{
			log->logEntry("body content fully received");
			body_received = true;
		}
	}
	else if (chunked)
	{
		// decode stuff
		return ;
	}
}

const char *Receiver::getBodyData()
{
	return body_data.c_str();
}

size_t Receiver::getBodyDataLength()
{
	return body_data.size();
}

bool Receiver::bodyInitialized()
{
	return body_initialized;
}

bool Receiver::bodyReceived()
{
	return body_received;
}

void Receiver::initializeBodyType(std::string type, size_t length)
{
	if (type == "chunked")
		chunked = true;
	else if (type == "content")
	{
		content = true;
		content_length = length;
		log->logEntry("content length is", content_length);
	}
	body_initialized = true;
}

void Receiver::readSocket(std::string &buffer)
{
	char buf[IO_SIZE + 1];

	while (true)
	{
		int ret = recv(socket, buf, IO_SIZE, MSG_DONTWAIT);
		if (ret < 0)
		{
			log->logError("recv()");
			break;
		}
		if (ret > 0)
		{
			buf[ret] = '\0';
			buffer += buf;
		}
		if (ret < IO_SIZE)
			break;
	}
}

void Receiver::receiveHeaders()
{
	std::string buffer;

	readSocket(buffer);
	message.append(buffer);
	if (checkHeadersReceived())
	{
		headers_received = true;
		splitMessage();
		log->logBlock(headers_part);
		// log->logBlock(body_part);
	}
}

void Receiver::splitMessage()
{
	headers_part = message.substr(0, message.find("\r\n\r\n") + 4);
	body_part = message.substr(headers_part.size());
}

bool Receiver::checkHeadersReceived()
{
	if (message.find("\r\n\r\n") != std::string::npos)
		return true;
	return false;
}

bool Receiver::headersReceived()
{
	return headers_received;
}

std::string Receiver::getHeaders()
{
	return headers_part;
}



// /**
// * @brief Moves the received message into the buffer and deletes the receiver instance.
// * @param buffer reference to string which the message will be moved into.
// */
// void Receiver::consumeInstance(std::string &buffer)
// {
// 	buffer = std::move(message);
// 	deleteInstance(socket);
// }




// void Receiver::decodeChunkedMessage()
// {
// 	std::string header_part;
// 	std::string body_part;
// 	std::string decoded;
// 	std::string chunk;
// 	size_t pos_start;
// 	size_t pos_end;

// 	header_part = message.substr(0, message.find("\r\n\r\n") + 4);
// 	body_part = message.substr(header_part.size());
// 	while (body_part.find("\r\n") != std::string::npos)
// 	{
// 		pos_start = body_part.find("\r\n");
// 		pos_end = body_part.find("\r\n", pos_start + 1);
// 		chunk = body_part.substr(pos_start + 2, pos_end - (pos_start + 2));
// 		decoded += chunk;
// 		body_part = body_part.substr(pos_end + 2);
// 	}
// 	message.clear();
// 	message.append(header_part);
// 	message.append(decoded);
// }

// void Receiver::checkChunkedReady()
// {
// 	std::string end;

// 	end = message.substr(message.size() - 7);
// 	if (end.find("0\r\n\r\n") != std::string::npos)
// 	{
// 		ready = true;
// 		decodeChunkedMessage();
// 	}
// }


// void Receiver::checkContentReady()
// {
// 	std::string header_part;
// 	std::string body_part;

// 	header_part = message.substr(0, message.find("\r\n\r\n") + 4);
// 	body_part = message.substr(header_part.size());
// 	if (body_part.size() == content_size)
// 		ready = true;
// }

