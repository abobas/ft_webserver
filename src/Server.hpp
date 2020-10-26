/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Server.hpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/10/19 21:16:59 by abobas        #+#    #+#                 */
/*   Updated: 2020/10/26 20:15:48 by abobas        ########   odam.nl         */
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
#include <cstring>
#include <sys/time.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <arpa/inet.h>

/**
* @brief Handles incoming connections and serves responses to clients' requests.
*/
class Server
{

public:
	Server(const Json &&config);

private:
	std::vector<Socket> sockets;
	std::map<Socket, std::string> messages;
	std::map<Socket, Socket> pairs;
	Json config;
	struct timeval tv
	{
		tv.tv_sec = 1, 
		tv.tv_usec = 0
	};
	fd_set read_set;
	fd_set write_set;

	void runtime();
	void createListenSockets();
	
	void fillSelectSets();
	int getSelectRange();
	int selectCall();

	void handleOperations(int select);
	
	void acceptClient(Socket &listen);
	void readClient(Socket &client);
	void writeClient(Socket &client);
	void writeWaitingClient(Socket &client);
	
	void readProxy(Socket &proxy);
	void writeProxy(Socket &proxy);
	
	void transformSocket(Socket &socket);
	void disconnectSocket(Socket &socket);
	
	void addSocket(Socket &&insert);
	void deleteSocket(Socket &erase);
	
	void addPair(Socket &key, Socket &&value);
	void deletePair(Socket &key);
	
	void addMessage(Socket &socket, std::string &&request);
	void addMessage(Socket &&socket, std::string &&request);
	void deleteMessage(Socket &socket);
};
