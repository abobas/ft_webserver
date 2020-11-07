/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Socket.hpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/07/21 16:57:38 by abobas        #+#    #+#                 */
/*   Updated: 2020/11/08 00:19:26 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "logger/Log.hpp"
#include "config/Json.hpp"
#include "incoming/Receiver.hpp"
#include "incoming/Evaluator.hpp"
#include "outgoing/Resolver.hpp"
#include <string>
#include <map>
#include <algorithm>
#include <string.h>
#include <arpa/inet.h>

class Socket
{
	friend class Proxy;

public:
	static void initializeSocket(Json &config);
	static std::map<int, Socket *> &getSockets();
	static void createListenSockets();

	void acceptConnection();
	void handleIncoming();
	void handleOutgoing();

	int getSocket() const;
	std::string getType();

private:
	static Log *log;
	static Json config;
	static std::map<int, Socket *> sockets;
	Receiver *receiver;
	Evaluator *evaluator;
	Resolver *resolver;
	std::string type;
	int socket_fd;
	int proxy_pair;

	Socket(std::string type, int socket);

	void handleReceiving();
	void handleEvaluating();
	void handleProcessing();
	bool isAlive();
	void deleteSocket();
	
	void setType(std::string new_type);
	void setPair(int socket);
};

inline bool operator==(const Socket &lhs, const Socket &rhs)
{
	if (lhs.getSocket() != rhs.getSocket())
		return false;
	return true;
}

inline bool operator<(const Socket &lhs, const Socket &rhs)
{
	if (lhs.getSocket() < rhs.getSocket())
		return true;
	return false;
}
