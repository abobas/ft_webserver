/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Processor.hpp                                      :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/11/05 13:13:08 by abobas        #+#    #+#                 */
/*   Updated: 2020/11/06 22:35:36 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Upload.hpp"
#include "../Parser.hpp"
#include "../Matcher.hpp"
#include "../Receiver.hpp"
#include "../../logger/Log.hpp"
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
	std::string request_type;
	int socket;
	bool processed;

	Processor(int socket, Parser &parsed, Matcher &matched, std::string type);
};
