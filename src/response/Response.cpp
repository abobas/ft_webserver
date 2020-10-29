/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Response.cpp                                       :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/08/27 21:45:05 by abobas        #+#    #+#                 */
/*   Updated: 2020/10/29 16:59:12 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"

Response::Response(Data &&data) : data(data)
{
	log = Log::getInstance();
	if (!isValid())
	{
		log->logEntry("invalid request");
		return;
	}
	try
	{
		if (isProxy())
			handleProxy();
		else if (isResource())
			handleResource();
		else
			this->data.response.sendNotImplemented();
	}
	catch (const char *e)
	{
		log->logError(e);
		this->data.response.sendInternalError();
	}
}

void Response::handleProxy()
{
	log->logEntry("handling proxy request");
	Proxy proxy(data);
	proxy_socket = proxy.getProxySocket();
	proxy_request = proxy.getProxyRequest();
	setProxyValue();
}

void Response::handleResource()
{
	Resource resource(data);
}

bool Response::isValid()
{
	if (!acceptedMethod())
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

bool Response::acceptedMethod()
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

bool Response::isResource()
{
	if (data.method == "GET" || data.method == "HEAD")
		return true;
	if (data.method == "PUT" || data.method == "POST")
		return true;
	return false;
}

Socket Response::getProxySocket()
{
	return proxy_socket;
}

std::string Response::getProxyRequest()
{
	return proxy_request;
}

void Response::setProxyValue()
{
	proxy_value = true;
}

bool Response::getProxyValue()
{
	return proxy_value;
}
