/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Parser.cpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/11/02 23:11:52 by abobas        #+#    #+#                 */
/*   Updated: 2020/11/05 12:43:50 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Parser.hpp"

std::string Parser::CRLF = "\r\n";

Parser::Parser()
{
}

Parser::Parser(std::string &headers)
{
	auto it = headers.begin();
	auto line = toStringToken(it, headers, CRLF);
	parseRequestLine(line);
	line = toStringToken(it, headers, CRLF);
	while (!line.empty())
	{
		parseHeader(line);
		line = toStringToken(it, headers, CRLF);
	}
}

void Parser::operator=(const Parser &other)
{
	headers = other.headers;
	method = other.method;
	uri = other.uri;
	path = other.path;
	query = other.query;
	version = other.version;
}

void Parser::parsePathLine(std::string line)
{
	size_t pos;

	uri = line;
	pos = line.find('?');
	if (pos != std::string::npos)
	{
		query = line.substr(pos + 1);
		path = line.substr(0, pos);
	}
	else
		path = line;
}

void Parser::parseRequestLine(std::string &line)
{
	auto it = line.begin();
	method = toCharToken(it, line, ' ');
	parsePathLine(toCharToken(it, line, ' '));
	version = toCharToken(it, line, ' ');
}

void Parser::parseHeader(std::string &line)
{
	auto it = line.begin();
	std::string name = toCharToken(it, line, ':');
	utils::toLower(name);
	auto value = utils::trim(toStringToken(it, line, CRLF));
	headers.insert({name, value});
}

std::string Parser::toStringToken(std::string::iterator &it, std::string &str, std::string &token)
{
	std::string ret;
	std::string part;
	auto itToken = token.begin();

	for (; it != str.end(); ++it)
	{
		if ((*it) == (*itToken))
		{
			part += (*itToken);
			++itToken;
			if (itToken == token.end())
			{
				++it;
				break;
			}
		}
		else
		{
			if (part.empty())
			{
				ret += part;
				part.clear();
				itToken = token.begin();
			}
			ret += *it;
		}
	}
	return ret;
}

std::string Parser::toCharToken(std::string::iterator &it, std::string &str, char token)
{
	std::string ret;
	for (; it != str.end(); ++it)
	{
		if ((*it) == token)
		{
			++it;
			break;
		}
		ret += *it;
	}
	return ret;
}

std::string Parser::getHeader(std::string header)
{
	utils::toLower(header);
	if (!hasHeader(header))
		return "";
	return headers.at(header);
}

bool Parser::hasHeader(std::string header)
{
	return (headers.find(utils::toLower(header)) != headers.end());
}

bool Parser::isChunked()
{
	return (getHeader("transfer-encoding") == "chunked");
}

bool Parser::hasContent()
{
	return hasHeader("content-length");
}

std::map<std::string, std::string> Parser::getHeaders()
{
	return headers;
}

std::string Parser::getMethod()
{
	return method;
}

std::string Parser::getQuery()
{
	return query;
}

std::string Parser::getUri()
{
	return uri;
}

std::string Parser::getPath()
{
	return path;
}

std::string Parser::getVersion()
{
	return version;
}
