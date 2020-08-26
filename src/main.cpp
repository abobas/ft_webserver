/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   main.cpp                                           :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/07/04 14:09:30 by abobas        #+#    #+#                 */
/*   Updated: 2020/08/25 22:52:56 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Json.hpp"

#include <iostream>
#include <fstream>
#include <fcntl.h>

void validate(int ac, char **av)
{
	if (ac != 3)
		throw("Error: wrong argument\nUsage: ./webserv --config config.json");
	if (std::string(av[1]) != "--config")
		throw("Error: wrong argument\nUsage: ./webserv --config config.json");
	int fd = open(av[2], O_RDONLY);
	if (fd < 0)
		throw("Error: configuration file not found");
	close(fd);
}

int main(int ac, char **av)
{
	try
	{
		validate(ac, av);
		std::string error;
		std::ifstream file(av[2]);
		std::string raw((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
		Json::Json config = Json::Json::parse(raw, error);
		Server server(config);
		server.runtime();
	}
	catch (const char *e)
	{
		std::cerr << e << std::endl;
	}
}
