/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Proxy.hpp                                          :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/09/06 18:32:53 by abobas        #+#    #+#                 */
/*   Updated: 2020/10/26 15:15:26 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Data.hpp"
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sstream>
#include <cstring>

/**
* @brief Sets up connection with proxy server.
*/
class Proxy
{
public:
	Proxy(Data &data);

private:
	Data data;
	Socket proxy_socket;
	struct sockaddr_in proxy_addr;
	std::string raw_response;
	std::string host;

	void setPath();
	void clearAddress();
	int createProxySocket();
	int setProxyAddress();
	int connectProxySocket();
	void sendProxyRequest();
	void receiveProxyResponse();
	void sendProxyResponse();
};
