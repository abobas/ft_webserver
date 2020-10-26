/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Response.cpp                                       :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/08/27 21:45:05 by abobas        #+#    #+#                 */
/*   Updated: 2020/10/26 19:38:12 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"
#include <iostream>

Response::Response(Data &&data) : data(data)
{
	std::cout << "data.method: " << data.method << std::endl;
	std::cout << "data.path: " << data.path << std::endl;
	std::cout << "request.path: " << data.request.getPath() << std::endl;
	std::cout << "---entering response control flow---" << std::endl
			  << std::endl;
	if (!isValid())
		return;
	if (isProxy())
	{
		Proxy proxy(this->data);
		this->proxy = proxy.getProxySocket();
		proxy_request = proxy.getProxyRequest();
	}
	else if (isCgi())
		Cgi cgi(this->data);
	else if (isFile())
		File file(this->data);
	else if (isUpload())
		Upload upload(this->data);
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
		if (accepted.string_value() == data.method)
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

Socket Response::getProxySocket()
{
	return proxy;
}

std::string Response::getProxyRequest()
{
	return proxy_request;
}
