/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Server.cpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/08/27 17:23:09 by abobas        #+#    #+#                 */
/*   Updated: 2020/08/27 17:23:46 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "SocketHandler.hpp"
#include "Socket.hpp"
#include "Json.hpp"

Server::Server(Json::Json config) : socket(config) {}

Server::~Server() {}

void Server::runtime()
{
    while (1)
        this->socket.runtime();
}
