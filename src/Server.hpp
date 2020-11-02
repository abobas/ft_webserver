/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Server.hpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/10/19 21:16:59 by abobas        #+#    #+#                 */
/*   Updated: 2020/11/02 01:27:17 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Socket.hpp"
#include "Log.hpp"
#include "json/Json.hpp"
#include "response/Response.hpp"
#include "response/Data.hpp"
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <sys/time.h>
#include <sys/select.h>

/**
* @brief Core object that handles all incoming connections.
*/
class Server
{

public:
	Server(Json &&config);

private:
	// initializiation
	Json config;
	Log *log;
	void createListenSockets();
	int getListenSocket(int port);

	// polling
	fd_set read_set;
	fd_set write_set;
	
	void mainLoop();
	int selectCall();
	void fillSelectSets();
	int getSelectRange();
	void handleOperations(int select);
	fd_set *getSet(Socket &socket);
	void executeOperation(Socket &socket);
	
	// socket operations
	void acceptClient(Socket &listen);
	void readClient(Socket &client);
	void writeClient(Socket &client);
	void writeWaitingClient(Socket &client);
	void readProxy(Socket &proxy);
	void writeProxy(Socket &proxy);

	// managing open connections
	std::vector<Socket> sockets;
	std::map<int, int> pairs;
	std::map<Socket, std::string> messages;
	
	void addSocket(Socket &insert);
	void addSocket(Socket &&insert);
	void deleteSocket(Socket &erase);
	void transformSocket(Socket &socket);
	void disconnectSocket(Socket &socket);
	Socket &findPair(Socket &client);
	void addPair(int key, int value);
	void deletePair(int key);
	void addMessage(Socket &socket, std::string &&message);
	void addMessage(Socket &&socket, std::string &&message);
	void deleteMessage(Socket &socket);
};
