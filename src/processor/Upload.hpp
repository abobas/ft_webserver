/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Upload.hpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/11/06 14:09:12 by abobas        #+#    #+#                 */
/*   Updated: 2020/11/27 22:53:49 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "../incoming/Parser.hpp"
#include "../incoming/Matcher.hpp"
#include "../incoming/Receiver.hpp"
#include "../logger/Log.hpp"
#include <string>
#include <map>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

class Upload
{
public:
	static Upload *getInstance(int socket, Parser &parsed, Matcher &matched);
	static void deleteInstance(int socket);
	void processUploadRequest();

	bool isProcessed();
	int getError();
	int getStatus();
	std::string getPath();

private:
	static Log *log;
	static std::map<int, Upload *> uploads;
	Parser parsed;
	Matcher matched;
	Receiver *receiver;
	std::string upload_path;
	int file;
	int socket;
	int status;
	int error;
	bool processed;
	bool initialized;

	Upload(int socket, Parser &parsed, Matcher &matched);
	void processUpload();
	bool uploadBody(const char *body, size_t bytes);
	std::string getBodyType();
	size_t getBodySize();
	bool initializeUpload();
	bool isExistingFile();
	bool deleteFile();
	bool createFile();
	void setPath();
	void checkUploadSize();
};