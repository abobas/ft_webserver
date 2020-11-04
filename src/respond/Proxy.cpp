/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Proxy.cpp                                          :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/09/06 18:32:50 by abobas        #+#    #+#                 */
/*   Updated: 2020/11/04 13:11:08 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Proxy.hpp"

std::string Proxy::CRLF = "\r\n";
Log *Proxy::log = Log::getInstance();

Proxy Proxy::resolveProxyRequest(const Matcher &matched, const Parser &parsed)
{
	return Proxy(matched, parsed);
}

Proxy::Proxy(const Matcher &matched, const Parser &parsed)
	: matched(matched), parsed(parsed)
{
	setPath();
	log->logEntry("path set");
	createProxySocket();
	log->logEntry("socket created");
	setProxyAddress();
	log->logEntry("address set");
	connectProxySocket();
	log->logEntry("proxy connected");
}

void Proxy::setPath()
{
	proxy_path = matched.getPath();
	if (proxy_path.empty())
		proxy_path = "/";
	log->logEntry("proxy_path: " + proxy_path);
	log->logEntry("matched_path : " + matched.getPath());
	log->logEntry("parsed path: " + parsed.getPath());
}

void Proxy::createProxySocket()
{
	int enable = 1;
	int new_socket;

	new_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (new_socket == -1)
		throw "socket()";
	if (setsockopt(new_socket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
		throw "setsockopt()";
	proxy_socket = Socket("proxy_write", new_socket);
}

void Proxy::setProxyAddress()
{
	Json::object location;
	std::string host;
	size_t pos;

	location = matched.getLocation();
	memset(&proxy_addr, 0, sizeof(proxy_addr));
	proxy_addr.sin_family = AF_INET;
	pos = location["proxy_pass"].string_value().find(':');
	if (pos == std::string::npos)
		proxy_addr.sin_port = htons(80);
	else
		proxy_addr.sin_port = htons(std::stoi(location["proxy_pass"].string_value().substr(pos + 1, std::string::npos)));
	host = location["proxy_pass"].string_value().substr(0, pos);
	if ((inet_pton(AF_INET, host.c_str(), &proxy_addr.sin_addr)) <= 0)
		throw "inet_pton()";
}

void Proxy::connectProxySocket()
{
	if (connect(proxy_socket.getSocket(), reinterpret_cast<sockaddr *>(&this->proxy_addr), sizeof(this->proxy_addr)) < 0)
		throw "connect()";
}

std::string Proxy::getProxyRequest()
{
	std::ostringstream oss;
	std::map<std::string, std::string> headers = parsed.getHeaders();

	oss << parsed.getMethod() << ' ' << proxy_path << ' ' << parsed.getVersion() << CRLF;
	for (auto header : headers)
	{
		if (header.first != "host" && header.first != "connection")
			oss << header.first << ": " << header.second << CRLF;
	}
	oss << CRLF;
	return oss.str();
}

Socket Proxy::getProxySocket()
{
	return proxy_socket;
}
