/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Cgi.hpp                                            :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/09/17 19:28:58 by abobas        #+#    #+#                 */
/*   Updated: 2020/11/11 17:52:11 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "../incoming/Parser.hpp"
#include "../incoming/Matcher.hpp"
#include "../incoming/Receiver.hpp"
#include "../outgoing/Responder.hpp"
#include "../logger/Log.hpp"
#include <string>
#include <map>
#include <vector>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>

class Cgi
{
public:
	static Cgi *getInstance(int socket, Parser &parsed, Matcher &matched);
	static void deleteInstance(int socket);

	void processCgiRequest();
	void resolveCgiRequest();

	bool isResolved();
	bool isProcessed();
	int getError();

private:
	static Log *log;
	static std::map<int, Cgi *> cgis;
	Parser parsed;
	Matcher matched;
	Receiver *receiver;
	std::vector<const char *> env;
	std::vector<std::string> memory;
	std::string cgi_path;
	std::string tmp_path;
	pid_t pid;
	struct stat get_file;
	struct stat tmp_file;
	int parent_output[2];
	int child_output[2];
	int tmp_fd;
	int get_fd;
	int socket;
	int error;
	int exit_status;
	bool initialized;
	bool processed;
	bool resolved;
	bool headers_sent;
	bool child_ready;
	bool post;
	bool chunked;

	Cgi(int socket, Parser &parsed, Matcher &matched);
	void readPipe(char *buf, int &bytes_read);
	void writeChunk(char *buf, int bytes_read);
	bool checkWait();
	void processChunked();
	void setTmp();
	void deleteTmp();
	void processContent();
	std::string getBodyType();
	size_t getBodySize();
	bool initializeCgi();
	void processCgi();
	bool forkProcess();
	void childProcess();
	void writePipeFromFile(int fd);
	bool openFile(int &fd, std::string path);
	bool readFile(int fd, char *buf, int &bytes_read);
	bool writeFile(int fd, const char *buf, int bytes_read);
	bool createPipes();
	void closePipe(int mode);
	bool checkRequest();
	void setPath();
	void setEnvironment();
	void setConfigEnv();
	void setMethodEnv();
	void setUriEnv();
	void setQueryEnv();
	void setHeadersEnv();
	void setServerNameEnv();
	void setServerPortEnv();
	void setScriptNameEnv();
	void setPathInfoEnv();
	void setContentLengthEnv();
};