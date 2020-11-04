/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Parser.hpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/11/02 23:11:54 by abobas        #+#    #+#                 */
/*   Updated: 2020/11/04 00:54:30 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Utils.hpp"
#include <string>
#include <map>

class Parser
{
public:
	static Parser getParsed(std::string &message);
	std::string getMethod() const;
	std::string getPath() const; 
	std::string getQuery() const;
	std::string getVersion() const;
	std::string getHeader(std::string header) const;
	std::map<std::string, std::string> getHeaders() const;
	bool hasBody() const;
	std::string getBody() const;
	size_t getBodySize() const;

private:
	static std::string CRLF;
	std::map<std::string, std::string> headers;
	std::string method;
	std::string path;
	std::string query;
	std::string version;
	std::string body;
	size_t body_size;

	Parser(std::string &message);
	void parseRequestLine(std::string &line);
	void parseStatusLine(std::string &line);
	void parsePathLine(std::string line);
	void parseHeader(std::string &line);
	std::string toStringToken(std::string::iterator &it, std::string &str, std::string &token);
	std::string toCharToken(std::string::iterator &it, std::string &str, char token);
	bool hasHeader(std::string name) const;
};
