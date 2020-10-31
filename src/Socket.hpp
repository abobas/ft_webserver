/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Socket.hpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/07/21 16:57:38 by abobas        #+#    #+#                 */
/*   Updated: 2020/10/30 13:10:20 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Log.hpp"
#include <string>
#include <algorithm>
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

	static int getListenSocket(int port);
	int acceptClient();
	bool closedClient();
	void cleanSocket();
	
	void sendData(std::string &value);
	void sendData(std::string &&value);
	void receiveData();

	void setType(std::string new_type);
	int getSocket() const;
	bool getEndOfFile() const;
	std::string getMessage() const;
	std::string getType() const;
	
private:
	Log *log;
	std::string type;
	std::string message;
	int socket_fd;
	
	bool headers_read = true;
	bool has_body = false;
	bool chunked = false;
	bool end_of_file = false;

	std::string readSocket();
	
	void cleanBody();
	void decodeChunkedBody(std::string &body);
	bool isChunked();
	bool hasBody();
	bool endOfHeaders();
	bool endOfChunked();
};

inline bool operator==(const Socket &lhs, const Socket &rhs)
{
	if (lhs.getSocket() != rhs.getSocket())
		return false;
	if (lhs.getType() != rhs.getType())
		return false;
	if (lhs.getEndOfFile() != rhs.getEndOfFile())
		return false;
	if (lhs.getMessage() != rhs.getMessage())
		return false;
	return true;
}

inline bool operator<(const Socket &lhs, const Socket &rhs)
{
	if (lhs.getSocket() < rhs.getSocket())
		return true;
	return false;
}
