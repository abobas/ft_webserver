/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Upload.cpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/11/06 14:09:14 by abobas        #+#    #+#                 */
/*   Updated: 2020/11/08 00:09:05 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Upload.hpp"

#define MODIFIED 200
#define CREATED 201
#define INTERNAL_ERROR 500

Log *Upload::log = Log::getInstance();
std::map<int, Upload *> Upload::uploads;

Upload::Upload(int socket, Parser &parsed, Matcher &matched)
	: parsed(parsed), matched(matched), socket(socket)
{
	initialized = false;
	processed = false;
	error = 0;
	status = 0;
	file = 0;
}

Upload *Upload::getInstance(int socket, Parser &parsed, Matcher &matched)
{
	if (!uploads[socket])
	{
		uploads[socket] = new Upload(socket, parsed, matched);
		log->logEntry("created upload", socket);
	}
	return uploads[socket];
}

void Upload::deleteInstance(int socket)
{
	if (uploads[socket])
	{
		delete uploads[socket];
		uploads.erase(socket);
		log->logEntry("deleted upload", socket);
	}
}

void Upload::processUploadRequest()
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

bool Upload::initializeUpload()
{
	setPath();
	if (isExistingFile())
	{
		log->logEntry("file exists");
		if (!deleteFile())
			return false;
		status = MODIFIED;
	}
	if (!createFile())
		return false;
	return true;
}

void Upload::setPath()
{
	upload_path = matched.getPath();
	if (upload_path[upload_path.size() - 1] == '/')
		upload_path += "upload";
}

bool Upload::isExistingFile()
{
	int fd;

	fd = open(upload_path.c_str(), O_RDONLY);
	if (fd < 0)
	{
		status = CREATED;
		return false;
	}
	else
	{
		close(fd);
		return true;
	}
}

bool Upload::deleteFile()
{
	if (remove(upload_path.c_str()) < 0)
	{
		log->logError("remove()");
		error = INTERNAL_ERROR;
		return false;
	}
	return true;
}

bool Upload::createFile()
{
	file = open(upload_path.c_str(), O_WRONLY | O_CREAT, 0777);
	if (file < 0)
	{
		log->logError("open()");
		error = INTERNAL_ERROR;
		return false;
	}
	return true;
}

void Upload::processUpload()
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

bool Upload::uploadBody(const char *body, size_t bytes)
{
	int ret = write(file, body, bytes);
	if (ret < 0)
	{
		error = INTERNAL_ERROR;
		return false;
	}
	return true;
}

std::string Upload::getBodyType()
{
	if (parsed.hasContent())
		return "content";
	else if (parsed.isChunked())
		return "chunked";
	else
		return "";
}

size_t Upload::getBodySize()
{
	if (parsed.hasContent())
		return stoi(parsed.getHeader("content-length"));
	else
		return 0;
}

std::string Upload::getPath()
{
	return upload_path;
}

bool Upload::isProcessed()
{
	return processed;
}

int Upload::getStatus()
{
	return status;
}

int Upload::getError()
{
	return error;
}
