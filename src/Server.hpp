/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Server.hpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/10/19 21:16:59 by abobas        #+#    #+#                 */
/*   Updated: 2020/10/28 16:37:42 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "json/Json.hpp"
#include "Socket.hpp"
#include "Logs.hpp"
#include "Response.hpp"
#include "Data.hpp"
#include <fstream>
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
	Server(Json &&config);

private:
	std::vector<Socket> sockets;
	std::map<int, int> pairs;
	std::map<Socket, std::string> messages;
	Json config;
	Logs log;
	struct timeval tv
	{
		tv.tv_sec = 1, 
		tv.tv_usec = 0
	};
	fd_set read_set;
	fd_set write_set;

	void createListenSockets();
	std::string getTime();
	void runtime();
	void fillSelectSets();
	int getSelectRange();
	int selectCall();
	void handleOperations();
	void acceptClient(Socket &listen);
	void readClient(Socket &client);
	void writeClient(Socket &client);
	void writeWaitingClient(Socket &client);
	void readProxy(Socket &proxy);
	void writeProxy(Socket &proxy);
	void transformSocket(Socket &socket);
	void disconnectSocket(Socket &socket);
	void addSocket(Socket &insert);
	void addSocket(Socket &&insert);
	void deleteSocket(Socket &erase);
	Socket &findPair(Socket &client);
	void addPair(int key, int value);
	void deletePair(int key);
	void addMessage(Socket &socket, std::string &&request);
	void addMessage(Socket &&socket, std::string &&request);
	void deleteMessage(Socket &socket);
};
