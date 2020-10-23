/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Cgi.cpp                                            :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/09/17 19:27:46 by abobas        #+#    #+#                 */
/*   Updated: 2020/10/19 22:50:46 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Cgi.hpp"

Cgi::Cgi(Data &data) : data(data) 
{
	redirectOutput();
	setEnvironment();
	executeScript();
}

void Cgi::setEnvironment()
{
	for (auto object : data.config["http"]["cgi"]["cgi_params"].object_items())
	{
		std::string insert;
		insert += object.first;
		insert += '=';
		insert += object.second.string_value();
		memory.push_back(std::move(insert));
		env.push_back(memory.back().c_str());
	}
	env.push_back(NULL);
}

void Cgi::redirectOutput()
{
	if (pipe(pipe_fd) < 0)
	{
		data.response.sendInternalError();
		return ;
	}
	if ((restore_fd = dup(1)) < 0)
	{
		data.response.sendInternalError();
		return ;
	}
	if ((dup2(pipe_fd[1], 1)) < 0)
	{
		data.response.sendInternalError();
		return ;
	}
}

void Cgi::resetOutput()
{
	if ((dup2(restore_fd, 1)) < 0)
	{
		data.response.sendInternalError();
		return ;
	}
}

void Cgi::executeScript()
{
	pid_t pid;

	pid = fork();
	if (pid < 0)
	{
		data.response.sendInternalError();
		return ;
	}
	else if (pid == 0)
		childProcess();
	else
		mainProcess();
}

void Cgi::childProcess()
{
	std::vector<char *> argv;
	argv.push_back(const_cast<char *>(data.path.c_str()));
	argv.push_back(NULL);

	if (execve(data.path.c_str(), argv.data(), const_cast<char **>(env.data())) < 0)
	{
		data.response.sendInternalError();
		return ;
	}
}

std::string Cgi::convertOutput()
{
	char buf[257];
	std::string buffer;

	while (1)
	{
		int ret = read(pipe_fd[0], buf, 256);
		buf[ret] = '\0';
		buffer += buf;
		if (ret < 256)
			break;
	}
	return buffer;
}

void Cgi::mainProcess()
{
	int status;

	while (1)
	{
		wait(&status);
		if (WIFEXITED(status) || WIFSIGNALED(status))
		{
			data.response.addHeader("content-type", "text/html");
			data.response.sendData(convertOutput());
			resetOutput();
			return;
		}
	}
}
