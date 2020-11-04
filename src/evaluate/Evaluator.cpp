/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Evaluator.cpp                                      :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/11/03 00:54:16 by abobas        #+#    #+#                 */
/*   Updated: 2020/11/04 20:38:23 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Evaluator.hpp"

Log *Evaluator::log = Log::getInstance();

Evaluator Evaluator::getEvaluator(int socket, std::string &message, Json config)
{
	return Evaluator(socket, message, config);
}

Evaluator::Evaluator(int socket, std::string &message, Json config)
	: socket(socket)
{
	Parser parsed = Parser::getParsed(message);
	Matcher matched = Matcher::getMatched(parsed, config);
	Validator validated = Validator::getValidated(socket, parsed, matched);
	if (!validated.isValid())
		return;
	evaluateRequest(parsed, matched);
}

void Evaluator::evaluateRequest(Parser &parsed, Matcher &matched)
{
	struct stat file;
	Responder respond(socket, parsed);

	try
	{
		if (isProxy(matched))
			resolveProxyRequest(matched, parsed);
		else if (isCgi(matched, parsed))
			resolveCgiRequest(matched, parsed);
		else if (isUpload(parsed))
			resolveUploadRequest(matched, parsed);
		else
		{
			if (!isFile(matched, &file))
			{
				respond.sendNotFound();
				return;
			}
			if (isDirectory(&file))
				resolveDirectoryRequest(matched, parsed);
			else if (isRegular(&file))
				resolveFileRequest(matched, respond);
			else
				respond.sendNotImplemented();
		}
	}
	catch (const char *e)
	{
		log->logError(e);
		respond.sendInternalError();
	}
}

void Evaluator::resolveProxyRequest(Matcher &matched, Parser &parsed)
{
	log->logEntry("resolving proxy request");
	Proxy proxy = Proxy::resolveProxyRequest(matched, parsed);
	proxy_socket = proxy.getProxySocket();
	proxy_request = proxy.getProxyRequest();
	proxy_set = true;
}

void Evaluator::resolveCgiRequest(Matcher &matched, Parser &parsed)
{
	log->logEntry("resolving CGI request");
	Cgi::resolveCgiRequest(socket, matched, parsed);
}

void Evaluator::resolveUploadRequest(Matcher &matched, Parser &parsed)
{
	log->logEntry("resolving upload request");
	Upload::resolveUploadRequest(socket, matched, parsed);
}

void Evaluator::resolveDirectoryRequest(Matcher &matched, Parser &parsed)
{
	log->logEntry("resolving directory request");
	Directory::resolveDirectoryRequest(socket, matched, parsed);
}

void Evaluator::resolveFileRequest(Matcher &matched, Responder &respond)
{
	log->logEntry("resolving file request");
	respond.sendFile(matched.getPath());
}

bool Evaluator::isProxy(Matcher &matched)
{
	if (!matched.getLocation()["proxy_pass"].string_value().empty())
		return true;
	return false;
}

bool Evaluator::isCgi(Matcher &matched, Parser &parsed)
{
	std::string extension;
	size_t size;

	for (auto file : matched.getConfig()["http"]["cgi"]["files"].object_items())
	{
		extension = file.first;
		size = matched.getPath().size();
		if (size > extension.size())
		{
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
	}
	return false;
}

bool Evaluator::isUpload(Parser &parsed)
{
	return parsed.getMethod() == "PUT" || parsed.getMethod() == "POST";
}

bool Evaluator::isFile(Matcher &matched, struct stat *file)
{
	if (stat(matched.getPath().c_str(), file) < 0)
		return false;
	return true;
}

bool Evaluator::isDirectory(struct stat *file)
{
	return S_ISDIR(file->st_mode);
}

bool Evaluator::isRegular(struct stat *file)
{
	return S_ISREG(file->st_mode);
}

Socket Evaluator::getProxySocket()
{
	return proxy_socket;
}

std::string Evaluator::getProxyRequest()
{
	return proxy_request;
}

bool Evaluator::isProxyRequest()
{
	return proxy_set;
}