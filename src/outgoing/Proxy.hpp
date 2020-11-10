/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Proxy.hpp                                          :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/09/06 18:32:53 by abobas        #+#    #+#                 */
/*   Updated: 2020/11/10 14:19:48 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Responder.hpp"
#include "../Socket.hpp"
#include "../incoming/Matcher.hpp"
#include "../incoming/Parser.hpp"
#include "../logger/Log.hpp"
#include <string>
#include <map>
#include <sstream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>

class Proxy
{
public:
	static Proxy *getInstance(int socket, Matcher &matched, Parser &parsed);
	static void deleteInstance(int socket);

	void resolveProxy();
	bool isResolved();
	std::string getProxyRequest();
	void setError();
	void setResponse(std::string &buffer);

private:
	static Log *log;
	static std::map<int, Proxy *> proxies;
	static std::string CRLF;
	Matcher &matched;
	Parser &parsed;
	class Socket *proxy;
	struct sockaddr_in proxy_addr;
	int socket_fd;
	int proxy_socket;
	std::string proxy_path;
	std::string status;
	std::string proxy_response;
	bool resolved;
	bool initialized;

	Proxy(int socket, Matcher &matched, Parser &parsed);
	void initializeProxy();
	void setPath();
	void clearAddress();
	void createProxySocket();
	void setProxyAddress();
	void connectProxySocket();
};