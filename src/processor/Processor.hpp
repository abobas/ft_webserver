/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Processor.hpp                                      :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/11/05 13:13:08 by abobas        #+#    #+#                 */
/*   Updated: 2020/11/07 12:19:49 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Upload.hpp"
#include "Cgi.hpp"
#include "../incoming/Parser.hpp"
#include "../incoming/Matcher.hpp"
#include "../incoming/Receiver.hpp"
#include "../logger/Log.hpp"
#include <string>
#include <map>

class Processor
{
public:
	static Processor *getInstance(int socket, Parser &parsed, Matcher &matched, std::string type);
	static void deleteInstance(int socket);
	void processRequest();
	bool isProcessed();
	int getUploadStatus();
	std::string getUploadPath();
	int getError();

private:
	static Log *log;
	static std::map<int, Processor *> processors;
	Parser parsed;
	Matcher matched;
	Upload *upload;
	Cgi *cgi;
	std::string request_type;
	int socket;
	bool processed;

	Processor(int socket, Parser &parsed, Matcher &matched, std::string type);
};
