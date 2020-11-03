/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Upload.cpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/10/23 20:38:10 by abobas        #+#    #+#                 */
/*   Updated: 2020/11/03 16:26:37 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Upload.hpp"

Json Upload::config = Config::getConfig();

void Upload::resolveUploadRequest(int socket, const Matcher &matched, const Parser &parsed)
{
	Upload up(socket, matched, parsed);

	if (up.isMaxBodyLimit(parsed))
		return;
	if (up.isExistingFile(matched))
	{
		up.deleteFile(matched);
		up.modified = true;
	}
	up.addFile(matched, parsed);
}

Upload::Upload(int socket, const Matcher &matched, const Parser &parsed) : socket(socket)
{
}

/**
 * TODO: per route specificeren ipv algemeen
 */
bool Upload::isMaxBodyLimit(const Parser &parsed)
{
	if (config["http"]["max_body"].number_value() != 0)
	{
		if (parsed.getBody().size() > config["http"]["max_body"].number_value())
		{
			Responder::getResponder(socket).sendForbidden();
			return true;
		}
	}
	return false;
}

void Upload::addFile(const Matcher &matched, const Parser &parsed)
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

bool Upload::isExistingFile(const Matcher &matched)
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

void Upload::deleteFile(const Matcher &matched)
{
	if (remove(matched.getPath().c_str()) < 0)
		throw "remove()";
}
