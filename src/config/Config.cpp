/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Config.cpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/11/03 02:12:43 by abobas        #+#    #+#                 */
/*   Updated: 2020/11/03 02:27:53 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"

Json Config::config;

void Config::initializeConfig(const char *argument)
{
	std::ifstream file(argument);
	std::string raw((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	std::string error;
	Json parsed = Json::parse(raw, error);
	if (parsed == nullptr)
		throw error.c_str();
	config = parsed;
}

Config::Config()
{
}

Json Config::getConfig()
{
	return config;
}
