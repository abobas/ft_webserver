/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Cgi.cpp                                            :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/09/17 19:27:46 by abobas        #+#    #+#                 */
/*   Updated: 2020/11/07 21:51:18 by abobas        ########   odam.nl         */
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
	tmp_fd = 0;
	get_fd = 0;
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
	respond.sendChunk(buf, bytes_read);
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
		log->logEntry("initialized CGI processor");
	}
	processCgi();
}

void Cgi::processCgi()
{
	if (!post)
	{
		log->logEntry("processing GET cgi request");
		if (!openFile(get_fd, matched.getPath()))
		{
			processed = true;
			return;
		}
		writePipeFromFile(get_fd);
		processed = true;
	}
	else if (post)
	{
		log->logEntry("processing POST cgi request");
		receiver = Receiver::getInstance(socket);
		if (!receiver->bodyInitialized())
		{
			receiver->initializeBodyType(getBodyType(), getBodySize());
			log->logEntry("initialized body receiver socket", socket);
		}
		if (!chunked)
			processContent();
		else if (chunked)
			processChunked();
	}
}

void Cgi::processContent()
{
	if (!receiver->bodyReceived())
	{
		receiver->receiveBody();
		if (!writeFile(parent_output[1], receiver->getBodyData(), receiver->getBodyDataLength()))
		{
			processed = true;
			return;
		}
	}
	if (receiver->bodyReceived())
	{
		processed = true;
		closePipe(0);
	}
}

void Cgi::processChunked()
{
	if (!receiver->bodyReceived())
	{
		receiver->receiveBody();
		if (!writeFile(tmp_fd, receiver->getBodyData(), receiver->getBodyDataLength()))
		{
			processed = true;
			return;
		}
	}
	if (receiver->bodyReceived())
	{
		processed = true;
		if (stat(tmp_path.c_str(), &tmp_file) < 0)
		{
			log->logError("stat()");
			return;
		}
		setEnvironment();
		if (!forkProcess())
			return;
		if (close(tmp_fd) < 0)
			log->logError("close()");
		if (!openFile(tmp_fd, tmp_path))
			return;
		writePipeFromFile(tmp_fd);
		deleteTmp();
	}
}

bool Cgi::initializeCgi()
{
	if (!checkRequest())
		return false;
	setPath();
	if (!createPipes())
		return false;
	if (!chunked)
	{
		setEnvironment();
		if (!forkProcess())
			return false;
	}
	else
	{
		setTmp();
		if (!openFile(tmp_fd, tmp_path))
			return false;
	}
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

void Cgi::writePipeFromFile(int fd)
{
	char buf[IO_SIZE + 1];
	int bytes_read;

	while (true)
	{
		if (!readFile(fd, buf, bytes_read))
			return;
		if (!writeFile(parent_output[1], buf, bytes_read))
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

bool Cgi::writeFile(int fd, const char *buf, int bytes_read)
{
	int ret;

	if (bytes_read == 0)
		return true;
	ret = write(fd, buf, bytes_read);
	if (ret < 0)
	{
		closePipe(2);
		if (kill(pid, SIGKILL) < 0)
			log->logError("kill()");
		log->logError("write()");
		error = INTERNAL_ERROR;
		return false;
	}
	log->logEntry("bytes written", ret);
	return true;
}

bool Cgi::readFile(int fd, char *buf, int &bytes_read)
{
	bytes_read = read(fd, buf, IO_SIZE);
	if (bytes_read < 0)
	{
		close(fd);
		closePipe(2);
		if (kill(pid, SIGKILL) < 0)
			log->logError("kill()");
		log->logError("read()");
		error = INTERNAL_ERROR;
		return false;
	}
	buf[bytes_read] = '\0';
	log->logEntry("bytes read", bytes_read);
	return true;
}

bool Cgi::openFile(int &fd, std::string path)
{
	fd = open(path.c_str(), O_RDWR | O_CREAT, 0777);
	if (fd < 0)
	{
		closePipe(2);
		if (kill(pid, SIGKILL) < 0)
			log->logError("kill()");
		log->logError("open()");
		error = INTERNAL_ERROR;
		return false;
	}
	return true;
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

void Cgi::setTmp()
{
	srand(time(0));
	tmp_path = "./tmp/" + std::to_string(rand());
}

void Cgi::deleteTmp()
{
	if (remove(tmp_path.c_str()) < 0)
		log->logError("remove()");
}

bool Cgi::checkRequest()
{
	if (parsed.getMethod() == "GET")
	{
		if (stat(matched.getPath().c_str(), &get_file) < 0)
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
	log->logEntry("Error: CGI script not properly configured");
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
		size_t pos = copy.find("-");
		if (pos != std::string::npos)
			copy[pos] = '_';
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
		length += std::to_string(tmp_file.st_size);
	else if (!post)
		length += std::to_string(get_file.st_size);
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

std::string Cgi::getBodyType()
{
	if (parsed.hasContent())
		return "content";
	else if (parsed.isChunked())
		return "chunked";
	else
		return "";
}

size_t Cgi::getBodySize()
{
	if (parsed.hasContent())
		return stoi(parsed.getHeader("content-length"));
	else
		return 0;
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