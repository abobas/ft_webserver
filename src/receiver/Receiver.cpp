/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Receiver.cpp                                       :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/11/01 23:35:17 by abobas        #+#    #+#                 */
/*   Updated: 2020/11/02 22:32:06 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Receiver.hpp"

#define IO_SIZE 1048576

std::map<int, Receiver*> Receiver::receivers;
Log *Receiver::log = Log::getInstance();

Receiver *Receiver::getInstance(int socket) noexcept
{
	if (!receivers[socket])
		receivers[socket] = new Receiver(socket);
	return receivers[socket];
}

void Receiver::deleteInstance(int socket)
{
	if (receivers[socket])
	{
		log->logEntry("deleting receiver", socket);
		delete receivers[socket];
		receivers[socket] = NULL;
		log->logEntry("deleted receiver", socket);
	}
}

Receiver::Receiver(int socket) : socket(socket)
{
}

void Receiver::receiveMessage()
{
	std::string buffer;

	readSocket(buffer);
	message.append(buffer);
	if (!headers_ready)
	{
		if (headersReceived())
		{
			headers_ready = true;
			checkChunked();
			checkContent();
		}
	}
	if (headers_ready)
	{
		if (chunked)
			checkChunkedReady();
		else if (content)
			checkContentReady();
		else
			ready = true;
	}
}

void Receiver::readSocket(std::string &buffer)
{
	char buf[IO_SIZE + 1];

	while (true)
	{
		int ret = recv(socket, buf, IO_SIZE, 0);
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

void Receiver::decodeChunkedMessage()
{
	std::string header_part;
	std::string body_part;
	std::string decoded;
	std::string chunk;
	size_t pos_start;
	size_t pos_end;

	header_part = message.substr(0, message.find("\r\n\r\n") + 4);
	body_part = message.substr(header_part.size());
	while (body_part.find("\r\n") != std::string::npos)
	{
		pos_start = body_part.find("\r\n");
		pos_end = body_part.find("\r\n", pos_start + 1);
		chunk = body_part.substr(pos_start + 2, pos_end - (pos_start + 2));
		decoded += chunk;
		body_part = body_part.substr(pos_end + 2);
	}
	message.clear();
	message.append(header_part);
	message.append(decoded);
}

void Receiver::checkChunkedReady()
{
	std::string end;

	end = message.substr(message.size() - 7);
	if (end.find("0\r\n\r\n") != std::string::npos)
	{
		ready = true;
		decodeChunkedMessage();
	}
}

void Receiver::checkChunked()
{
	std::string lower;

	for (auto c : message.substr())
		lower += static_cast<char>(std::tolower(c));
	if (lower.find("transfer-encoding: chunked") != std::string::npos)
	{
		chunked = true;
		log->logEntry("message is chunked");
	}
}

void Receiver::checkContentReady()
{
	std::string header_part;
	std::string body_part;

	header_part = message.substr(0, message.find("\r\n\r\n") + 4);
	body_part = message.substr(header_part.size());
	if (body_part.size() == content_size)
		ready = true;
}

void Receiver::checkContent()
{
	std::string lower;
	size_t pos;

	for (auto c : message.substr())
		lower += static_cast<char>(std::tolower(c));
	pos = lower.find("content-length:");
	if (pos != std::string::npos)
	{
		size_t end_pos = lower.find("\r\n", pos);
		content_size = std::stoi(lower.substr(pos + 15, end_pos - (pos + 15)));
		content = true;
		log->logEntry("message has content");
	}
}

bool Receiver::headersReceived()
{
	if (message.find("\r\n\r\n") != std::string::npos)
		return true;
	return false;
}

void Receiver::getMessage(std::string &buffer)
{
	buffer = std::move(message);
}

bool Receiver::isReady()
{
	return ready;
}