/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Server.hpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/10/19 21:16:59 by abobas        #+#    #+#                 */
/*   Updated: 2020/10/23 17:37:07 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Socket.hpp"
#include "json/Json.hpp"
#include "Response.hpp"
#include "Data.hpp"
#include <vector>
#include <map>
#include <algorithm>
#include <string>
#include <sys/time.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <arpa/inet.h>

class Server
{

public:
	Server(const Json &&config);

private:
	std::vector<Socket> sockets;
	std::map<Socket, std::string> requests;
	Json config;
	struct timeval tv
	{
		tv.tv_sec = 1, 
		tv.tv_usec = 0
	};
	fd_set read_set;
	fd_set write_set;

	void runtime();
	void createServerSockets();
	void fillSets();
	int getRange();
	int selectCall();
	void handleOperations(int select);
	void acceptClient(Socket &server);
	void readClient(Socket &client);
	void writeClient(Socket &client);
	void transformClient(Socket &client);
	void disconnectClient(Socket &client);
	void addSocket(Socket &&insert);
	void deleteSocket(Socket &erase);
	void addRequest(Socket &client, std::string &&request);
	void deleteRequest(Socket &client);
};
