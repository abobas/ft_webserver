/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Proxy.cpp                                          :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/09/06 18:32:50 by abobas        #+#    #+#                 */
/*   Updated: 2020/11/10 16:56:11 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Proxy.hpp"

std::string Proxy::CRLF = "\r\n";
Log *Proxy::log = Log::getInstance();
std::map<int, Proxy *> Proxy::proxies;

Proxy::Proxy(int socket, Matcher &matched, Parser &parsed) :
	matched(matched), parsed(parsed), socket_fd(socket)
{
	resolved = false;
	initialized = false;
}

Proxy *Proxy::getInstance(int socket, Matcher &matched, Parser &parsed)
{
	if (!proxies[socket])
	{
		proxies[socket] = new Proxy(socket, matched, parsed);
		log->logEntry("created proxy", socket);
	}
	return proxies[socket];
}

void Proxy::deleteInstance(int socket)
{
	if (proxies[socket])
	{
		delete proxies[socket];
		proxies.erase(socket);
		log->logEntry("deleted proxy", socket);
	}
}

void Proxy::resolveProxy()
{
	if (!initialized)
		initializeProxy();
	if (status == "wait")
		return;
	else if (status == "error")
		throw "proxy recv()";
	else if (status == "continue")
	{
		Responder respond(socket_fd, parsed);
		respond.sendDataRaw(proxy_response);
		resolved = true;
	}
}

void Proxy::setResponse(std::string &buffer)
{
	proxy_response = std::move(buffer);
	status = "continue";
}

void Proxy::setError()
{
	status = "error";
}

void Proxy::initializeProxy()
{
	setPath();
	createProxySocket();
	setProxyAddress();
	connectProxySocket();
	initialized = true;
	status = "wait";
	log->logEntry("client now waiting on proxy");
}

void Proxy::setPath()
{
	proxy_path = matched.getPath();
	if (proxy_path.empty())
		proxy_path = "/";
}

void Proxy::createProxySocket()
{
	int enable = 1;

	proxy_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (proxy_socket == -1)
		throw "socket()";
	if (setsockopt(proxy_socket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
		throw "setsockopt()";
	proxy = new Socket("proxy_write", proxy_socket);
	proxy->setPair(socket_fd);
	Socket::getSockets().insert({proxy_socket, proxy});
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
	{
		proxy->deleteSocket();
		throw "inet_pton()";
	}
}

void Proxy::connectProxySocket()
{
	if (connect(proxy_socket, reinterpret_cast<sockaddr *>(&this->proxy_addr), sizeof(this->proxy_addr)) < 0)
	{
		proxy->deleteSocket();
		throw "connect()";
	}
	log->logEntry("connected with proxy", proxy_socket);
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

bool Proxy::isResolved()
{
	return resolved;
}