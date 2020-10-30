/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Log.cpp                                            :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/10/28 16:29:45 by abobas        #+#    #+#                 */
/*   Updated: 2020/10/30 01:46:19 by abobas        ########   odam.nl         */
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

void Log::logEntry(std::string message)
{
	file << getTime() << message << std::endl;
}

void Log::logEntry(std::string message, int number)
{
	file << getTime() << message + " " << number << std::endl;
}

void Log::logBlock(std::string message)
{
	file << "-------begin-of-message------" << std::endl;
	file << message << std::endl;
	file << "--------end-of-message-------" << std::endl;
}

void Log::logError(const char *error)
{
	file << "Error: " << getTime() << error << ": " << strerror(errno) << std::endl;
}

std::string Log::getTime()
{
	struct timeval time;
	struct tm *tmp;
	char string[128];

	if (gettimeofday(&time, NULL))
		return "";
	tmp = localtime(&time.tv_sec);
	strftime(string, 128, "%X -", tmp);
	return std::string(string) + " ";
}