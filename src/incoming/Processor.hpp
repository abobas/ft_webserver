/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Processor.hpp                                      :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/11/05 13:13:08 by abobas        #+#    #+#                 */
/*   Updated: 2020/11/05 16:32:28 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Parser.hpp"
#include "Matcher.hpp"
#include "Receiver.hpp"
#include "../logger/Log.hpp"
#include <string>
#include <map>
#include <unistd.h>
#include <fcntl.h>

class Processor
{
public:
	static Processor *getInstance(int socket, Parser &parsed, Matcher &matched, std::string type) noexcept;
	static void deleteInstance(int socket);
	void processRequest();
	bool isProcessed();
	int getStatus();

private:
	static Log *log;
	static std::map<int, Processor *> processors;
	Parser parsed;
	Matcher matched;
	Receiver *receiver;
	std::string request_type;
	int file;
	int socket;
	int status;

	bool initialized = false;
	bool processed = false;

	Processor(int socket, Parser &parsed, Matcher &matched, std::string type);
	void processUpload();
	bool uploadBody(const char *body, size_t bytes);
	bool initializeUpload();
	bool isExistingFile();
	bool deleteFile();
	bool createFile();
	void processCgi();
	std::string getBodyType();
	size_t getBodySize();
}
