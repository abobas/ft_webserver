/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   main.cpp                                           :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/07/04 14:09:30 by abobas        #+#    #+#                 */
/*   Updated: 2020/10/28 00:48:58 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "json/Json.hpp"
#include <iostream>
#include <fcntl.h>

void validateInput(int ac, char **av)
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

Json getConfig(char *config)
{
	std::ifstream file(config);
	std::string raw((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	std::string error;
	Json parsed = Json::parse(raw, error);
	if (parsed == nullptr)
		throw(error.c_str());
	return (parsed);
}

int main(int ac, char **av)
{
	
	try
	{
		validateInput(ac, av);
		Server server(getConfig(av[2]));
	}
	catch (const char *e)
	{
		std::cerr << e << std::endl;
	}
}
