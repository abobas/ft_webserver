/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Cgi.cpp                                            :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/09/17 19:27:46 by abobas        #+#    #+#                 */
/*   Updated: 2020/10/31 22:35:05 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Cgi.hpp"

Cgi::Cgi(Data &data) : data(data)
{
	log = Log::getInstance();
	if (!checkRequest())
		return ;
	setPath();
	setEnvironment();
	createPipes();
	executeScript();
}

void Cgi::createPipes()
{
	if (pipe(parent_output) < 0)
		throw "pipe()";
	if (pipe(child_output) < 0)
	{
		close(parent_output[0]);
		close(parent_output[1]);
		throw "pipe()";
	}
}

void Cgi::executeScript()
{
	pid = fork();
	if (pid < 0)
	{
		closePipe(2);
		throw "fork()";
	}
	else if (pid == 0)
		childProcess();
	else
		parentProcess();
}


void Cgi::childProcess()
{
	std::vector<char *> argv;

	int ret_in = dup2(parent_output[0], STDIN_FILENO);
	int ret_out = dup2(child_output[1], STDOUT_FILENO);
	closePipe(3);
	if (ret_in < 0 || ret_out < 0)
	{
		closePipe(4);
		exit(1);
	}
	argv.push_back(const_cast<char *>(cgi_path.c_str()));
	argv.push_back(NULL);
	if (execve(cgi_path.c_str(), argv.data(), const_cast<char **>(env.data())) < 0)
		exit(1);
}

void Cgi::parentProcess()
{
	int exit;
	std::string cgi_output;
	
	parentWritePipe();
	exit = parentWait();
	if (exit != 0)
	{
		closePipe(1);
		throw "child process";
	}
	int ret = readOperation(child_output[0], cgi_output);
	// readOperation closes fd
	//closePipe(1);
	if (ret < 0)
		throw "read()";
	data.response.sendCgi(cgi_output);
}

void Cgi::parentWritePipe()
{
	std::string cgi_input;
	
	if (post)
		cgi_input = data.request.getBody();
	else
	{
		int fd = open(data.path.c_str(), O_RDONLY);
		if (fd < 0)
		{
			closePipe(2);
			kill(pid, SIGKILL);
			throw "open()";
		}
		if (readOperation(fd, cgi_input) < 0)
		{
			closePipe(2);
			kill(pid, SIGKILL);
			throw "read()";
		}
	}
	if (write(parent_output[1], cgi_input.c_str(), cgi_input.size()) < 0)
	{
		closePipe(2);
		kill(pid, SIGKILL);
		throw "write()";
	}
	closePipe(0);
}

int Cgi::parentWait()
{
	int status;
	
	while (true)
	{
		wait(&status);
		if (WIFEXITED(status))
			return WEXITSTATUS(status);
		if (WIFSIGNALED(status))
			return WTERMSIG(status);
	}
}

void Cgi::closePipe(int mode)
{
	if (mode == 0 || mode == 2 || mode == 4)
	{
		if (close(parent_output[0]) < 0)
			log->logError("close(parent_output[0])");
	}
	if (mode == 0 || mode == 2 || mode == 3)
	{
		if (close(parent_output[1]) < 0)
			log->logError("close(parent_output[1])");
	}
	if (mode == 1 || mode == 2 || mode == 3)
	{
		if (close(child_output[0]) < 0)
			log->logError("close(child_output[0])");
	}
	if (mode == 0 || mode == 2 || mode == 4)
	{
		if (close(child_output[1]) < 0)
			log->logError("close(child_output[1])");
	}
}

bool Cgi::checkRequest()
{
	if (data.method == "GET")
	{
		if (stat(data.path.c_str(), &file) < 0)
		{
			data.response.sendNotFound();
			return false;
		}
	}
	if (data.method == "POST")
		post = true;
	return true;	
}

void Cgi::setPath()
{
	for (auto file : data.config["http"]["cgi"]["files"].object_items())
	{
		std::string format = file.first;
		if (data.path.substr(data.path.size() - format.size()) == format)
		{
			Json::object obj = file.second.object_items();
			cgi_path = obj["path"].string_value();
			return;
		}
	}
}

void Cgi::setEnvironment()
{
	setConfigEnv();
	setMethodEnv();
	setUriEnv();
	setQueryEnv();
	setLengthEnv();
	setServerNameEnv();
	setServerPortEnv();
	setPathEnv();
	env.push_back(NULL);
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

void Cgi::setMethodEnv()
{
	std::string method("REQUEST_METHOD=");
	method += data.request.getMethod();
	memory.push_back(std::move(method));
	env.push_back(memory.back().c_str());
}
	
void Cgi::setUriEnv()
{
	std::string uri("REQUEST_URI=");
	uri += data.request.getPath();
	memory.push_back(std::move(uri));
	env.push_back(memory.back().c_str());
}
	
void Cgi::setQueryEnv()
{
	if (data.request.getQueryString().empty())
		return;
	std::string query("QUERY_STRING=");
	query += data.request.getQueryString();
	memory.push_back(std::move(query));
	env.push_back(memory.back().c_str());
}
	
void Cgi::setLengthEnv()
{
	std::string length("CONTENT_LENGTH=");
	if (post)
		length += std::to_string(data.request.getBody().size());
	else
		length += std::to_string(file.st_size);
	memory.push_back(std::move(length));
	env.push_back(memory.back().c_str());	
}

void Cgi::setServerNameEnv()
{
	std::string name("SERVER_NAME=");
	name += data.server["name"].string_value();
	memory.push_back(std::move(name));
	env.push_back(memory.back().c_str());
}
	
void Cgi::setServerPortEnv()
{
	std::string port("SERVER_PORT=");
	port += std::to_string(static_cast<int>(data.server["listen"].number_value()));
	memory.push_back(std::move(port));
	env.push_back(memory.back().c_str());
}

void Cgi::setPathEnv()
{
	std::string script("SCRIPT_NAME=");
	script += cgi_path;
	memory.push_back(std::move(script));
	env.push_back(memory.back().c_str());
}

int Cgi::readOperation(int fd, std::string &buffer)
{
	char buf[1025];
	
	while (1)
	{
		int ret = read(fd, buf, 1024);
		if (ret < 0)
		{
			close(fd);
			return -1;
		}
		buf[ret] = '\0';
		buffer += buf;
		if (ret < 1024)
			break;
	}
	close(fd);
	return 0;
}