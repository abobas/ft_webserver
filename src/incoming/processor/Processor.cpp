/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Processor.cpp                                      :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/11/05 13:13:06 by abobas        #+#    #+#                 */
/*   Updated: 2020/11/06 22:36:43 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Processor.hpp"

#define MODIFIED 200
#define CREATED 201
#define INTERNAL_ERROR 500

Log *Processor::log = Log::getInstance();
std::map<int, Processor *> Processor::processors;

Processor::Processor(int socket, Parser &parsed, Matcher &matched, std::string type)
	: parsed(parsed), matched(matched), request_type(type), socket(socket)
{
	processed = false;
}

Processor *Processor::getInstance(int socket, Parser &parsed, Matcher &matched, std::string type) if (!processors[socket])
{
	processors[socket] = new Processor(socket, parsed, matched, type);
	log->logEntry("created processor", socket);
}
return processors[socket];
}

void Processor::deleteInstance(int socket)
{
	if (processors[socket])
	{
		delete processors[socket];
		processors[socket] = NULL;
		log->logEntry("deleted processor", socket);
		Upload::deleteInstance(socket);
	}
}

void Processor::processRequest()
{
	if (request_type == "upload")
	{
		upload = Upload::getInstance(socket, parsed, matched);
		upload->processUploadRequest();
	}
}

bool Processor::isProcessed()
{
	if (request_type == "upload" && upload)
		return upload->isProcessed();
	else
		return processed;
}

int Processor::getUploadStatus()
{
	return upload->getStatus();
}

int Processor::getError()
{
	if (request_type == "upload")
		return upload->getError();
	else
		return 0; // cgi
}

std::string Processor::getUploadPath()
{
	return upload->getPath();
}