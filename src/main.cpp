/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   main.cpp                                           :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/07/04 14:09:30 by abobas        #+#    #+#                 */
/*   Updated: 2020/11/03 02:30:26 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "config/Config.hpp"
#include <iostream>
#include <fcntl.h>

void validateInput(int ac, char **av)
{
	if (ac != 3)
		throw "Error: wrong argument\nUsage: ./webserv --config config.json";
	if (std::string(av[1]) != "--config")
		throw "Error: wrong argument\nUsage: ./webserv --config config.json";
	int fd = open(av[2], O_RDONLY);
	if (fd < 0)
		throw "Error: configuration file not found";
	close(fd);
}

int main(int ac, char **av)
{
	try
	{
		validateInput(ac, av);
		Config::initializeConfig(av[1]);
		Server server();
	}
	catch (const char *e)
	{
		std::cerr << e << std::endl;
		return 1;
	}
	return 0;
}
