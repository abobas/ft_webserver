/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Evaluator.cpp                                      :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/11/03 00:54:16 by abobas        #+#    #+#                 */
/*   Updated: 2020/11/03 16:29:13 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Evaluator.hpp"

Log *Evaluator::log = Log::getInstance();
Json Evaluator::config = Config::getConfig();

Evaluator Evaluator::getEvaluator(int socket, std::string &message)
{
	return Evaluator(socket, message);
}

Evaluator::Evaluator(int socket, std::string &message) : socket(socket)
{
	const Parser &parsed = Parser::getParsed(socket, message);
	const Matcher &matched = Matcher::getMatched(parsed);
	const Validator &validated = Validator::getValidated(socket, parsed, matched);
	if (!validated.isValid())
		return;
	evaluateRequest(parsed, matched);
}

void Evaluator::evaluateRequest(const Parser &parsed, const Matcher &matched)
{
	struct stat file;
	
	try
	{
		if (isProxy(matched))
		// 	// handleProxy();
		else if (isCgi(parsed))
			resolveCgiRequest(matched, parsed);
		else if (isUpload(parsed))
			resolveUploadRequest(matched, parsed);
		else
		{
			if (!isFile(matched, &file))
				return;
			if (isDirectory(&file))
				resolveDirectoryRequest(matched, parsed);
			else if (isRegular(&file))
				resolveFileRequest(matched);
			else
				Responder::getResponder(socket).sendNotImplemented();
		}
	}
	catch (const char *e)
	{
		log->logError(e);
		Responder::getResponder(socket).sendInternalError();
	}
}

void Evaluator::resolveCgiRequest(const Matcher &matched, const Parser &parsed)
{
	log->logEntry("resolving upload request");
	Cgi::resolveCgiRequest(socket, matched, parsed);
}

void Evaluator::resolveUploadRequest(const Matcher &matched, const Parser &parsed)
{
	log->logEntry("resolving upload request");
	Upload::resolveUploadRequest(socket, matched, parsed);
}

void Evaluator::resolveDirectoryRequest(const Matcher &matched, const Parser &parsed)
{
	log->logEntry("resolving directory request");
	Directory::resolveDirectoryRequest(socket, matched, parsed);
}

void Evaluator::resolveFileRequest(const Matcher &matched)
{
	log->logEntry("resolving file request");
	Responder::getResponder(socket).sendFile(matched.getPath());
}

bool Evaluator::isProxy(const Matcher &matched)
{
	if (!matched.getLocation()["proxy_pass"].string_value().empty())
		return true;
	return false;
}

bool Evaluator::isCgi(const Parser &parsed)
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

bool Evaluator::isUpload(const Parser &parsed)
{
	return parsed.getMethod() == "PUT" || parsed.getMethod() == "POST";
}

bool Evaluator::isFile(const Matcher &matched, struct stat *file)
{
	if (stat(matched.getPath().c_str(), file) < 0)
	{
		Responder::getResponder(socket).sendNotFound();
		return false;
	}
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
