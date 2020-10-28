/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Logs.hpp                                           :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/10/28 16:29:48 by abobas        #+#    #+#                 */
/*   Updated: 2020/10/28 16:51:33 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Socket.hpp"
#include <fstream>
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
	void logError(const char *error);

private:
	std::ofstream file;
	std::string getTime();
};