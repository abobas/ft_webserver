/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Matcher.cpp                                        :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/11/03 01:06:15 by abobas        #+#    #+#                 */
/*   Updated: 2020/11/06 22:41:46 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Matcher.hpp"

Matcher::Matcher()
{
}

Matcher::Matcher(Parser &parsed, Json &config) : config(config), parsed(parsed)
{
	match = false;
	matchServer();
	matchLocation();
}

Matcher::Matcher(const Matcher &rhs)
{
	*this = rhs;
}

Matcher& Matcher::operator=(const Matcher &rhs)
{
	server = rhs.server;
	location = rhs.location;
	matched_path = rhs.matched_path;
	config = rhs.config;
	parsed = rhs.parsed;
	match = rhs.match;
	return *this;
}

void Matcher::matchServer()
{
	size_t pos;
	int port;

	pos = parsed.getHeader("host").find(':');
	if (pos == std::string::npos)
	{
		server = config["http"]["servers"][0].object_items();
		return;
	}
	port = std::stoi(parsed.getHeader("host").substr(pos + 1));
	for (auto serv : config["http"]["servers"].array_items())
	{
		if (serv["listen"].number_value() == port)
		{
			server = serv.object_items();
			return;
		}
	}
}

void Matcher::matchLocation()
{
	size_t path_length = 0;

	matched_path = parsed.getPath();
	for (auto loc : server["locations"].object_items())
	{
		if (matched_path.substr(0, loc.first.size()) == loc.first && loc.first.size() > path_length)
		{
			match = true;
			location = loc.second.object_items();
			path_length = loc.first.size();
		}
	}
	if (path_length == 0)
		return;
	matched_path = location["root"].string_value();
	if (path_length == 1)
		matched_path += parsed.getPath();
	else
		matched_path += parsed.getPath().substr(path_length, std::string::npos);
}

bool Matcher::isMatched()
{
	return match;
}

Json Matcher::getConfig()
{
	return config;
}

std::string Matcher::getPath()
{
	return matched_path;
}

Json::object Matcher::getServer()
{
	return server;
}

Json::object Matcher::getLocation()
{
	return location;
}
