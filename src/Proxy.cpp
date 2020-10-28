/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Proxy.cpp                                          :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/09/06 18:32:50 by abobas        #+#    #+#                 */
/*   Updated: 2020/10/28 01:21:49 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Proxy.hpp"

static std::string lineTerminator = "\r\n";

Proxy::Proxy(Data &data) : data(data)
{
	try
	{
		setPath();
		createProxySocket();
		setProxyAddress();
		connectProxySocket();
	}
	catch (const char *e)
	{
		throw e;
	}
}

void Proxy::setPath()
{
	if (data.path.size() == 0)
		data.path = "/";
}

void Proxy::createProxySocket()
{
	int new_socket;
	int enable = 1;

	new_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (new_socket == -1)
		throw "socket()";
	if (setsockopt(new_socket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
		throw "setsockopt()";
	proxy_socket = Socket("proxy_write", new_socket);
}

void Proxy::setProxyAddress()
{
	memset(&proxy_addr, 0, sizeof(proxy_addr));
	proxy_addr.sin_family = AF_INET;
	size_t pos = data.location["proxy_pass"].string_value().find(':');
	if (pos == std::string::npos)
		proxy_addr.sin_port = htons(80);
	else
		proxy_addr.sin_port = htons(std::stoi(data.location["proxy_pass"].string_value().substr(pos + 1, std::string::npos)));
	host = data.location["proxy_pass"].string_value().substr(0, pos);
	if ((inet_pton(AF_INET, host.c_str(), &proxy_addr.sin_addr)) <= 0)
		throw "inet_pton()";
}

void Proxy::connectProxySocket()
{
	if (connect(this->proxy_socket.getSocket(), reinterpret_cast<sockaddr *>(&this->proxy_addr), sizeof(this->proxy_addr)) < 0)
		throw "connect()";
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
