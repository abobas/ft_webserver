/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Parser.cpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/11/02 23:11:52 by abobas        #+#    #+#                 */
/*   Updated: 2020/11/04 00:54:37 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Parser.hpp"

std::string Parser::CRLF = "\r\n";

Parser Parser::getParsed(std::string &message)
{
	return Parser(message);
}

Parser::Parser(std::string &message)
{
	auto it = message.begin();
	auto line = toStringToken(it, message, CRLF);
	parseRequestLine(line);
	line = toStringToken(it, message, CRLF);
	while (!line.empty())
	{
		parseHeader(line);
		line = toStringToken(it, message, CRLF);
	}
	body = message.substr(std::distance(message.begin(), it));
	body_size = body.size();
}

void Parser::parsePathLine(std::string line)
{
	size_t pos;

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

std::string Parser::getHeader(std::string header) const
{
	utils::toLower(header);
	if (!hasHeader(header))
		return "";
	return headers.at(header);
}

bool Parser::hasHeader(std::string header) const
{
	return headers.find(utils::toLower(header)) != headers.end();
}

std::map<std::string, std::string> Parser::getHeaders() const
{
	return headers;
}

std::string Parser::getBody() const
{
	return body;
}

std::string Parser::getMethod() const
{
	return method;
}

std::string Parser::getQuery() const
{
	return query;
}

std::string Parser::getPath() const
{
	return path;
}

std::string Parser::getVersion() const
{
	return version;
}

size_t Parser::getBodySize() const
{
	return body_size;
}

bool Parser::hasBody() const
{
	return body.empty();
}