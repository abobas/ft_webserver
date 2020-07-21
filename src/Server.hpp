/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Server.hpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/07/21 17:22:55 by abobas        #+#    #+#                 */
/*   Updated: 2020/07/21 19:52:42 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "SocketHandler.hpp"
#include "Socket.hpp"

class Server
{
private:
	SocketHandler socket;

public:
	Server()
	{
		this->socket.init();
	}
	~Server() {}
	void runtime()
	{
		while (1)
			this->socket.handleSockets();
	}
};
