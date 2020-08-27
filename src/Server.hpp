/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Server.hpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/07/21 17:22:55 by abobas        #+#    #+#                 */
/*   Updated: 2020/08/27 17:24:04 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "SocketHandler.hpp"
#include "Socket.hpp"
#include "Json.hpp"

class Server
{
private:
	SocketHandler socket;

public:
	Server(Json::Json config);
	~Server();
	void runtime();
};
