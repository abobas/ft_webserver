/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Upload.cpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/10/23 20:38:10 by abobas        #+#    #+#                 */
/*   Updated: 2020/11/04 21:43:12 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Upload.hpp"

Log *Upload::log = Log::getInstance();

void Upload::resolveUploadRequest(int socket, Matcher &matched, Parser &parsed)
{
	Upload(socket, matched, parsed);
}

Upload::Upload(int socket, Matcher &matched, Parser &parsed)
	: matched(matched), parsed(parsed), respond(socket, parsed), socket(socket)
{
	if (isExistingFile())
	{
		deleteFile();
		modified = true;
	}
	addFile();
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
		respond.sendModified(matched.getPath(), parsed.getPath());
	else
		respond.sendCreated(matched.getPath(), parsed.getPath());
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
