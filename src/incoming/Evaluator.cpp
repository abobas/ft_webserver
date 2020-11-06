/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Evaluator.cpp                                      :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/11/03 00:54:16 by abobas        #+#    #+#                 */
/*   Updated: 2020/11/06 22:42:15 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Evaluator.hpp"

#define NOT_FOUND 404
#define NOT_IMPLEMENTED 501

Log *Evaluator::log = Log::getInstance();
Json Evaluator::config = Json();
std::map<int, Evaluator *> Evaluator::evaluators;

void Evaluator::initializeEvaluator(Json &config)
{
	Evaluator::config = config;
}

Evaluator::Evaluator(int socket) : socket(socket)
{
	error = 0;
	evaluated = false;
	processed = false;
}

Evaluator *Evaluator::getInstance(int socket)
{
	if (!evaluators[socket])
	{
		evaluators[socket] = new Evaluator(socket);
		log->logEntry("created evaluator", socket);
	}
	return evaluators[socket];
}

void Evaluator::deleteInstance(int socket)
{
	if (evaluators[socket])
	{
		delete evaluators[socket];
		evaluators[socket] = NULL;
		log->logEntry("deleted evaluator", socket);
		Processor::deleteInstance(socket);
	}
}

void Evaluator::evaluateHeaders(std::string &&headers)
{
	parsed = Parser(headers);
	matched = Matcher(parsed, config);
	validated = Validator(socket, parsed, matched);
	if (!validated.isValid())
	{
		error = validated.getError();
		evaluated = true;
		processed = true;
		return;
	}
	evaluateRequest();
	evaluated = true;
	if (getError())
	{
		processed = true;
		return;
	}
	log->logEntry("request type is " + request_type);
	evaluated = true;
	if (request_type != "upload" && request_type != "cgi")
		processed = true;
}

void Evaluator::processRequest()
{
	processor = Processor::getInstance(socket, parsed, matched, request_type, error);
	if (!processor->isProcessed())
	{
		processor->processRequest();
		processed = processor->isProcessed();
	}
	if (processor->isProcessed())
	{
		log->logEntry("done processing");
		error = processor->getError();
	}
}

void Evaluator::evaluateRequest()
{
	struct stat file;

	if (isProxy())
		request_type = "proxy";
	else if (isCgi())
		request_type = "cgi";
	else if (isUpload())
		request_type = "upload";
	else
	{
		if (!isFile(&file))
		{
			error = NOT_FOUND;
			return;
		}
		if (isDirectory(&file))
			request_type = "dir";
		else if (isRegular(&file))
			request_type = "file";
		else
		{
			log->logEntry("request type is not implemented");
			error = NOT_IMPLEMENTED;
		}
	}
}

bool Evaluator::isProxy()
{
	if (!matched.getLocation()["proxy_pass"].string_value().empty())
		return true;
	return false;
}

bool Evaluator::isCgi()
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

bool Evaluator::isUpload()
{
	return parsed.getMethod() == "PUT" || parsed.getMethod() == "POST";
}

bool Evaluator::isFile(struct stat *file)
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

Parser &Evaluator::getParsed()
{
	return parsed;
}

Matcher &Evaluator::getMatched()
{
	return matched;
}

bool Evaluator::isEvaluated()
{
	return evaluated;
}

bool Evaluator::isProcessed()
{
	return processed;
}

int Evaluator::getError()
{
	return error;
}

std::string Evaluator::getType()
{
	return request_type;
}

int Evaluator::getUploadStatus()
{
	return processor->getUploadStatus();
}

std::string Evaluator::getUploadPath()
{
	return processor->getUploadPath();
}

std::string Evaluator::getValidMethods()
{
	return validated.getMethods();
}