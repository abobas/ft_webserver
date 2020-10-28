/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Logs.hpp                                           :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/10/28 16:29:48 by abobas        #+#    #+#                 */
/*   Updated: 2020/10/28 17:22:04 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Socket.hpp"
#include <fstream>
#include <iostream>
#include <string>
#include <string.h>
#include <sys/time.h>

class Logs
{
public:
	Logs();
	void createLogFile(std::string path);
	void logSocket(std::string message, Socket socket);
	void logSocket(std::string message, int socket);
	void logEntry(std::string message);
	void logError(const char *error);

private:
	std::ofstream file;
	std::string getTime();
};