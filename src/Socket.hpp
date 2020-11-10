/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Socket.hpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/07/21 16:57:38 by abobas        #+#    #+#                 */
/*   Updated: 2020/11/10 14:27:31 by abobas        ########   odam.nl         */
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

class Resolver;

class Socket
{
public:
	Socket(std::string type, int socket);
	~Socket();
	static void initializeSocket(Json &config);
	static std::map<int, Socket *> &getSockets();
	static void createListenSockets();
	void acceptConnection();
	void handleIncoming();
	void handleOutgoing();
	void handleProxyOutgoing();
	void handleProxyIncoming();
	int getSocket() const;
	std::string getType();
	void setPair(int socket);
	void deleteSocket();

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

	void handleReceiving();
	void handleEvaluating();
	void handleProcessing();
	bool isAlive();
	void setType(std::string new_type);
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
