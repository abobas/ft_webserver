/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Directory.cpp                                      :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/08/28 20:37:10 by abobas        #+#    #+#                 */
/*   Updated: 2020/11/04 00:44:09 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Directory.hpp"

void Directory::resolveDirectoryRequest(int socket, const Matcher &matched, const Parser &parsed)
{
	Directory dir(socket, matched, parsed);
}

Directory::Directory(int socket, const Matcher &matched, const Parser &parsed)
	: matched(matched), parsed(parsed), socket(socket)
{
	setPath();
	if (isAutoIndex())
		resolveDirListing();
	else
		resolveDirIndex();
}

void Directory::setPath()
{
	dir_path = matched.getPath();
	if (dir_path[dir_path.size() - 1] != '/')
		dir_path.append("/");
}

bool Directory::isAutoIndex()
{
	return matched.getLocation()["autoindex"].bool_value();
}

void Directory::resolveDirListing()
{
	std::string raw;
	DIR *dir;

	writeDirTitle(raw);
	dir = opendir(dir_path.c_str());
	if (!dir)
		throw "opendir()";
	for (struct dirent *dirent = readdir(dir); dirent != NULL; dirent = readdir(dir))
	{
		if (std::string(dirent->d_name) != "." && std::string(dirent->d_name) != "..")
			writeDirFile(raw, dirent->d_name);
	}
	closedir(dir);
	Responder::getResponder(socket).sendData(raw);
}

void Directory::resolveDirIndex()
{
	DIR *dir;
	std::string file;

	file = matched.getLocation()["index"].string_value();
	dir = opendir(dir_path.c_str());
	if (!dir)
		throw "opendir()";
	for (struct dirent *dirent = readdir(dir); dirent != NULL; dirent = readdir(dir))
	{
		if (file == dirent->d_name)
		{
			closedir(dir);
			dir_path.append(file);
			Responder::getResponder(socket).sendFile(dir_path);
			return;
		}
	}
	closedir(dir);
	Responder::getResponder(socket).sendNotFound();
}

void Directory::writeDirTitle(std::string &raw)
{
	raw.append("<p><h1>Index of ");
	raw.append(parsed.getPath());
	raw.append("</h1></p>");
}

void Directory::writeDirFile(std::string &raw, std::string &&file)
{
	raw.append("<a href=\"");
	raw.append(parsed.getPath());
	if (raw[raw.size() - 1] != '/')
		raw.append("/");
	raw.append(file);
	raw.append("\">");
	raw.append(file);
	raw.append("</a><br>");
}
