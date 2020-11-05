/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Socket.hpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/07/21 16:57:38 by abobas        #+#    #+#                 */
/*   Updated: 2020/11/05 16:37:31 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "logger/Log.hpp"
#include "config/Json.hpp"
#include "incoming/Receiver.hpp"
#include "incoming/Evaluator.hpp"
#include "incoming/Processor.hpp"
#include <string>

/**
* @brief Handles socket operations.
*/
class Socket
{

public:
	Socket();
	Socket(std::string type, int socket);
	~Socket();
	static void initializeSocket(Json &config);
	void handleIncoming();

	int getSocket() const;
	std::string getType() const;
	void setType(std::string new_type);
	
	// std::string getMessage();
	// void receiveMessage();
	// bool isReady();
	// bool isAlive();


private:
	static Log *log;
	static Json config;
	Receiver *receiver;
	Evaluator *evaluator;
	int socket;
	std::string type;
	
	// std::string message;
	// bool received = false;
};

inline bool operator==(const Socket &lhs, const Socket &rhs)
{
	if (lhs.getSocket() != rhs.getSocket())
		return false;
	if (lhs.getType() != rhs.getType())
		return false;
	return true;
}

inline bool operator<(const Socket &lhs, const Socket &rhs)
{
	if (lhs.getSocket() < rhs.getSocket())
		return true;
	return false;
}
