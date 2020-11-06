/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Proxy.hpp                                          :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/09/06 18:32:53 by abobas        #+#    #+#                 */
/*   Updated: 2020/11/05 17:11:33 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "../incoming/Parser.hpp"
#include "../incoming/Matcher.hpp"
#include "../logger/Log.hpp"
#include "../Socket.hpp"
#include <sstream>
#include <string>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>

class Proxy
{
public:
	/**
	* @brief Sets up connection with proxy server.
	*/
	static Proxy resolveProxyRequest(Matcher &matched, Parser &parsed);

	Socket getProxySocket();
	std::string getProxyRequest();

private:
	Matcher &matched;
	Parser &parsed;
	static std::string CRLF;
	static Log *log;
	std::string proxy_path;
	Socket proxy_socket;
	struct sockaddr_in proxy_addr;

	Proxy(Matcher &matched, Parser &parsed);
	void setPath();
	void clearAddress();
	void createProxySocket();
	void setProxyAddress();
	void connectProxySocket();
};
