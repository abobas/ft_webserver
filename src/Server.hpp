/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Server.hpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/10/19 21:16:59 by abobas        #+#    #+#                 */
/*   Updated: 2020/11/07 22:50:19 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Socket.hpp"
#include "logger/Log.hpp"
#include "config/Json.hpp"
#include <string>
#include <vector>
#include <sys/time.h>
#include <sys/select.h>

/**
* @brief Core object that handles all incoming connections.
*/
class Server
{

public:
	Server(Json &&config);

private:
	static Log *log;
	static timeval tv;
	Json config;
	fd_set read_set;
	fd_set write_set;

	void mainLoop();
	int selectCall();
	void fillSelectSets();
	int getSelectRange();
	void handleOperations(int select);
	fd_set *getSet(Socket *socket);
	void executeOperation(Socket *socket);
};
