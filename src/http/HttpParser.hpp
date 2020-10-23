/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   HttpParser.hpp                                     :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/08/26 18:47:37 by abobas        #+#    #+#                 */
/*   Updated: 2020/10/23 17:54:23 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "../Socket.hpp"
#include "Utils.hpp"
#include <map>
#include <string>
#include <cstdlib>

class HttpParser {
public:
	HttpParser();
	virtual ~HttpParser();

	std::string getBody();
	std::string getHeader(const std::string& name);
	std::map<std::string, std::string> getHeaders();
	std::string getMethod();
	std::string getURL();
	std::string getVersion();
	std::string getStatus();
	std::string getReason();

	bool hasHeader(const std::string& name);

	void parse(std::string message);
	void parseResponse(std::string message);

private:
	std::string method;
	std::string url;
	std::string version;
	std::string body;
	std::string status;
	std::string reason;
	std::map<std::string, std::string> headers;

	void parseRequestLine(std::string& line);
	void parseStatusLine(std::string& line);

};

