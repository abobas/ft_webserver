/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Proxy.cpp                                          :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/09/06 18:32:50 by abobas        #+#    #+#                 */
/*   Updated: 2020/10/26 21:23:14 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Proxy.hpp"
#include <iostream>

static std::string lineTerminator = "\r\n";

Proxy::Proxy(Data &data) : data(data)
{
	setPath();
	if (createProxySocket())
		return ;
	std::cout << "proxy created" << std::endl;
	if (setProxyAddress())
		return;
	std::cout << "proxy address set" << std::endl;
	if (connectProxySocket())
		return;
	std::cout << "proxy connected" << std::endl;
	success = true;

	// sendProxyRequest();
	// std::cout << "proxy request sent" << std::endl;
	// receiveProxyResponse();
	// sendProxyResponse();
}

bool Proxy::proxySuccess()
{
	return success;
}

void Proxy::setPath()
{
	if (data.path.size() == 0)
		data.path = "/";
}

int Proxy::createProxySocket()
{
	int new_socket;
	int enable = 1;

	new_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (new_socket == -1)
	{
		perror("socket()");
		data.response.sendInternalError();
		return 1;
	}
	if (fcntl(new_socket, F_SETFL, O_NONBLOCK) < 0)
	{
		perror("fcntl()");
		data.response.sendInternalError();
		return 1;
	}
	if (setsockopt(new_socket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
	{
		perror("setsockopt()");
		data.response.sendInternalError();
		return 1;
	}
	proxy_socket = Socket("proxy_write", new_socket);
	return 0;
}

int Proxy::setProxyAddress()
{
	memset(&proxy_addr, 0, sizeof(proxy_addr));
	size_t pos = data.location["proxy_pass"].string_value().find(':');
	if (pos == std::string::npos)
		proxy_addr.sin_port = htons(80);
	else
		proxy_addr.sin_port = htons(std::stoi(data.location["proxy_pass"].string_value().substr(pos + 1, std::string::npos)));
	host = data.location["proxy_pass"].string_value().substr(0, pos);
	if ((inet_pton(AF_INET, host.c_str(), &proxy_addr.sin_addr)) <= 0)
	{
		perror("inet_pton()");
		data.response.sendInternalError();
		return 1;
	}
	return 0;
}

int Proxy::connectProxySocket()
{
	if (connect(this->proxy_socket.getSocket(), reinterpret_cast<sockaddr *>(&this->proxy_addr), sizeof(this->proxy_addr)) < 0)
	{
		perror("connect()");
		data.response.sendInternalError();
		return 1;
	}
	return 0;
}

std::string Proxy::getProxyRequest()
{
	std::ostringstream oss;
	std::map<std::string, std::string> headers = data.request.getHeaders();

	oss << data.request.getMethod() << ' ' << data.path << ' ' << data.request.getVersion() << lineTerminator;
	for (auto header : headers)
	{
		if (header.first != "host" && header.first != "connection")
			oss << header.first << ": " << header.second << lineTerminator;
	}
	oss << lineTerminator;
	return oss.str();
}

Socket Proxy::getProxySocket()
{
	return proxy_socket;
}

// void Proxy::receiveProxyResponse()
// {
// 	raw_response = proxy_socket.receive();
// }

// void Proxy::sendProxyResponse()
// {
// 	data.response.sendDataRaw(raw_response);
// }