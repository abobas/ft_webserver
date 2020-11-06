/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Parser.hpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/11/02 23:11:54 by abobas        #+#    #+#                 */
/*   Updated: 2020/11/05 23:18:15 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Utils.hpp"
#include <string>
#include <map>

class Parser
{
public:
	Parser();
	Parser(std::string &headers);
	Parser(const Parser &rhs);
	Parser &operator=(const Parser &rhs);
	std::string getMethod();
	std::string getUri();
	std::string getPath();
	std::string getQuery();
	std::string getVersion();
	std::string getHeader(std::string header);
	std::map<std::string, std::string> getHeaders();
	bool isChunked();
	bool hasContent();

private:
	static std::string CRLF;
	std::map<std::string, std::string> headers;
	std::string method;
	std::string uri;
	std::string path;
	std::string query;
	std::string version;

	void parseRequestLine(std::string &line);
	void parseStatusLine(std::string &line);
	void parsePathLine(std::string line);
	void parseHeader(std::string &line);
	std::string toStringToken(std::string::iterator &it, std::string &str, std::string &token);
	std::string toCharToken(std::string::iterator &it, std::string &str, char token);
	bool hasHeader(std::string name);
};
