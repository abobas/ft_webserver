/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Validator.cpp                                      :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/11/03 02:44:16 by abobas        #+#    #+#                 */
/*   Updated: 2020/11/11 15:32:08 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Validator.hpp"

#define BAD_REQUEST 400
#define PAYLOAD_TOO_LARGE 413
#define URI_TOO_LARGE 414
#define NOT_FOUND 404
#define METHOD_NOT_ALLOWED 405
#define VERSION_NOT_SUPPORTED 505
#define MAX_HEADER_SIZE 2000

Log *Validator::log = Log::getInstance();

Validator::Validator()
{
}

Validator::Validator(int socket, Parser &parsed, Matcher &matched)
	: parsed(parsed), matched(matched), socket(socket)
{
	error = 0;
	valid = true;
	if (!checkEmpty())
		return;
	if (!checkMatch())
		return;
	if (!checkProtocol())
		return;
	if (!checkMethod())
		return;
	if (!checkContentLength())
		return;
	if (!checkMaxUri())
		return;
	if (!checkMaxHeaderSize())
		return;
	if (!checkHeaders())
		return;
}

Validator::Validator(const Validator &rhs)
{
	*this = rhs;
}

Validator &Validator::operator=(const Validator &rhs)
{
	parsed = rhs.parsed;
	matched = rhs.matched;
	socket = rhs.socket;
	valid = rhs.valid;
	error = rhs.error;
	return *this;
}

bool Validator::checkEmpty()
{
	if (parsed.getMethod().empty() || parsed.getVersion().empty() || parsed.getPath().empty())
		return returnError("invalid request syntax", BAD_REQUEST);
	return true;
}

bool Validator::checkMatch()
{
	if (!matched.isMatched())
		return returnError("request's location couldn't be matched", NOT_FOUND);
	return true;
}

bool Validator::checkProtocol()
{
	if (parsed.getVersion() != "HTTP/1.1")
		return returnError("request's HTTP protocol is not supported", VERSION_NOT_SUPPORTED);
	return true;
}

bool Validator::checkMethod()
{
	if (checkCgiExtension())
	{
		if (checkCgiMethods())
			return true;
	}
	for (auto accepted : matched.getLocation()["accepted-methods"].array_items())
	{
		methods += accepted.string_value() + ", ";
		if (accepted.string_value() == parsed.getMethod())
			return true;
	}
	if (!methods.empty())
		methods = methods.substr(0, methods.size() - 2);
	return returnError("request's method not accepted", METHOD_NOT_ALLOWED);
}

bool Validator::checkCgiExtension()
{
	std::string extension;
	size_t size = 0;

	for (auto file : matched.getConfig()["http"]["cgi"]["files"].object_items())
	{
		extension = file.first;
		size = matched.getPath().size();
		if (size > extension.size())
		{
			if (matched.getPath().substr(size - extension.size()) == extension)
				return true;
		}
	}
	return false;
}

bool Validator::checkCgiMethods()
{
	std::string extension;
	size_t size = 0;

	for (auto file : matched.getConfig()["http"]["cgi"]["files"].object_items())
	{
		extension = file.first;
		size = matched.getPath().size();
		if (matched.getPath().substr(size - extension.size()) == extension)
		{
			Json::object obj = file.second.object_items();
			for (auto accepted : obj["accepted-methods"].array_items())
			{
				if (accepted.string_value() == parsed.getMethod())
					return true;
			}
		}
	}
	return false;
}

bool Validator::checkContentLength()
{
	int content_size;

	if (!parsed.hasContent())
		return true;
	if (parsed.getHeader("content-length").size() > 10)
		return returnError("content-length is invalid", BAD_REQUEST);
	for (auto c : parsed.getHeader("content-length"))
	{
		if (!isdigit(c))
			return returnError("content-length is invalid", BAD_REQUEST);
	}
	content_size = std::stoi(parsed.getHeader("content-length"));
	if (content_size < 0)
		return returnError("content-length is invalid", BAD_REQUEST);
	if (matched.getConfig()["http"]["max_body"].number_value() != 0)
	{
		if (content_size > matched.getConfig()["http"]["max_body"].number_value())
			return returnError("max body size exceeded", PAYLOAD_TOO_LARGE);
	}
	if (matched.getLocation()["max_body"].number_value() != 0)
	{
		if (content_size > matched.getLocation()["max_body"].number_value())
			return returnError("max body size exceeded", PAYLOAD_TOO_LARGE);
	}
	return true;
}

bool Validator::checkMaxUri()
{
	if (parsed.getPath().size() > MAX_HEADER_SIZE)
		return returnError("URI too large", URI_TOO_LARGE);
	return true;
}

bool Validator::checkMaxHeaderSize()
{
	for (auto header : parsed.getHeaders())
	{
		if (header.first.size() > MAX_HEADER_SIZE || header.second.size() > MAX_HEADER_SIZE)
			return returnError("header size too large", URI_TOO_LARGE);
	}
	return true;
}

bool Validator::checkHeaders()
{
	for (auto header : parsed.getHeaders())
	{
		if (header.first.empty() || header.second.empty())
			return returnError("header formatting is invalid", BAD_REQUEST);
		for (auto c : header.first)
		{
			if (isspace(c))
				return returnError("header formatting is invalid", BAD_REQUEST);
		}
	}
	return true;
}

bool Validator::returnError(const char *message, int err)
{
	valid = false;
	log->logEntry(message);
	error = err;
	return false;
}

int Validator::getError()
{
	return error;
}

bool Validator::isValid()
{
	return valid;
}

std::string Validator::getMethods()
{
	return methods;
}