/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Validator.cpp                                      :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/11/03 02:44:16 by abobas        #+#    #+#                 */
/*   Updated: 2020/11/04 16:22:50 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Validator.hpp"

Log *Validator::log = Log::getInstance();

Validator Validator::getValidated(int socket, Parser &parsed, Matcher &matched)
{
	return Validator(socket, parsed, matched);
}

Validator::Validator(int socket, Parser &parsed, Matcher &matched)
	: parsed(parsed), matched(matched), respond(socket, parsed), socket(socket)
{
	if (!checkEmpty())
		return;
	if (!checkMatch())
		return;
	if (!checkProtocol())
		return;
	if (!checkMethod())
		return;
}

bool Validator::checkEmpty()
{
	if (parsed.getMethod().empty() || parsed.getVersion().empty())
	{
		valid = false;
		respond.sendBadRequest();
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
		respond.sendNotFound();
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
		respond.sendBadRequest();
		log->logEntry("request's HTTP protocol is not supported");
		return false;
	}
	return true;
}

bool Validator::checkMethod()
{
	std::string methods;

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
	respond.sendBadMethod(methods);
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

bool Validator::isValid() const
{
	return valid;
}