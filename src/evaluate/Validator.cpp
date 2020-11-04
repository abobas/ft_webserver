/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Validator.cpp                                      :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/11/03 02:44:16 by abobas        #+#    #+#                 */
/*   Updated: 2020/11/04 11:27:13 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Validator.hpp"

Log *Validator::log = Log::getInstance();

Validator Validator::getValidated(int socket, const Parser &parsed, const Matcher &matched)
{
	return Validator(socket, parsed, matched);
}

Validator::Validator(int socket, const Parser &parsed, const Matcher &matched)
	: parsed(parsed), matched(matched), socket(socket)
{
	if (!checkEmpty())
		return;
	log->logEntry("not empty");
	if (!checkMatch())
		return;
	log->logEntry("is empty");
	if (!checkProtocol())
		return;
	log->logEntry("correct protocol");
	if (!checkMethod())
		return;
	log->logEntry("methods OK");
}

bool Validator::checkEmpty()
{
	if (parsed.getMethod().empty() || parsed.getVersion().empty())
	{
		valid = false;
		Responder::getResponder(socket).sendBadRequest();
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
		Responder::getResponder(socket).sendNotFound();
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
		Responder::getResponder(socket).sendBadRequest();
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
	log->logEntry("passed cgi check");
	for (auto accepted : matched.getLocation()["accepted-methods"].array_items())
	{
		methods += accepted.string_value() + ", ";
		if (accepted.string_value() == parsed.getMethod())
			return true;
	}
	// if (!methods.empty())
	// 	methods = methods.substr(0, methods.size() - 2);
	valid = false;
	log->logEntry("request's method not accepted");
	Responder::getResponder(socket).sendBadMethod(methods);
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