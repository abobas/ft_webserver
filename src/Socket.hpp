/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Socket.hpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/07/21 16:57:38 by abobas        #+#    #+#                 */
/*   Updated: 2020/07/21 20:46:18 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>

class Socket
{
private:
	std::string type;
	int socket;

public:
	Socket(const std::string type, int socket) : type(type), socket(socket) {}
	std::string getType() const
	{
		return this->type;
	}
	void setType(const std::string new_type)
	{
		this->type = new_type;
	}
	int getSocket() const
	{
		return this->socket;
	}
	~Socket() {}
};

bool operator==(const Socket &lhs, const Socket &rhs)
{
	if (lhs.getSocket() != rhs.getSocket())
		return (false);
	if (lhs.getType() != rhs.getType())
		return (false);
	return (true);
}

bool operator<(const Socket &lhs, const Socket &rhs)
{
	if (lhs.getSocket() < rhs.getSocket())
		return (true);
	return (false);
}

