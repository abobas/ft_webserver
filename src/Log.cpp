/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Log.cpp                                            :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/10/28 16:29:45 by abobas        #+#    #+#                 */
/*   Updated: 2020/10/29 00:19:54 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Log.hpp"

Log *Log::instance = NULL;

Log *Log::getInstance()
{
	if (!instance)
		instance = new Log("./logs/log.txt");
	return instance;
}

Log::Log(std::string path)
{
	file.open(path.c_str(), std::ios::out | std::ios::trunc);
	if (!file.is_open())
	{
		std::cerr << "Error: could not create log.txt" << std::endl;
		exit(1);
	}
}

Log::~Log()
{
	delete instance;
}

void Log::logSocket(std::string message, int socket)
{
	file << getTime() << message + " " << socket << std::endl;
}

void Log::logEntry(std::string message)
{
	file << getTime() << message << std::endl;
}

void Log::logEntry(std::string message, int number)
{
	file << getTime() << message + " " << number << std::endl;
}

void Log::logPlain(std::string message, int mode = 0)
{
	if (mode == 1)
		file << "-----------------------------------" << std::endl;
	file << message << std::endl;
	if (mode == 1)
		file << "-----------------------------------" << std::endl;
}

void Log::logError(const char *error)
{
	file << getTime() << error << ": " << strerror(errno) << std::endl;
}

std::string Log::getTime()
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