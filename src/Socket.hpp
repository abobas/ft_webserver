/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Socket.hpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/07/21 16:57:38 by abobas        #+#    #+#                 */
/*   Updated: 2020/11/07 13:22:40 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "logger/Log.hpp"
#include "config/Json.hpp"
#include "incoming/Receiver.hpp"
#include "incoming/Evaluator.hpp"
#include "outgoing/Resolver.hpp"
#include <string>

/**
* @brief Handles socket operations.
*/
class Socket
{

public:
	Socket();
	Socket(std::string type, int socket);
	static void initializeSocket(Json &config);
	
	void handleIncoming();
	void handleOutgoing();
	bool isAlive();

	int getSocket() const;
	std::string getType();
	void setType(std::string new_type);

private:
	static Log *log;
	static Json config;
	Receiver *receiver;
	Evaluator *evaluator;
	Resolver *resolver;
	std::string type;
	int socket;
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
