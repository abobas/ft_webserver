/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Response.cpp                                       :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/08/27 21:45:05 by abobas        #+#    #+#                 */
/*   Updated: 2020/10/26 18:07:24 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"
#include <iostream>

Response::Response(Data &&data) : data(data)
{
	if (!isValid())
		return;
	if (isProxy())
		Proxy proxy(data);
	else if (isCgi())
		Cgi cgi(data);
	else if (isFile())
		File file(data);
	else if (isUpload())
		Upload upload(data);
}

bool Response::isValid()
{
	if (data.not_found)
	{
		data.response.sendNotFound();
		return false;
	}
	if (!validMethod())
	{
		data.response.sendBadMethod();
		return false;
	}
	return true;
}

bool Response::validMethod()
{
	for (auto accepted : data.location["accepted-methods"].array_items())
	{
		if (accepted == data.method)
			return true;
	}
	return false;
}

bool Response::isProxy()
{
	if (data.location["proxy_pass"].string_value().size() != 0)
		return true;
	return false;
}

bool Response::isCgi()
{
	if (data.path.substr(data.path.size() - 4) == std::string(".php") ||
		data.path.substr(data.path.size() - 3) == std::string(".pl"))
		return true;
	return false;
}

bool Response::isFile()
{
	if (data.method == "GET" || data.method == "HEAD")
		return true;
	return false;
}

bool Response::isUpload()
{
	if (data.method == "PUT" || data.method == "POST")
		return true;
	return false;
}
