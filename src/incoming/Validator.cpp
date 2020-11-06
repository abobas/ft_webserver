/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Validator.cpp                                      :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/11/03 02:44:16 by abobas        #+#    #+#                 */
/*   Updated: 2020/11/06 22:53:18 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Validator.hpp"

#define BAD_REQUEST 400
#define PAYLOAD_TOO_LARGE 413
#define NOT_FOUND 404
#define METHOD_NOT_ALLOWED 405

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
	if (!checkMaxBodyLimit())
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
	if (parsed.getMethod().empty() || parsed.getVersion().empty())
	{
		valid = false;
		error = BAD_REQUEST;
		log->logEntry("request is empty");
		return false;
	}
	return true;
}

bool Validator::checkMatch()
{
	if (!matched.isMatched())
	{
		valid = false;
		error = NOT_FOUND;
		log->logEntry("request's location couldn't be matched");
		return false;
	}
	return true;
}

bool Validator::checkProtocol()
{
	if (parsed.getVersion() != "HTTP/1.1")
	{
		valid = false;
		error = BAD_REQUEST;
		log->logEntry("request's HTTP protocol is not supported");
		return false;
	}
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
	valid = false;
	log->logEntry("request's method not accepted");
	error = METHOD_NOT_ALLOWED;
	return false;
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

bool Validator::checkMaxBodyLimit()
{
	int content_size;

	if (!parsed.hasContent())
		return true;
	content_size = std::stoi(parsed.getHeader("content-length"));
	if (matched.getConfig()["http"]["max_body"].number_value() != 0)
	{
		if (content_size > matched.getConfig()["http"]["max_body"].number_value())
		{
			valid = false;
			log->logEntry("max body size exceeded");
			error = PAYLOAD_TOO_LARGE;
			return false;
		}
	}
	if (matched.getLocation()["max_body"].number_value() != 0)
	{
		if (content_size > matched.getLocation()["max_body"].number_value())
		{
			valid = false;
			log->logEntry("max body size exceeded");
			error = PAYLOAD_TOO_LARGE;
			return false;
		}
	}
	return true;
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