/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Proxy.hpp                                          :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/09/06 18:32:53 by abobas        #+#    #+#                 */
/*   Updated: 2020/10/28 01:22:14 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Data.hpp"
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sstream>
#include <string>
#include <cstring>

/**
* @brief Sets up connection with proxy server.
*/
class Proxy
{
public:
	Proxy(Data &data);
	Socket getProxySocket();
	std::string getProxyRequest();

private:
	Data data;
	Socket proxy_socket;
	struct sockaddr_in proxy_addr;
	std::string host;

	void setPath();
	void clearAddress();
	void createProxySocket();
	void setProxyAddress();
	void connectProxySocket();
};
