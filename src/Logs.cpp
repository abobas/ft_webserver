/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Logs.cpp                                           :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/10/28 16:29:45 by abobas        #+#    #+#                 */
/*   Updated: 2020/10/28 17:22:31 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Logs.hpp"

Logs::Logs()
{
}

void Logs::createLogFile(std::string path)
{
	file.open(path.c_str(), std::ios::out | std::ios::trunc);
	if (!file.is_open())
		throw "is_open()";
}

void Logs::logSocket(std::string message, Socket socket)
{
	file << getTime() << message + " " << socket.getSocket() << std::endl;
}

void Logs::logSocket(std::string message, int socket)
{
	file << getTime() << message + " " << socket << std::endl;
}

void Logs::logEntry(std::string message)
{
	file << getTime() << message << std::endl;
}

void Logs::logError(const char *error)
{
	file << getTime() << error << ": " << strerror(errno) << std::endl;
}

std::string Logs::getTime()
{
	struct timeval time;
	struct tm *tmp;
	char string[128];

	if (gettimeofday(&time, NULL))
		return "";
	tmp = localtime(&time.tv_sec);
	strftime(string, 128, "%x %X", tmp);
	return std::string(string) + " ";
}