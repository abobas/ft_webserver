/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Response.cpp                                       :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/08/27 21:45:05 by abobas        #+#    #+#                 */
/*   Updated: 2020/10/31 20:47:36 by abobas        ########   odam.nl         */
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
	std::string methods;
	
	if (data.method.empty() || data.request.getVersion() != "HTTP/1.1")
	{
		log->logEntry("bad request");
		data.response.sendBadRequest();
		return false;
	}
	if (!acceptedMethod(methods))
	{
		log->logEntry("bad method");
		data.response.sendBadMethod(methods);
		return false;
	}
	if (data.not_found)
	{
		log->logEntry("not found");
		data.response.sendNotFound();
		return false;
	}
	return true;
}

bool Response::acceptedMethod(std::string &methods)
{
	if (isCgi())
	{
		for (auto file : data.config["http"]["cgi"]["files"].object_items())
		{
			std::string format = file.first;
			if (data.path.substr(data.path.size() - format.size()) == format)
			{
				Json::object obj = file.second.object_items();
				for (auto accepted : obj["accepted-methods"].array_items())
				{
					if (accepted.string_value() == data.method)
						return true;
				}
			}
		}	
	}
	for (auto accepted : data.location["accepted-methods"].array_items())
	{
		methods += accepted.string_value() + ", ";
		if (accepted.string_value() == data.method)
			return true;
	}
	if (!methods.empty())
		methods = methods.substr(0, methods.size() - 2);
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
	for (auto file : data.config["http"]["cgi"]["files"].object_items())
	{
		std::string format = file.first;
		if (data.path.substr(data.path.size() - format.size()) == format)
			return true;
	}
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
