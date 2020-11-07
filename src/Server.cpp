/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Server.cpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/08/27 17:11:43 by abobas        #+#    #+#                 */
/*   Updated: 2020/11/07 22:50:47 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Log *Server::log = Log::getInstance();
timeval Server::tv{tv.tv_sec = 0, tv.tv_usec = 0};

Server::Server(Json &&config) : config(config)
{
	Socket::initializeSocket(config);
	try
	{
		Socket::createListenSockets();
		while (1)
			mainLoop();
	}
	catch (const char *e)
	{
		log->logError(e);
	}
}

void Server::mainLoop()
{
	int select = selectCall();
	if (select < 0)
		throw "select()";
	if (select > 0)
		handleOperations(select);
}

int Server::selectCall()
{
	fillSelectSets();
	return (select(getSelectRange(), &read_set, &write_set, NULL, &tv));
}

void Server::fillSelectSets()
{
	FD_ZERO(&read_set);
	FD_ZERO(&write_set);
	for (auto socket : Socket::getSockets())
	{
		if (socket.second->getType() == "listen")
			FD_SET(socket.second->getSocket(), &read_set);
		else if (socket.second->getType() == "client_read")
			FD_SET(socket.second->getSocket(), &read_set);
		else if (socket.second->getType() == "client_write")
			FD_SET(socket.second->getSocket(), &write_set);

		// 	else if (socket->getType() == "proxy_read")
		// 		FD_SET(socket->getSocket(), &read_set);
		// 	else if (socket->getType() == "proxy_write")
		// 		FD_SET(socket->getSocket(), &write_set);
		// 	else if (socket->getType() == "wait_client_write")
		// 		FD_SET(socket->getSocket(), &write_set);
	}
}

int Server::getSelectRange()
{
	int max = 0;
	for (auto socket : Socket::getSockets())
	{
		if (socket.second->getSocket() > max)
			max = socket.second->getSocket();
	}
	return (max + 1);
}

void Server::handleOperations(int select)
{
	std::vector<Socket *> tmp;

	for (auto socket : Socket::getSockets())
	{
		if (FD_ISSET(socket.second->getSocket(), getSet(socket.second)))
		{
			tmp.push_back(socket.second);
			select--;
			if (select < 1)
				break;
		}
	}
	for (auto socket : tmp)
		executeOperation(socket);
}

fd_set *Server::getSet(Socket *socket)
{
	if (socket->getType() == "listen")
		return &read_set;
	else if (socket->getType() == "client_read")
		return &read_set;
	else if (socket->getType() == "client_write")
		return &write_set;

	// else if (socket->getType() == "proxy_read")
	// 	return &read_set;
	// else if (socket->getType() == "proxy_write")
	// 	return &write_set;
	// else if (socket->getType() == "wait_client_write")
	// 	return &write_set;
	else
		return NULL;
}

void Server::executeOperation(Socket *socket)
{
	if (socket->getType() == "listen")
		socket->acceptConnection();
	else if (socket->getType() == "client_read")
		socket->handleIncoming();
	else if (socket->getType() == "client_write")
		socket->handleOutgoing();

	// else if (socket->getType() == "proxy_read")
	// 	readProxy(socket);
	// else if (socket->getType() == "proxy_write")
	// 	writeProxy(socket);
	// else if (socket->getType() == "wait_client_write")
	// 	writeWaitingClient(socket);
}