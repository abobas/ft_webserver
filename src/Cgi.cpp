/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Cgi.cpp                                            :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/09/17 19:27:46 by abobas        #+#    #+#                 */
/*   Updated: 2020/10/28 16:28:33 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Cgi.hpp"

//debugging
#include <iostream>

Cgi::Cgi(Data &data) : data(data)
{
	try
	{
		setEnvironment();
		setTmp();
		executeScript();
		deleteTmp();
	}
	catch (const char *e)
	{
		perror(e);
		data.response.sendInternalError();
	}
}

void Cgi::executeScript()
{
	pid_t pid;

	pid = fork();
	if (pid < 0)
		throw "fork()";
	else if (pid == 0)
		childProcess();
	else
		parentProcess();
}

void Cgi::childProcess()
{
	std::vector<char *> argv;
	int fd;

	argv.push_back(const_cast<char *>(data.path.c_str()));
	argv.push_back(NULL);
	fd = open(tmp_path.c_str(), O_WRONLY | O_CREAT | O_TRUNC);
	if (fd < 0)
	{
		perror("child: open()");
		exit(1);
	}
	if (dup2(fd, STDOUT_FILENO) < 0)
	{
		perror("child: dup2()");
		exit(1);
	}
	close(fd);
	if (execve(data.path.c_str(), argv.data(), const_cast<char **>(env.data())) < 0)
	{
		perror("child: execve()");
		exit(1);
	}
}

void Cgi::parentProcess()
{
	int status;
	int exit;

	while (1)
	{
		wait(&status);
		if (WIFEXITED(status))
		{
			exit = WEXITSTATUS(status);
			break;
		}
		if (WIFSIGNALED(status))
		{
			exit = WTERMSIG(status);
			break;
		}
	}
	if (exit == 0)
	{
		std::cout << "written to " << tmp_path << std::endl;
		data.response.addHeader("content-type", "text/html");
		data.response.sendFile(tmp_path);
	}
	else
		data.response.sendInternalError();
}

void Cgi::setTmp()
{
	tmp_path = "./tmp/webserv-cgi-output-" + std::to_string(rand());
}

void Cgi::deleteTmp()
{
	if (remove(tmp_path.c_str()) < 0)
		perror("remove()");
	else
		std::cout << "deleted " << tmp_path << std::endl;
}

void Cgi::setEnvironment()
{
	setConfigEnv();
	setRequestEnv();
	setServerEnv();
	setPathEnv();
	env.push_back(NULL);
	// std::cout << "--------ENV---------" << std::endl;
	// for (auto n : env)
	// 	std::cout << n << std::endl;
	// std::cout << "--------------------" << std::endl;
}

void Cgi::setConfigEnv()
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
}

void Cgi::setRequestEnv()
{
	std::string method("REQUEST_METHOD=");
	method += data.request.getMethod();
	memory.push_back(std::move(method));
	env.push_back(memory.back().c_str());

	std::string uri("REQUEST_URI=");
	uri += data.request.getPath();
	memory.push_back(std::move(uri));
	env.push_back(memory.back().c_str());

	std::string query("QUERY_STRING=");
	if (data.request.getQueryString().empty())
		return;
	query += data.request.getQueryString();
	memory.push_back(std::move(query));
	env.push_back(memory.back().c_str());
}

void Cgi::setServerEnv()
{
	std::string name("SERVER_NAME=");
	name += data.server["name"].string_value();
	memory.push_back(std::move(name));
	env.push_back(memory.back().c_str());

	std::string port("SERVER_PORT=");
	port += std::to_string(static_cast<int>(data.server["listen"].number_value()));
	memory.push_back(std::move(port));
	env.push_back(memory.back().c_str());
}

void Cgi::setPathEnv()
{
	std::string script("SCRIPT_NAME=");
	size_t pos = data.path.find_last_of('/');
	if (pos == std::string::npos)
		pos = -1;
	script += data.path.substr(pos + 1);
	memory.push_back(std::move(script));
	env.push_back(memory.back().c_str());
}
