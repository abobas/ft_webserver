/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Socket.cpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/08/26 19:00:35 by abobas        #+#    #+#                 */
/*   Updated: 2020/10/31 22:34:03 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Socket.hpp"

std::string CRLF = "\r\n";

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
	char buf[1];
	int ret = recv(socket_fd, buf, 1, MSG_PEEK);
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

bool Socket::endOfHeaders()
{
	if (message.find("\r\n\r\n") != std::string::npos)
		return true;
	return false;
}

bool Socket::endOfChunked()
{
	if (message.find("0\r\n\r\n") != std::string::npos)
		return true;
	return false;
}

bool Socket::isChunked()
{
	std::string lower;

	for (auto c : message.substr())
		lower += static_cast<char>(std::tolower(c));
	if (lower.find("transfer-encoding: chunked") != std::string::npos)
		return true;
	return false;
}

void Socket::decodeChunkedBody(std::string &body)
{
	std::string decoded;
	std::string chunk;
	size_t pos_start;
	size_t pos_end;
	
	while (body.find("\r\n") != std::string::npos)
	{
		pos_start = body.find("\r\n");
		pos_end = body.find("\r\n", pos_start + 1);
		chunk = body.substr(pos_start + 2, pos_end - (pos_start + 2));
		decoded += chunk;
		body = body.substr(pos_end + 2);
	}
	body = decoded;
	log->logEntry("chunked content size (decoded) is", body.size());
}

void Socket::cleanBody()
{
	std::string header_part;
	std::string body_part;

	header_part = message.substr(0, message.find("\r\n\r\n") + 4);
	body_part = message.substr(header_part.size());
	decodeChunkedBody(body_part);
	message = header_part + body_part;
}

bool Socket::isContent()
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
		return true;
	}
	return false;
}

bool Socket::endOfContent()
{
	std::string header_part;
	std::string body_part;

	header_part = message.substr(0, message.find("\r\n\r\n") + 4);
	body_part = message.substr(header_part.size());
	if (body_part.size() == content_size)
		return true;
	return false;
}

void Socket::receiveData()
{
	std::string buffer = readSocket();
	message += buffer;
	if (!headers_read)
	{
		if (endOfHeaders())
		{
			headers_read = true;
			chunked = isChunked();
			if (chunked)
				log->logEntry("request is chunked");
			content = isContent();
			if (content)
			{
				log->logEntry("request has content");
				log->logEntry("content size is", content_size);
			}
		}
	}
	if (headers_read)
	{
		if (chunked)
		{
			if (endOfChunked())
			{
				cleanBody();
				end_of_file = true;
			}
		}
		else if (content)
		{
			if (endOfContent())
				end_of_file = true;
		}
		else
			end_of_file = true;
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

std::string Socket::readSocket()
{
	char buf[1025];
	std::string buffer;

	while (1)
	{
		int ret = recv(socket_fd, buf, 1024, 0);
		if (ret <= 0)
		{
			log->logError("recv()");
			break ;
		}
		buf[ret] = '\0';
		buffer += buf;
		if (ret < 1024)
			break;
	}
	return buffer;
}

void Socket::cleanSocket()
{
	message.clear();
	chunked = false;
	content = false;
	headers_read = false;
	end_of_file = false;
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

std::string Socket::getMessage() const
{
	return message;
}

bool Socket::getEndOfFile() const
{
	return end_of_file;
}