/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Response.cpp                                       :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/08/27 21:45:05 by abobas        #+#    #+#                 */
/*   Updated: 2020/10/28 01:29:20 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"

Response::Response(Data &&data) : data(data)
{	
	if (!isValid())
		return ;
	try
	{
		if (isProxy())
			handleProxy();
		else if (isFile())
			File file(this->data);
		else if (isUpload())
			Upload upload(this->data);
		else
			data.response.sendNotImplemented();
	}
	catch (const char *e)
	{
		perror(e);
		data.response.sendInternalError();
	}
}

bool Response::isValid()
{
	if (!validMethod())
	{
		data.response.sendBadMethod();
		return false;
	}
	if (data.not_found)
	{
		data.response.sendNotFound();
		return false;
	}
	return true;
}

bool Response::validMethod()
{
	if (data.method.empty())
		return false;
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

void Response::handleProxy()
{
	try
	{
		Proxy proxy(this->data);
		this->proxy = proxy.getProxySocket();
		proxy_request = proxy.getProxyRequest();
		proxy_true = true;
	}
	catch (const char *e)
	{
		throw e;
	}
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
