/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   main.cpp                                           :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/07/04 14:09:30 by abobas        #+#    #+#                 */
/*   Updated: 2020/07/21 22:26:48 by abobas        ########   odam.nl         */
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
	{
		std::cout << "Error: wrong argument" << std::endl;
		exit(1);
	}
	if (std::string(av[1]) != "--config")
	{
		std::cout << "Error: wrong argument" << std::endl;
		exit(1);
	}
	int fd = open(av[2], O_RDONLY);
	if (fd < 0)
	{
		std::cout << "Error: configuration file not found" << std::endl;
		exit(1);
	}
	close(fd);
}

int main(int ac, char **av)
{
	validate(ac, av);

	std::string error;
	std::ifstream basicIfstream(av[2]);
	std::string rawConfig((std::istreambuf_iterator<char>(basicIfstream)), std::istreambuf_iterator<char>());
    Json::Json parsed = Json::Json::parse(rawConfig, error);
	
	Server server;
	server.runtime();
}
