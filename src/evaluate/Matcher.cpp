/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Matcher.cpp                                        :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/11/03 01:06:15 by abobas        #+#    #+#                 */
/*   Updated: 2020/11/04 15:56:26 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Matcher.hpp"

Matcher Matcher::getMatched(Parser &parsed, Json config)
{
	return Matcher(parsed, config);
}

Matcher::Matcher(Parser &parsed, Json config): config(config), parsed(parsed)
{
	matchServer();
	matchLocation();
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
		return ;
	matched_path = location["root"].string_value();
	if (path_length == 1)
		matched_path += parsed.getPath();
	else
		matched_path += parsed.getPath().substr(path_length, std::string::npos);
}

bool Matcher::isMatched() const
{
	return match;
}

Json Matcher::getConfig() const
{
	return config;
}

std::string Matcher::getPath() const
{
	return matched_path;
}

Json::object Matcher::getServer() const
{
	return server;
}

Json::object Matcher::getLocation() const
{
	return location;
}
