/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Server.hpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/10/19 21:16:59 by abobas        #+#    #+#                 */
/*   Updated: 2020/10/28 20:51:24 by abobas        ########   odam.nl         */
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
* @brief Handles incoming connections and serves responses to clients' requests.
*/
class Server
{

public:
	Server(Json &&config);
	~Server();

private:
	std::vector<Socket> sockets;
	std::map<int, int> pairs;
	std::map<Socket, std::string> messages;
	Json config;
	Log *log;
	struct timeval tv
	{
		tv.tv_sec = 1,
		tv.tv_usec = 0
	};
	fd_set read_set;
	fd_set write_set;
	
	void createListenSockets();
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

	void addSocket(Socket &insert);
	void addSocket(Socket &&insert);
	void deleteSocket(Socket &erase);
	void transformSocket(Socket &socket);
	void disconnectSocket(Socket &socket);

	Socket &findPair(Socket &client);
	void addPair(int key, int value);
	void deletePair(int key);

	void addMessage(Socket &socket, std::string &&request);
	void addMessage(Socket &&socket, std::string &&request);
	void deleteMessage(Socket &socket);
};
