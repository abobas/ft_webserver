/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Receiver.cpp                                       :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/11/01 23:35:17 by abobas        #+#    #+#                 */
/*   Updated: 2020/11/06 16:56:48 by abobas        ########   odam.nl         */
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
	if (body_part.empty())
		return;
	if (content)
	{
		body_data = std::move(body_part);
		content_received += body_data.size();
		if (content_received == content_length)
		{
			log->logEntry("body content fully received");
			body_received = true;
		}
	}
	else if (chunked)
	{
		if (chunkedEnd())
		{
			log->logEntry("full chunked body received");
			body_data = std::move(body_part);
			decodeChunkedBody();
			body_received = true;
			return;
		}
		splitChunked();
		log->logEntry("split received chunks");
		log->logEntry("body_data size", body_data.size());
		log->logEntry("body_part size", body_part.size());
		//log->logBlock(body_data);
		decodeChunkedBody();
		//log->logBlock(body_data);
		return;
	}
}

void Receiver::decodeChunkedBody()
{
	std::string decoded;
	std::string chunk;
	size_t pos_start;
	size_t pos_end;

	decoded.reserve(body_data.size());
	while (body_data.find("\r\n") != std::string::npos)
	{
		pos_start = body_data.find("\r\n");
		pos_end = body_data.find("\r\n", pos_start + 1);
		chunk = std::move(body_data.substr(pos_start + 2, pos_end - (pos_start + 2)));
		decoded += chunk;
		body_data.erase(0, pos_end + 2);
	}
	body_data = std::move(decoded);
}

void Receiver::splitChunked()
{
	size_t pos = 0;
	size_t ret = 0;
	int count = 0;

	while (ret != std::string::npos)
	{
		ret = body_part.find("\r\n", ret);
		if (ret != std::string::npos)
		{
			count++;
			ret++;
		}
		if (count % 2 == 0)
		{
			count = 0;
			pos = ret + 1;
		}
	}
	if (pos == 0)
		return;
	std::string tmp = std::move(body_part.substr(pos));
	body_data = std::move(body_part.erase(pos));
	body_part = std::move(tmp);
}

bool Receiver::chunkedEnd()
{
	std::string end;

	if (body_part.size() < 5)
		return false;
	end = body_part.substr(body_part.size() - 5);
	if (end.find("0\r\n\r\n") != std::string::npos)
		return true;
	return false;
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

	int ret = recv(socket, buf, IO_SIZE, MSG_DONTWAIT);
	if (ret < 0)
		log->logError("recv()");
	if (ret > 0)
	{
		buf[ret] = '\0';
		buffer += buf;
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
