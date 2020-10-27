/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Response.cpp                                       :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/08/27 21:45:05 by abobas        #+#    #+#                 */
/*   Updated: 2020/10/27 21:53:44 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"
#include <iostream>

Response::Response(Data &&data) : data(data)
{
	std::cout << "request.method+path: " << data.method << " " << data.request.getPath() << std::endl;
	std::cout << "request.query: " << data.request.getQueryString() << std::endl;
	std::cout << "data.path: " << data.path << std::endl;
	std::cout << "entering validation" << std::endl;
	if (!isValid())
	{
		std::cout << "response data not valid, aborting" << std::endl;
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
		std::cout << "client request not implemented" << std::endl;
		data.response.sendNotImplemented();
	}
}

bool Response::isValid()
{
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
	return true;
}

bool Response::validMethod()
{
	if (data.location["accepted-methods"].array_items().empty())
		return false;
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
