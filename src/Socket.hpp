/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Socket.hpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/07/21 16:57:38 by abobas        #+#    #+#                 */
/*   Updated: 2020/10/28 21:21:45 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

/**
* @brief Handles socket operations.
*/
class Socket
{

public:
	Socket();
	Socket(std::string type, int socket);

	void sendData(std::string &value);
	void sendData(std::string &&value);
	void sendFile(std::string &path);
	std::string receive();
	static int getListenSocket(int port);
	int acceptClient();
	std::string getType() const;
	void setType(std::string new_type);
	int getSocket() const;

private:
	std::string type;
	int socket_fd;
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
