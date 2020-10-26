/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   HttpParser.cpp                                     :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/08/26 18:47:23 by abobas        #+#    #+#                 */
/*   Updated: 2020/10/26 18:19:06 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "HttpParser.hpp"

static std::string lineTerminator = "\r\n";

static std::string toStringToken(std::string::iterator &it, std::string &str, std::string &token)
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

static std::string toCharToken(std::string::iterator &it, std::string &str, char token)
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

std::pair<std::string, std::string> parseHeader(std::string &line)
{
	auto it = line.begin();
	std::string name = toCharToken(it, line, ':');
	utils::toLower(name);
	auto value = utils::trim(toStringToken(it, line, lineTerminator));
	return std::pair<std::string, std::string>(name, value);
}

HttpParser::HttpParser() = default;
HttpParser::~HttpParser() = default;

std::string HttpParser::getBody() { return body; }
std::string HttpParser::getMethod() { return method; }
std::string HttpParser::getURL() { return url; }
std::string HttpParser::getVersion() { return version; }
std::string HttpParser::getStatus() { return status; }
std::string HttpParser::getReason() { return reason; }

std::map<std::string, std::string> HttpParser::getHeaders()
{
	return headers;
}

std::string HttpParser::getHeader(const std::string &name)
{
	std::string local_name = name;

	utils::toLower(local_name);
	if (!hasHeader(local_name))
		return "";

	return headers.at(local_name);
}

bool HttpParser::hasHeader(const std::string &name)
{
	std::string local_name = name;
	return headers.find(utils::toLower(local_name)) != headers.end();
}

void HttpParser::parse(std::string message)
{
	auto it = message.begin();
	auto line = toStringToken(it, message, lineTerminator);

	parseRequestLine(line);
	line = toStringToken(it, message, lineTerminator);

	while (!line.empty())
	{
		headers.insert(parseHeader(line));
		line = toStringToken(it, message, lineTerminator);
	}

	body = message.substr(std::distance(message.begin(), it));
}

void HttpParser::parseRequestLine(std::string &line)
{
	auto it = line.begin();

	method = toCharToken(it, line, ' ');
	url = toCharToken(it, line, ' ');
	version = toCharToken(it, line, ' ');
}

void HttpParser::parseResponse(std::string message)
{
	auto it = message.begin();
	auto line = toStringToken(it, message, lineTerminator);
	parseStatusLine(line);
	line = toStringToken(it, message, lineTerminator);

	while (!line.empty())
	{
		headers.insert(parseHeader(line));
		line = toStringToken(it, message, lineTerminator);
	}

	body = message.substr(std::distance(message.begin(), it));
}

void HttpParser::parseStatusLine(std::string &line)
{
	auto it = line.begin();

	version = toCharToken(it, line, ' ');
	status = toCharToken(it, line, ' ');
	reason = toStringToken(it, line, lineTerminator);
}
