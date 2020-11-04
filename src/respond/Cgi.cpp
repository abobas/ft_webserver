/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Cgi.cpp                                            :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/09/17 19:27:46 by abobas        #+#    #+#                 */
/*   Updated: 2020/11/04 21:01:11 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Cgi.hpp"

#define IO_SIZE 1048576

Log *Cgi::log = Log::getInstance();

void Cgi::resolveCgiRequest(int socket, Matcher &matched, Parser &parsed)
{
	Cgi(socket, matched, parsed);
}

Cgi::Cgi(int socket, Matcher &matched, Parser &parsed)
	: matched(matched), parsed(parsed), respond(socket, parsed), socket(socket)
{
	if (!checkRequest())
		return;
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
	log->logEntry("entered parent process");
	parentWritePipe();
	log->logEntry("wrote input to child");

	respond.sendChunkHeader();
	char buf[IO_SIZE + 1];
	bool ready = false;
	while (true)
	{
		int ret = read(child_output[0], buf, IO_SIZE);
		if (ret < 0)
		{
			closePipe(1);
			throw "read()";
		}
		if (!ready)
			ready = waitCheck();
		//log->logEntry("status: ", status);
		//log->logEntry("output bytes read", ret);
		if (ready == true && ret == 0)
			break;
		if (ret > 0)
		{
			buf[ret] = '\0';
			log->logBlock(buf);
			respond.sendChunk(buf, ret + 1);
		}
	}
	closePipe(1);
	respond.sendChunkEnd();
}

bool Cgi::waitCheck()
{
	int status;

	waitpid(pid, &status, WNOHANG);
	if (WIFEXITED(status))
	{
		log->logEntry("child process exit status", WEXITSTATUS(status));
		return true;
	}
	if (WIFSIGNALED(status))
	{
		log->logEntry("child process signalled status", WTERMSIG(status));
		return true;
	}
	return false;
}

void Cgi::parentWritePipe()
{
	std::string cgi_input;

	if (post)
		cgi_input = std::move(parsed.getBody());
	else
	{
		int fd = open(matched.getPath().c_str(), O_RDONLY);
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
	log->logEntry("writing to child now");
	writeOperation(parent_output[1], cgi_input.c_str(), cgi_input.size());
	closePipe(0);
}

void Cgi::writeOperation(int fd, const char *buffer, int size)
{
	int write_size;

	while (size > 0)
	{
		write_size = (size >= IO_SIZE) ? IO_SIZE : size;
		int ret = write(fd, buffer, write_size);
		if (ret < 0)
		{
			closePipe(2);
			kill(pid, SIGKILL);
			throw "write()";
		}
		//log->logEntry("bytes written to child", ret);
		size -= write_size;
		buffer += write_size;
	}
}

int Cgi::readOperation(int fd, std::string &buffer)
{
	char buf[IO_SIZE + 1];

	while (1)
	{
		int ret = read(fd, buf, IO_SIZE);
		if (ret < 0)
		{
			close(fd);
			return -1;
		}
		buf[ret] = '\0';
		buffer += buf;
		if (ret < IO_SIZE)
			break;
	}
	close(fd);
	return 0;
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
	if (parsed.getMethod() == "GET")
	{
		if (stat(matched.getPath().c_str(), &file) < 0)
		{
			respond.sendNotFound();
			return false;
		}
	}
	if (parsed.getMethod() == "POST")
	{
		post = true;
		if (parsed.getHeader("content-length").empty())
			chunked = true;
	}
	return true;
}

void Cgi::setPath()
{
	std::string extension;
	size_t size;

	for (auto file : matched.getConfig()["http"]["cgi"]["files"].object_items())
	{
		extension = file.first;
		size = matched.getPath().size();
		if (matched.getPath().substr(size - extension.size()) == extension)
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
	setServerNameEnv();
	setServerPortEnv();
	setScriptNameEnv();
	setPathInfoEnv();
	setHeadersEnv();
	setContentLengthEnv();
	env.push_back(NULL);
}

void Cgi::setConfigEnv()
{
	for (auto object : matched.getConfig()["http"]["cgi"]["cgi_params"].object_items())
	{
		std::string insert;
		insert += object.first;
		insert += '=';
		insert += object.second.string_value();
		memory.push_back(std::move(insert));
		env.push_back(memory.back().c_str());
	}
}

void Cgi::setHeadersEnv()
{
	for (auto header : parsed.getHeaders())
	{
		std::string insert;
		std::string copy(header.first);
		for (auto &c : copy)
			c = toupper(c);
		insert.append(copy + "=");
		insert.append(header.second);
		memory.push_back(std::move(insert));
		env.push_back(memory.back().c_str());
	}
}

void Cgi::setContentLengthEnv()
{
	if (post && !chunked)
		return;
	std::string length("CONTENT_LENGTH=");
	if (chunked)
		length += std::to_string(parsed.getBodySize());
	else
		length += std::to_string(file.st_size);
	memory.push_back(std::move(length));
	env.push_back(memory.back().c_str());
}

void Cgi::setMethodEnv()
{
	std::string method("REQUEST_METHOD=");
	method += parsed.getMethod();
	memory.push_back(std::move(method));
	env.push_back(memory.back().c_str());
}

void Cgi::setUriEnv()
{
	std::string uri("REQUEST_URI=");
	uri += parsed.getPath();
	memory.push_back(std::move(uri));
	env.push_back(memory.back().c_str());
}

void Cgi::setQueryEnv()
{
	if (parsed.getQuery().empty())
		return;
	std::string query("QUERY_STRING=");
	query += parsed.getQuery();
	memory.push_back(std::move(query));
	env.push_back(memory.back().c_str());
}

void Cgi::setServerNameEnv()
{
	std::string name("SERVER_NAME=");
	name += matched.getServer()["name"].string_value();
	memory.push_back(std::move(name));
	env.push_back(memory.back().c_str());
}

void Cgi::setServerPortEnv()
{
	std::string port("SERVER_PORT=");
	port += std::to_string(static_cast<int>(matched.getServer()["listen"].number_value()));
	memory.push_back(std::move(port));
	env.push_back(memory.back().c_str());
}

void Cgi::setScriptNameEnv()
{
	std::string script("SCRIPT_NAME=");
	script += cgi_path;
	memory.push_back(std::move(script));
	env.push_back(memory.back().c_str());
}

void Cgi::setPathInfoEnv()
{
	std::string path_info("PATH_INFO=");
	memory.push_back(std::move(path_info));
	env.push_back(memory.back().c_str());
}
