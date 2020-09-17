/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   CgiHandler.cpp                                     :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/09/17 19:27:46 by abobas        #+#    #+#                 */
/*   Updated: 2020/09/17 21:59:14 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "CgiHandler.hpp"
#include "Socket.hpp"
#include "Json.hpp"
#include "HttpParser.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include <memory>
#include <sys/types.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

//debugging
#include <iostream>

CgiHandler::CgiHandler(HttpRequest &request, HttpResponse &response, Json::Json &config, std::string &path)
    : request(request), response(response), config(config), path(path) {}

CgiHandler::~CgiHandler() {}

void CgiHandler::resolve()
{
    this->redirectOutput();
    this->setEnvironment();
    this->executeScript();
}

void CgiHandler::setEnvironment()
{
    std::string insert;
    
    for (auto object : this->config["http"]["cgi"]["cgi_params"].object_items())
    {
        insert += object.first;
        insert += '=';
        insert += object.second.string_value();
        env.push_back(strdup(insert.c_str()));
        insert.clear();
    }
    env.push_back(NULL);
}

void CgiHandler::redirectOutput()
{
    if (pipe(this->pipe_fd) < 0)
        throw "Error: pipe failed in CgiHandler::redirectOutput()";
    if ((this->restore_fd = dup(1)) < 0)
        throw "Error: dup failed in CgiHandler::redirectOutput()";
    if ((dup2(this->pipe_fd[1], 1)) < 0)
        throw "Error: dup2 failed in CgiHandler::redirectOutput()";
}

void CgiHandler::resetOutput()
{
    if ((dup2(this->restore_fd, 1)) < 0)
        throw "Error: dup2 failed in CgiHandler::resetOutput()";
}

void CgiHandler::executeScript()
{
    pid_t	pid;

	pid = fork();
	if (pid < 0)
		throw "Error: forking failed in CgiHandler::executeScript()";
	else if (pid == 0)
		this->child();
	else
		this->waiting();
}

void CgiHandler::child()
{
    std::vector<char *> argv;
    argv.push_back(strdup(this->path.c_str()));
    argv.push_back(NULL);
    close(this->pipe_fd[0]);

    if (execve(this->path.c_str(), argv.data(), this->env.data()) < 0)
        throw "Error: executing script failed in CgiHandler::child()";
}

void CgiHandler::freeShit()
{
    for (auto string : this->env)
        free(string);
}

std::string CgiHandler::convertOutput()
{
    char buf[257];
    std::string buffer;

    while (1)
    {
        int ret = read(this->pipe_fd[0], buf, 256);
        buf[ret] = '\0';
        buffer += buf;
        if (ret < 256)
            break;
    }
    return buffer;
}

void CgiHandler::waiting()
{
    int		status;

	while (1)
	{
		wait(&status);
		if (WIFEXITED(status) || WIFSIGNALED(status))
        {
            this->freeShit();
            this->response.sendData(this->convertOutput());
            this->resetOutput();
			return ;
        }
	}
}
