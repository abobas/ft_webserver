/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   main.cpp                                           :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/07/04 14:09:30 by abobas        #+#    #+#                 */
/*   Updated: 2020/07/05 16:11:16 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "includes/Server.hpp"
#include <iostream>
#include <signal.h>

Server server;

void	signal_catcher(int signal)
{
	if (signal == SIGINT)
	{
		std::cout << "\nProcess interrupted" << std::endl;
		server.CloseSockets();
		exit(1);
	}
}

int main()
{
	try
	{
		signal(SIGINT, signal_catcher);
		server.AcceptConnections();
	}
	catch(const char *e)
	{
		std::cerr << e << std::endl;
	}
}
