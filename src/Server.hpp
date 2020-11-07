/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Server.hpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/10/19 21:16:59 by abobas        #+#    #+#                 */
/*   Updated: 2020/11/07 12:13:40 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Socket.hpp"
#include "logger/Log.hpp"
#include "config/Json.hpp"
#include "outgoing/Responder.hpp"
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <sys/time.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <string.h>

/**
* @brief Core object that handles all incoming connections.
*/
class Server
{

public:
	Server(Json &&config);

private:
	static Log *log;
	static timeval tv;
	Json config;
	std::vector<Socket> sockets;
	std::map<int, int> pairs;
	std::map<Socket, std::string> messages;
	fd_set read_set;
	fd_set write_set;

	void createListenSockets();
	int getListenSocket(int port);
	void mainLoop();
	int selectCall();
	void fillSelectSets();
	int getSelectRange();
	void handleOperations(int select);
	fd_set *getSet(Socket &socket);
	void executeOperation(Socket &socket);
	
	void acceptClient(Socket &listen);
	void readClient(Socket &client);
	void writeClient(Socket &client);

	void addSocket(Socket &insert);
	void addSocket(Socket &&insert);
	void deleteSocket(Socket &erase);
	void disconnectSocket(Socket &socket);

	// void writeWaitingClient(Socket &client);
	// void readProxy(Socket &proxy);
	// void writeProxy(Socket &proxy);
	// Socket &findPair(Socket &client);
	// void addPair(int key, int value);
	// void deletePair(int key);
};
