/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Directory.cpp                                      :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/08/28 20:37:10 by abobas        #+#    #+#                 */
/*   Updated: 2020/10/26 21:28:48 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Directory.hpp"
#include <iostream>

Directory::Directory(Data &data) : data(data)
{
	std::cout << "entered directory" << std::endl;
	if (data.path[data.path.size() - 1] != '/')
		this->data.path.append("/");
	if (data.location["autoindex"].bool_value())
		handleDirListing();
	else
		handleDirIndex();
	std::cout << "finished directory" << std::endl;
}

void Directory::handleDirListing()
{
	std::string raw;
	DIR *dir;

	writeDirTitle(raw);
	dir = opendir(data.path.c_str());
	if (!dir)
	{
		data.response.sendInternalError();
		return;
	}
	for (struct dirent *dirent = readdir(dir); dirent != NULL; dirent = readdir(dir))
	{
		if (std::string(dirent->d_name) != "." && std::string(dirent->d_name) != "..")
			writeDirFile(raw, dirent->d_name);
	}
	closedir(dir);
	data.response.sendData(raw);
}

void Directory::writeDirTitle(std::string &raw)
{
	raw.append("<p><h1>Index of ");
	raw.append(data.request.getPath());
	raw.append("</h1></p>");
}

void Directory::writeDirFile(std::string &raw, std::string &&file)
{
	raw.append("<a href=\"");
	raw.append(data.request.getPath());
	if (raw[raw.size() - 1] != '/')
		raw.append("/");
	raw.append(file);
	raw.append("\">");
	raw.append(file);
	raw.append("</a><br>");
}

void Directory::handleDirIndex()
{
	DIR *dir;
	std::string file = data.location["index"].string_value();
	
	dir = opendir(data.path.c_str());
	if (!dir)
	{
		perror("opendir()");
		data.response.sendInternalError();
		return ;
	}
	for (struct dirent *dirent = readdir(dir); dirent != NULL; dirent = readdir(dir))
	{
		if (file == dirent->d_name)
		{
			closedir(dir);
			this->data.path.append(file);
			data.response.sendFile(data.path);
			return;
		}
	}
	closedir(dir);
	data.response.sendNotFound();
}
