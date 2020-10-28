/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   File.cpp                                           :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/08/27 22:06:27 by abobas        #+#    #+#                 */
/*   Updated: 2020/10/28 01:40:48 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "File.hpp"

File::File(Data &data) : data(data)
{
	if (!setStat())
		return;
	if (S_ISDIR(file.st_mode))
		Directory directory(data);
	else if (isCgi())
		Cgi cgi(data);
	else if (S_ISREG(file.st_mode))
		fileHandler();
	else
		data.response.sendNotImplemented();
}

void File::fileHandler()
{
	if (data.method == "HEAD")
		data.response.sendFileHeaders(data.path);
	else
		data.response.sendFile(data.path);
}

bool File::setStat()
{
	if (stat(data.path.c_str(), &file) < 0)
	{
		perror("stat()");
		data.response.sendNotFound();
		return false;
	}
	return true;
}

bool File::isCgi()
{
	for (auto file : data.config["http"]["cgi"]["files"].array_items())
	{
		std::string format = file.string_value();
		if (data.path.substr(data.path.size() - format.size()) == format)
			return true;
	}
	return false;
}