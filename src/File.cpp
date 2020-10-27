/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   File.cpp                                           :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/08/27 22:06:27 by abobas        #+#    #+#                 */
/*   Updated: 2020/10/27 22:45:30 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "File.hpp"
#include <iostream>

File::File(Data &data) : data(data)
{
	if (!setStat())
		return;
	if (S_ISDIR(file.st_mode))
	{
		std::cout << "entering directory" << std::endl;
		Directory directory(data);
		std::cout << "entering directory" << std::endl;
	}
	else if (isCgi())
	{
		std::cout << "entering cgi" << std::endl;
		Cgi cgi(data);
		std::cout << "entering cgi" << std::endl;
	}
	else if (S_ISREG(file.st_mode))
	{
		if (data.method == "HEAD")
			data.response.sendFileHeaders(data.path);
		else
			data.response.sendFile(data.path);
	}
	else
	{
		std::cout << "client request not implemented" << std::endl;
		data.response.sendNotImplemented();
	}
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