/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   SocketHandler.hpp                                  :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/07/21 17:17:49 by abobas        #+#    #+#                 */
/*   Updated: 2020/08/28 21:08:45 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Socket.hpp"
#include "Json.hpp"
#include "ResponseHandler.hpp"
#include "HttpParser.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include <vector>
#include <map>
#include <algorithm>
#include <string>
#include <sys/time.h>
#include <unistd.h>

class SocketHandler
{

public:
	SocketHandler(const Json::Json &config);
	~SocketHandler();
	void runtime();

private:
	std::vector<Socket> sockets;
	std::map<Socket, std::string> requests;
	Json::Json config;
	struct timeval tv
	{
		tv.tv_sec = 1, tv.tv_usec = 0
	};
	fd_set read_set;
	fd_set write_set;

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
