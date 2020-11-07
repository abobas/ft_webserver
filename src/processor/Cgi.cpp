/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Cgi.cpp                                            :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/09/17 19:27:46 by abobas        #+#    #+#                 */
/*   Updated: 2020/11/07 13:47:48 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Cgi.hpp"

#define NOT_FOUND 404
#define INTERNAL_ERROR 500
#define IO_SIZE 4096

Log *Cgi::log = Log::getInstance();
std::map<int, Cgi *> Cgi::cgis;

Cgi::Cgi(int socket, Parser &parsed, Matcher &matched)
	: parsed(parsed), matched(matched), socket(socket)
{
	initialized = false;
	headers_sent = false;
	child_ready = false;
	processed = false;
	resolved = false;
	post = false;
	chunked = false;
	error = 0;
}

Cgi *Cgi::getInstance(int socket, Parser &parsed, Matcher &matched)
{
	if (!cgis[socket])
	{
		cgis[socket] = new Cgi(socket, parsed, matched);
		log->logEntry("created CGI", socket);
	}
	return cgis[socket];
}

void Cgi::deleteInstance(int socket)
{
	if (cgis[socket])
	{
		delete cgis[socket];
		cgis[socket] = NULL;
		log->logEntry("deleted CGI", socket);
	}
}

void Cgi::resolveCgiRequest()
{
	char buf[IO_SIZE + 1];
	Responder respond(socket, parsed);
	int bytes_read;

	if (!headers_sent)
	{
		respond.sendChunkHeader();
		headers_sent = true;
	}
	if (!child_ready)
		child_ready = checkWait();
	readPipe(buf, bytes_read);
	respond.sendChunk(buf, bytes_read + 1);
	if (child_ready && bytes_read < IO_SIZE)
	{
		log->logEntry("child output completely read");
		closePipe(1);
		resolved = true;
		respond.sendChunkEnd();
		return;
	}
}

void Cgi::readPipe(char *buf, int &bytes_read)
{
	bytes_read = read(child_output[0], buf, IO_SIZE);
	if (bytes_read < 0)
	{
		closePipe(1);
		resolved = true;
		throw "read()";
	}
	buf[bytes_read] = '\0';
}

bool Cgi::checkWait()
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

void Cgi::processCgiRequest()
{
	if (!initialized)
	{
		if (!initializeCgi())
		{
			processed = true;
			return;
		}
		initialized = true;
	}
	processCgi();
}

void Cgi::processCgi()
{
	if (!post)
	{
		readFileWritePipe();
		processed = true;
	}
	/// post logic toevoegen
}

void Cgi::readFileWritePipe()
{
	char buf[IO_SIZE + 1];
	int fd;
	int bytes_read;

	if (!openFile(fd))
		return;
	while (true)
	{
		if (!readFile(fd, buf, bytes_read))
			return;
		if (!writePipe(buf, bytes_read))
		{
			close(fd);
			return;
		}
		if (bytes_read < IO_SIZE)
		{
			close(fd);
			closePipe(0);
			return;
		}
	}
}

bool Cgi::writePipe(char *buf, int bytes_read)
{
	int ret;

	ret = write(parent_output[1], buf, bytes_read + 1);
	if (ret < 0)
	{
		closePipe(2);
		kill(pid, SIGKILL);
		log->logError("write()");
		error = INTERNAL_ERROR;
		return false;
	}
	log->logEntry("bytes written", ret);
	log->logBlock(buf);
	return true;
}

bool Cgi::readFile(int fd, char *buf, int &bytes_read)
{
	bytes_read = read(fd, buf, IO_SIZE);
	if (bytes_read < 0)
	{
		close(fd);
		closePipe(2);
		kill(pid, SIGKILL);
		log->logError("read()");
		error = INTERNAL_ERROR;
		return false;
	}
	buf[bytes_read] = '\0';
	log->logEntry("bytes read", bytes_read);
	log->logBlock(buf);
	return true;
}

bool Cgi::openFile(int &fd)
{
	fd = open(matched.getPath().c_str(), O_RDONLY);
	if (fd < 0)
	{
		closePipe(2);
		kill(pid, SIGKILL);
		log->logError("open()");
		error = INTERNAL_ERROR;
		return false;
	}
	return true;
}

bool Cgi::initializeCgi()
{
	if (!checkRequest())
		return false;
	setPath();
	setEnvironment();
	if (!createPipes())
		return false;
	if (!forkProcess())
		return false;
	return true;
}

bool Cgi::forkProcess()
{
	pid = fork();
	if (pid < 0)
	{
		closePipe(2);
		log->logError("fork()");
		error = INTERNAL_ERROR;
		return false;
	}
	else if (pid == 0)
		childProcess();
	return true;
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
			error = NOT_FOUND;
			return false;
		}
	}
	if (parsed.getMethod() == "POST")
		post = true;
	if (parsed.getHeader("transfer-encoding") == "chunked")
		chunked = true;
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
		if (size > extension.size())
		{
			if (matched.getPath().substr(size - extension.size()) == extension)
			{
				Json::object obj = file.second.object_items();
				cgi_path = obj["path"].string_value();
				return;
			}
		}
	}
}

bool Cgi::createPipes()
{
	if (pipe(parent_output) < 0)
	{
		log->logError("pipe(parent_output)");
		error = INTERNAL_ERROR;
		return false;
	}
	if (pipe(child_output) < 0)
	{
		close(parent_output[0]);
		close(parent_output[1]);
		log->logError("pipe(child_output)");
		error = INTERNAL_ERROR;
		return false;
	}
	return true;
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
	std::string length("CONTENT_LENGTH=");

	if (post && !chunked)
		return;
	else if (post && chunked)
	{
		// wtf ?
		return;
	}
	else if (!post)
	{
		length += std::to_string(file.st_size);
		memory.push_back(std::move(length));
		env.push_back(memory.back().c_str());
	}
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

bool Cgi::isProcessed()
{
	return processed;
}

bool Cgi::isResolved()
{
	return resolved;
}

int Cgi::getError()
{
	return error;
}