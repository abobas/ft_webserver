/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Validator.cpp                                      :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/11/03 02:44:16 by abobas        #+#    #+#                 */
/*   Updated: 2020/11/03 13:45:56 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Validator.hpp"

Log *Validator::log = Log::getInstance();
Json Validator::config = Config::getConfig();

const Validator &Validator::getValidated(int socket, const Parser &parsed, const Matcher &matched)
{
	return Validator(socket, parsed, matched);
}

Validator::Validator(int socket, const Parser &parsed, const Matcher &matched) : socket(socket)
{
	if (!checkEmpty(parsed))
		return;
	if (!checkMatch(matched))
		return;
	if (!checkProtocol(parsed))
		return;
	if (!checkMethod(parsed, matched))
		return;
}

bool Validator::checkEmpty(const Parser &parsed)
{
	if (parsed.getMethod().empty() || parsed.getVersion().empty())
	{
		valid = false;
		Responder::getResponder(socket).sendBadRequest();
		log->logEntry("request is empty");
		return false;
	}
}

bool Validator::checkMatch(const Matcher &matched)
{
	if (!matched.isMatched())
	{
		valid = false;
		Responder::getResponder(socket).sendNotFound();
		log->logEntry("request's location couldn't be matched");
		return false;
	}
	return true;
}

bool Validator::checkProtocol(const Parser &parsed)
{
	if (parsed.getVersion() != "HTTP/1.1")
	{
		valid = false;
		Responder::getResponder(socket).sendBadRequest();
		log->logEntry("request's HTTP protocol is not supported");
		return false;
	}
	return true;
}

bool Validator::checkMethod(const Parser &parsed, const Matcher &matched)
{
	std::string methods;

	if (checkCgiExtension(parsed))
	{
		if (checkCgiMethods(parsed))
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
	Responder::getResponder(socket).sendBadMethod(methods);
	return false;
}

bool Validator::checkCgiExtension(const Parser &parsed)
{
	std::string extension;
	size_t size;

	for (auto file : config["http"]["cgi"]["files"].object_items())
	{
		extension = file.first;
		size = parsed.getPath().size();
		if (parsed.getPath().substr(size - extension.size()) == extension)
			return true;
	}
	return false;
}

bool Validator::checkCgiMethods(const Parser &parsed)
{
	std::string extension;
	size_t size;

	for (auto file : config["http"]["cgi"]["files"].object_items())
	{
		extension = file.first;
		size = parsed.getPath().size();
		if (parsed.getPath().substr(size - extension.size()) == extension)
		{
			Json::object obj = file.second.object_items();
			for (auto accepted : obj["accepted-methods"].array_items())
			{
				if (accepted.string_value() == parsed.getMethod())
					return true;
			}
		}
	}
}

bool Validator::isValid() const
{
	return valid;
}