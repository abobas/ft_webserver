/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Data.cpp                                           :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/09/18 17:47:11 by abobas        #+#    #+#                 */
/*   Updated: 2020/10/26 17:51:29 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Data.hpp"

Data::Data(Socket &client, Json &config, std::string &request)
	: client(client), config(config), request(client, request), response(this->request)
{
	setServer();
	setLocation();
	setPath();
	setMethod();
}

Data getData(Socket &client, Json &config, std::string &request)
{
	return Data(client, config, request);
}

void Data::setServer()
{
	size_t pos = request.getHeader("host").find(':');
	if (pos == std::string::npos)
	{
		server = config["http"]["servers"][0].object_items();
		return;
	}
	int port = std::stoi(request.getHeader("host").substr(pos + 1));
	for (auto serv : config["http"]["servers"].array_items())
	{
		if (serv["listen"].number_value() == port)
		{
			server = serv.object_items();
			return;
		}
	}
}

void Data::setLocation()
{
	path = request.getPath();
	path_length = 0;
	for (auto loc : server["locations"].object_items())
	{
		if (path.substr(0, loc.first.size()) == loc.first && loc.first.size() > path_length)
		{
			location = loc.second.object_items();
			path_length = loc.first.size();
		}
	}
	if (path_length == 0)
		not_found = true;
}

void Data::setPath()
{
	path = location["root"].string_value();
	if (path_length == 1)
		path += request.getPath();
	else
		path += request.getPath().substr(path_length, std::string::npos);
}

void Data::setMethod()
{
	method = request.getMethod();
}
