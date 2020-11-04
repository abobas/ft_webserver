/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Proxy.hpp                                          :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/09/06 18:32:53 by abobas        #+#    #+#                 */
/*   Updated: 2020/11/04 12:05:01 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "../evaluate/Parser.hpp"
#include "../evaluate/Matcher.hpp"
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
	static Proxy resolveProxyRequest(const Matcher &matched, const Parser &parsed);

	Socket getProxySocket();
	std::string getProxyRequest();

private:
	const Matcher &matched;
	const Parser &parsed;
	static std::string CRLF;
	static Log *log;
	std::string proxy_path;
	Socket proxy_socket;
	struct sockaddr_in proxy_addr;

	Proxy(const Matcher &matched, const Parser &parsed);
	void setPath();
	void clearAddress();
	void createProxySocket();
	void setProxyAddress();
	void connectProxySocket();
};
