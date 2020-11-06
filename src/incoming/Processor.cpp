/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Processor.cpp                                      :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/11/05 13:13:06 by abobas        #+#    #+#                 */
/*   Updated: 2020/11/06 00:09:11 by abobas        ########   odam.nl         */
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
	error = 0;
	status = CREATED;
	file = 0;
}

Processor *Processor::getInstance(int socket, Parser &parsed, Matcher &matched, std::string type)
{
	if (!processors[socket])
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
	}
}

void Processor::processRequest()
{
	if (request_type == "upload")
	{
		if (!initialized)
		{
			if (!initializeUpload())
			{
				processed = true;
				return;
			}
			initialized = true;
		}
		processUpload();
	}
	// if (request_type == "cgi")
	// 	processCgi();
}

void Processor::processUpload()
{
	receiver = Receiver::getInstance(socket);
	if (!receiver->bodyInitialized())
	{
		receiver->initializeBodyType(getBodyType(), getBodySize());
		log->logEntry("initialized bodyreceiver socket", socket);
	}
	if (!receiver->bodyReceived())
	{
		receiver->receiveBody();
		if (!uploadBody(receiver->getBodyData(), receiver->getBodyDataLength()))
		{
			processed = true;
			return;
		}
	}
	if (receiver->bodyReceived())
	{
		processed = true;
		close(file);
		log->logEntry("processed upload request", socket);
	}
}

bool Processor::uploadBody(const char *body, size_t bytes)
{
	int ret = write(file, body, bytes);
	if (ret < 0)
	{
		error = INTERNAL_ERROR;
		return false;
	}
	return true;
}

bool Processor::initializeUpload()
{
	if (isExistingFile())
	{
		log->logEntry("file exists");
		if (!deleteFile())
			return false;
		status = MODIFIED;
	}
	if (!createFile())
	{
		status = INTERNAL_ERROR;
		return false;
	}
	return true;
}

bool Processor::createFile()
{
	file = open(matched.getPath().c_str(), O_WRONLY | O_CREAT, 0777);
	if (file < 0)
	{
		log->logError("open()");
		return false;
	}
	return true;
}

bool Processor::isExistingFile()
{
	int fd;

	fd = open(matched.getPath().c_str(), O_RDONLY);
	if (fd < 0)
		return false;
	else
	{
		close(fd);
		return true;
	}
}

bool Processor::deleteFile()
{
	if (remove(matched.getPath().c_str()) < 0)
	{
		log->logError("remove()");
		status = INTERNAL_ERROR;
		return false;
	}
	return true;
}

std::string Processor::getBodyType()
{
	if (parsed.hasContent())
		return "content";
	else if (parsed.isChunked())
		return "chunked";
	else
		return "";
}

size_t Processor::getBodySize()
{
	if (parsed.hasContent())
		return stoi(parsed.getHeader("content-length"));
	else
		return 0;
}

bool Processor::isProcessed()
{
	return processed;
}

int Processor::getStatus()
{
	return status;
}

int Processor::getError()
{
	return error;
}