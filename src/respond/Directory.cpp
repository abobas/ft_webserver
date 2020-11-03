/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Directory.cpp                                      :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/08/28 20:37:10 by abobas        #+#    #+#                 */
/*   Updated: 2020/11/03 16:08:23 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Directory.hpp"

void Directory::resolveDirectoryRequest(int socket, const Matcher &matched, const Parser &parsed)
{
	Directory dir(socket, matched, parsed);

	if (dir.isAutoIndex(matched))
		dir.resolveDirListing(parsed);
	else
		dir.resolveDirIndex(matched);
}

Directory::Directory(int socket, const Matcher &matched, const Parser &parsed) : socket(socket)
{
	dir_path = matched.getPath();
	if (dir_path[dir_path.size() - 1] != '/')
		dir_path.append("/");
}

bool Directory::isAutoIndex(const Matcher &matched)
{
	return matched.getLocation()["autoindex"].bool_value();
}

void Directory::resolveDirListing(const Parser &parsed)
{
	std::string raw;
	DIR *dir;

	writeDirTitle(raw, parsed);
	dir = opendir(dir_path.c_str());
	if (!dir)
		throw "opendir()";
	for (struct dirent *dirent = readdir(dir); dirent != NULL; dirent = readdir(dir))
	{
		if (std::string(dirent->d_name) != "." && std::string(dirent->d_name) != "..")
			writeDirFile(raw, dirent->d_name, parsed);
	}
	closedir(dir);
	Responder::getResponder(socket).sendData(raw);
}

void Directory::resolveDirIndex(const Matcher &matched)
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

void Directory::writeDirTitle(std::string &raw, const Parser &parsed)
{
	raw.append("<p><h1>Index of ");
	raw.append(parsed.getPath());
	raw.append("</h1></p>");
}

void Directory::writeDirFile(std::string &raw, std::string &&file, const Parser &parsed)
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
