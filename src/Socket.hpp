/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Socket.hpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/07/21 16:57:38 by abobas        #+#    #+#                 */
/*   Updated: 2020/10/26 22:46:28 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <unistd.h>
#include <string>
#include <fcntl.h>

/**
* @brief Handles socket operations like reading / writing.
*/
class Socket
{

public:
	Socket();
	Socket(const std::string type, int socket);
	std::string getType() const;
	void setType(std::string new_type);
	int getSocket() const;
	void sendData(std::string &value);
	void sendData(std::string &&value);
	void sendFile(std::string &path);
	std::string receive();

private:
	std::string type;
	int socket;

	void debug(std::string &value);
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
