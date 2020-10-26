/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Response.cpp                                       :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/08/27 21:45:05 by abobas        #+#    #+#                 */
/*   Updated: 2020/10/26 22:11:03 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"
#include <iostream>

Response::Response(Data &&data) : data(data)
{
	std::cout << "request.method+path: " << data.method << " " << data.request.getPath() << std::endl;
	std::cout << "data.path: " << data.path << std::endl;
	
	if (!isValid())
	{
		std::cout << "not valid" << std::endl;
		return;
	}
	std::cout << "finished validation" << std::endl;
	if (isProxy())
	{
		std::cout << "entering proxy" << std::endl;
		Proxy proxy(this->data);
		if (proxy.proxySuccess())
		{
			this->proxy = proxy.getProxySocket();
			proxy_request = proxy.getProxyRequest();
			proxy_true = true;
			std::cout << "finished proxy" << std::endl;
		}
	}
	else if (isCgi())
	{
		std::cout << "entering cgi" << std::endl;
		Cgi cgi(this->data);
		std::cout << "finished cgi" << std::endl;
	}
	else if (isFile())
	{
		std::cout << "entering file" << std::endl;
		File file(this->data);
		std::cout << "finished file" << std::endl;
	}
	else if (isUpload())
	{
		std::cout << "entering upload" << std::endl;
		Upload upload(this->data);
		std::cout << "finished upload" << std::endl;
	}
	else
	{
		std::cout << "service unavailable" << std::endl;
		data.response.sendServiceUnavailable();
	}
}

bool Response::isValid()
{
	std::cout << "entering validation" << std::endl;
	if (data.method.empty())
		return false;
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
	std::cout << "finished method validation" << std::endl;
	return true;
}

bool Response::validMethod()
{
	std::cout << "entering method validation" << std::endl;
	if (data.location["accepted-methods"].array_items().empty())
	{
		std::cout << "Config error: add accepted-methods in server location" << std::endl;
		return false;
	}
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

bool Response::isProxySet()
{
	return proxy_true;
}
