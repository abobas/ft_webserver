/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Socket.hpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/07/21 16:57:38 by abobas        #+#    #+#                 */
/*   Updated: 2020/10/28 17:41:08 by abobas        ########   odam.nl         */
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
* @brief Handles socket operations like reading / writing.
*/
class Socket
{

public:
	Socket();
	Socket(std::string type, int socket);

	void sendData(std::string &value);
	void sendData(std::string &&value);
	std::string receive();
	void sendFile(std::string &path);

	std::string getType() const;
	void setType(std::string new_type);
	int getSocket() const;

private:
	std::string type;
	int socket;
};

inline int getListenSocket(int port)
{
	int new_socket;
	int enable = 1;
	sockaddr_in new_address;

	memset(&new_address, 0, sizeof(new_address));
	new_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (setsockopt(new_socket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
		throw "setsockopt()";
	new_address.sin_family = AF_INET;
	new_address.sin_addr.s_addr = INADDR_ANY;
	new_address.sin_port = htons(port);
	if (bind(new_socket, reinterpret_cast<sockaddr *>(&new_address), sizeof(new_address)) < 0)
		throw "bind()";
	if (listen(new_socket, SOMAXCONN) < 0)
		throw "listen()";
	return new_socket;
}

inline int getAcceptedClient(int listen)
{
	struct sockaddr client_address;
	unsigned int client_address_length = 0;

	memset(&client_address, 0, sizeof(client_address));
	return accept(listen, &client_address, &client_address_length);
}

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
