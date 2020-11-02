/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Socket.hpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/07/21 16:57:38 by abobas        #+#    #+#                 */
/*   Updated: 2020/11/02 22:23:34 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Log.hpp"
#include "receiver/Receiver.hpp"
#include <string>

/**
* @brief Handles socket operations.
*/
class Socket
{

public:
	Socket();
	Socket(std::string type, int socket);

	bool isAlive();
	void receiveMessage();

	void sendData(std::string &value);
	void sendData(std::string &&value);
	
	bool isReady();
	std::string getMessage();
	int getSocket() const;
	std::string getType() const;
	void setType(std::string new_type);
	
private:
	static Log *log;
	Receiver *receiver;	
	std::string message;
	std::string type;
	int socket_fd;
	bool received = false;
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
