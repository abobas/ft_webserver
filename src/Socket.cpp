/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Socket.cpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/08/26 19:00:35 by abobas        #+#    #+#                 */
/*   Updated: 2020/08/26 21:48:52 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Socket.hpp"
#include <unistd.h>
#include <string>

std::string Socket::getType() const
{
    return this->type;
}

void Socket::setType(const std::string new_type)
{
    this->type = new_type;
}

int Socket::getSocket() const
{
    return this->socket;
}

void Socket::send(std::string value) const
{
	write(this->socket, value.c_str(), value.size());
}
