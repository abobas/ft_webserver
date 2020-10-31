/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Cgi.hpp                                            :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/09/17 19:28:58 by abobas        #+#    #+#                 */
/*   Updated: 2020/10/31 20:56:04 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Data.hpp"
#include "../Log.hpp"
#include <vector>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>

/**
* @brief Executes CGI file and sends output to client.
*/
class Cgi
{
public:
	Cgi(Data &data);

private:
	Data data;
	Log *log;
	std::vector<const char *> env;
	std::vector<std::string> memory;
	std::string cgi_path;
	bool post = false;
	struct stat file;
	int parent_output[2];
	int child_output[2];
	pid_t pid;
	
	void createPipes();
	int readOperation(int fd, std::string &buffer);

	void executeScript();
	void parentProcess();
	void parentWritePipe();
	void closePipe(int mode);
	void childClosePipe(int mode);
	int parentWait();
	void childProcess();
	void setTmp();
	void deleteTmp();
	bool checkRequest();
	void setPath();
	void setEnvironment();
	void setServerNameEnv();
	void setServerPortEnv();
	void setConfigEnv();
	void setMethodEnv();
	void setUriEnv();
	void setQueryEnv();
	void setLengthEnv();	
	void setPathEnv();
};