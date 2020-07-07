/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   main.cpp                                           :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/07/04 14:09:30 by abobas        #+#    #+#                 */
/*   Updated: 2020/07/07 18:08:34 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "includes/Server.hpp"
#include <iostream>

int main()
{
	try
	{
		Server server;
		server.acceptConnections();
	}
	catch(const char *e)
	{
		std::cerr << e << std::endl;
	}
}
