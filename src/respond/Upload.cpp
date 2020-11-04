/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Upload.cpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/10/23 20:38:10 by abobas        #+#    #+#                 */
/*   Updated: 2020/11/04 00:48:06 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Upload.hpp"

void Upload::resolveUploadRequest(int socket, const Matcher &matched, const Parser &parsed)
{
	Upload(socket, matched, parsed);
}

Upload::Upload(int socket, const Matcher &matched, const Parser &parsed)
	: matched(matched), parsed(parsed), socket(socket)
{
	if (isMaxBodyLimit())
		return;
	if (isExistingFile())
	{
		deleteFile();
		modified = true;
	}
	addFile();
}

/**
 * TODO: per route specificeren ipv algemeen
 */
bool Upload::isMaxBodyLimit()
{
	if (matched.getConfig()["http"]["max_body"].number_value() != 0)
	{
		if (parsed.getBodySize() > matched.getConfig()["http"]["max_body"].number_value())
		{
			Responder::getResponder(socket).sendForbidden();
			return true;
		}
	}
	return false;
}

void Upload::addFile()
{
	std::ofstream file;

	file.open(matched.getPath().c_str(), std::ofstream::out);
	if (!file.is_open())
		throw "open()";
	file << parsed.getBody();
	file.close();
	if (modified)
		Responder::getResponder(socket).sendModified(matched.getPath(), parsed.getPath());
	else
		Responder::getResponder(socket).sendCreated(matched.getPath(), parsed.getPath());
}

bool Upload::isExistingFile()
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

void Upload::deleteFile()
{
	if (remove(matched.getPath().c_str()) < 0)
		throw "remove()";
}
