/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Cgi.hpp                                            :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/09/17 19:28:58 by abobas        #+#    #+#                 */
/*   Updated: 2020/10/19 22:49:18 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Data.hpp"
#include <vector>
#include <sys/types.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

class Cgi
{
public:
	Cgi(Data &data);
	
private:
	Data data;
	std::vector<const char *> env;
	std::vector<std::string> memory;
	int restore_fd;
	int pipe_fd[2];

	std::string convertOutput();
	void redirectOutput();
	void resetOutput();
	void executeScript();
	void childProcess();
	void mainProcess();
	void setEnvironment();
};