/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Matcher.cpp                                        :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/11/03 01:06:15 by abobas        #+#    #+#                 */
/*   Updated: 2020/11/03 15:19:26 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Matcher.hpp"

Json Matcher::config = Config::getConfig();

const Matcher &Matcher::getMatched(const Parser &parsed)
{
	return Matcher(parsed);
}

Matcher::Matcher(const Parser &parsed)
{
	matchServer(parsed);
	matchLocation(parsed);
}

void Matcher::matchServer(const Parser &parsed)
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

void Matcher::matchLocation(const Parser &parsed)
{
	int path_length = 0;
	
	matched_path = parsed.getPath();
	for (auto loc : server["locations"].object_items())
	{
		if (matched_path.substr(0, loc.first.size()) == loc.first && loc.first.size() > path_length)
		{
			location = loc.second.object_items();
			path_length = loc.first.size();
		}
	}
	if (path_length == 0)
	{
		no_match = true;
		return ;
	}
	matched_path = location["root"].string_value();
	if (path_length == 1)
		matched_path += parsed.getPath();
	else
		matched_path += parsed.getPath().substr(path_length, std::string::npos);
}

bool Matcher::isMatched() const
{
	return no_match;
}

std::string Matcher::getPath() const
{
	return matched_path;
}