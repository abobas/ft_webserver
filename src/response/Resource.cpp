/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Resource.cpp                                       :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/08/27 22:06:27 by abobas        #+#    #+#                 */
/*   Updated: 2020/10/28 21:03:35 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Resource.hpp"

Resource::Resource(Data &data) : data(data)
{
	log = Log::getInstance();
	if (isUpload())
		handleUpload();
	else if (setStat())
	{
		if (isDirectory())
			handleDirectory();
		else if (isCgi())
			handleCgi();
		else if (isRegular())
			handleFile();
		else
			this->data.response.sendNotImplemented();
	}
}

void Resource::handleFile()
{
	log->logEntry("handling file request");
	if (data.method == "HEAD")
		data.response.sendFileHeaders(data.path);
	else
		data.response.sendFile(data.path);
}

void Resource::handleCgi()
{
	log->logEntry("handling CGI request");
	Cgi cgi(data);
}

void Resource::handleUpload()
{
	log->logEntry("handling upload request");
	Upload upload(data);
}

void Resource::handleDirectory()
{
	log->logEntry("handling directory request");
	Directory directory(data);
}

bool Resource::setStat()
{
	if (stat(data.path.c_str(), &file) < 0)
	{
		data.response.sendNotFound();
		return false;
	}
	return true;
}

bool Resource::isDirectory()
{
	return S_ISDIR(file.st_mode);
}

bool Resource::isRegular()
{
	return S_ISREG(file.st_mode);
}

bool Resource::isCgi()
{
	for (auto file : data.config["http"]["cgi"]["files"].array_items())
	{
		std::string format = file.string_value();
		if (data.path.substr(data.path.size() - format.size()) == format)
			return true;
	}
	return false;
}

bool Resource::isUpload()
{
	return data.method == "PUT" || data.method == "POST";
}