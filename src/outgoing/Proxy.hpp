/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Proxy.hpp                                          :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/09/06 18:32:53 by abobas        #+#    #+#                 */
/*   Updated: 2020/11/08 00:36:33 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "../incoming/Matcher.hpp"
#include "../incoming/Parser.hpp"
#include "../logger/Log.hpp"
#include "../Socket.hpp"
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
	static Proxy *createInstance(int socket, Matcher &matched, Parser &parsed);
	static Proxy *getInstance(int socket);
	static void deleteInstance(int socket);

	void resolveProxy();
	void writeProxy();
	void readProxy();
	bool isResolved();

private:
	static Log *log;
	static std::map<int, Proxy *> proxies;
	static std::string CRLF;
	Matcher &matched;
	Parser &parsed;
	Socket *proxy;
	struct sockaddr_in proxy_addr;
	int socket_fd;
	int proxy_socket;
	std::string proxy_path;
	bool resolved;
	bool initialized;
	bool routed;
	
	Proxy(int socket, Matcher &matched, Parser &parsed);
	
	void initializeProxy();
	
	void setPath();
	void clearAddress();
	void createProxySocket();
	void setProxyAddress();
	void connectProxySocket();
	std::string getProxyRequest();
};