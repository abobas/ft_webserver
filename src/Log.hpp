/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Log.hpp                                            :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/10/28 16:29:48 by abobas        #+#    #+#                 */
/*   Updated: 2020/10/29 01:10:30 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <fstream>
#include <iostream>
#include <string>
#include <string.h>
#include <sys/time.h>

class Log
{
public:
	static Log *getInstance();
	void logEntry(std::string message);
	void logEntry(std::string message, int number);
	void logBlock(std::string message);
	void logError(const char *error);

private:
	static Log *instance;
	std::ofstream file;

	Log(std::string path);
	~Log();
	std::string getTime();
};
