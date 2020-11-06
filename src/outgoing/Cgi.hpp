/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Cgi.hpp                                            :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/09/17 19:28:58 by abobas        #+#    #+#                 */
/*   Updated: 2020/11/05 17:12:01 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Responder.hpp"
#include "../logger/Log.hpp"
#include "../incoming/Matcher.hpp"
#include "../incoming/Parser.hpp"
#include <vector>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>

class Cgi
{
public:
	/**
	* @brief Resolves CGI request and sends output to client.
	*/
	static void resolveCgiRequest(int socket, Matcher &matched, Parser &parsed);

private:
	static Log *log;
	Matcher &matched;
	Parser &parsed;
	Responder respond;
	std::vector<const char *> env;
	std::vector<std::string> memory;
	std::string cgi_path;
	struct stat file;
	int parent_output[2];
	int child_output[2];
	pid_t pid;
	int socket;
	bool post = false;
	bool chunked = false;

	Cgi(int socket, Matcher &matched, Parser &parsed);
	void executeScript();
	void parentProcess();
	void parentWritePipe();
	void childProcess();
	bool waitCheck();
	void createPipes();
	void closePipe(int mode);
	int readOperation(int fd, std::string &buffer);
	void writeOperation(int fd, const char *buffer, int size);
	bool checkRequest();
	void setPath();
	void setEnvironment();
	void setConfigEnv();
	void setMethodEnv();
	void setUriEnv();
	void setQueryEnv();
	void setHeadersEnv();
	void setContentLengthEnv();
	void setServerNameEnv();
	void setServerPortEnv();
	void setScriptNameEnv();
	void setPathInfoEnv();
};