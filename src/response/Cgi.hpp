/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Cgi.hpp                                            :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/09/17 19:28:58 by abobas        #+#    #+#                 */
/*   Updated: 2020/10/27 22:52:18 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Data.hpp"
#include <vector>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>

/**
* @brief Executes CGI file and sends output to client.
*/
class Cgi
{
public:
	Cgi(Data &data);

private:
	Data data;
	std::vector<const char *> env;
	std::vector<std::string> memory;
	std::string tmp_path;

	void executeScript();
	void childProcess();
	void parentProcess();
	void setTmp();
	void deleteTmp();
	void setEnvironment();
	void setConfigEnv();
	void setServerEnv();
	void setRequestEnv();
	void setPathEnv();
};