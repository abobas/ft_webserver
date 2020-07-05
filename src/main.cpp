/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   main.cpp                                           :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/07/04 14:09:30 by abobas        #+#    #+#                 */
/*   Updated: 2020/07/05 18:16:32 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "includes/Server.hpp"
#include <iostream>

int main()
{
	try
	{
		Server server;
		server.AcceptConnections();
	}
	catch(const char *e)
	{
		std::cerr << e << std::endl;
	}
}
