/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   HttpRequest.cpp                                    :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/08/26 18:47:45 by abobas        #+#    #+#                 */
/*   Updated: 2020/08/26 19:27:25 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include <sstream>
#include <vector>
#include <algorithm>
#include "HttpResponse.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "Utils.hpp"

#define STATE_NAME 0
#define STATE_VALUE 1

const char HttpRequest::HTTP_HEADER_ACCEPT[] = "Accept";
const char HttpRequest::HTTP_HEADER_ALLOW[] = "Allow";
const char HttpRequest::HTTP_HEADER_CONNECTION[] = "Connection";
const char HttpRequest::HTTP_HEADER_CONTENT_LENGTH[] = "Content-Length";
const char HttpRequest::HTTP_HEADER_CONTENT_TYPE[] = "Content-Type";
const char HttpRequest::HTTP_HEADER_COOKIE[] = "Cookie";
const char HttpRequest::HTTP_HEADER_HOST[] = "Host";
const char HttpRequest::HTTP_HEADER_LAST_MODIFIED[] = "Last-Modified";
const char HttpRequest::HTTP_HEADER_ORIGIN[] = "Origin";

const char HttpRequest::HTTP_HEADER_UPGRADE[] = "Upgrade";
const char HttpRequest::HTTP_HEADER_USER_AGENT[] = "User-Agent";

const char HttpRequest::HTTP_METHOD_CONNECT[] = "CONNECT";
const char HttpRequest::HTTP_METHOD_DELETE[] = "DELETE";
const char HttpRequest::HTTP_METHOD_GET[] = "GET";
const char HttpRequest::HTTP_METHOD_HEAD[] = "HEAD";
const char HttpRequest::HTTP_METHOD_OPTIONS[] = "OPTIONS";
const char HttpRequest::HTTP_METHOD_PATCH[] = "PATCH";
const char HttpRequest::HTTP_METHOD_POST[] = "POST";
const char HttpRequest::HTTP_METHOD_PUT[] = "PUT";

HttpRequest::HttpRequest(Socket client, std::string request)
	: client(client)
{
	this->parser.parse(request);
	std::vector<std::string> parts = utils::split(getHeader(HTTP_HEADER_CONNECTION), ',');
}

HttpRequest::~HttpRequest() = default;


/**
 * @brief Get the body of the HttpRequest.
 */
std::string HttpRequest::getBody()
{
	return this->parser.getBody();
}

/**
 * @brief Get the named header.
 * @param [in] name The name of the header field to retrieve.
 * @return The value of the header field.
 */
std::string HttpRequest::getHeader(const std::string &name)
{
	return this->parser.getHeader(name);
}

std::map<std::string, std::string> HttpRequest::getHeaders()
{
	return this->parser.getHeaders();
}

std::string HttpRequest::getMethod()
{
	return this->parser.getMethod();
}

std::string HttpRequest::getPath()
{
	return this->parser.getURL();
}

/**
 * @brief Get the query part of the request.
 * The query is a set of name = value pairs.  The return is a map keyed by the name items.
 *
 * @return The query part of the request.
 */
std::map<std::string, std::string> HttpRequest::getQuery()
{
	std::map<std::string, std::string> query_map;
	std::string possible_query_string = this->getPath();
	int qindex = possible_query_string.find_first_of('?');

	if (qindex < 0)
		return query_map;

	std::string query_string = possible_query_string.substr(qindex + 1, -1);

	int state = STATE_NAME;
	std::string name;
	std::string value;

	for (char currentChar : query_string)
	{
		if (state == STATE_NAME)
		{
			if (currentChar != '=')
			{
				name += currentChar;
			}
			else
			{
				state = STATE_VALUE;
				value = "";
			}
		}
		else
		{
			if (currentChar != '&')
			{
				value += currentChar;
			}
			else
			{
				query_map[name] = value;
				state = STATE_NAME;
				name = "";
			}
		}
	}

	if (state == STATE_VALUE)
		query_map[name] = value;

	return query_map;
}

/**
 * @brief Get the underlying socket.
 * @return The underlying socket.
 */
Socket HttpRequest::getSocket()
{
	return this->client;
}

std::string HttpRequest::getVersion()
{
	return this->parser.getVersion();
}

/**
 * Return the constituent parts of the path.
 * If we imagine a path as composed of parts separated by slashes, then this function
 * returns a vector composed of the parts.  For example:
 *
 * @return A vector of the constituent parts of the path.
 */
std::vector<std::string> HttpRequest::pathSplit()
{
	std::istringstream stream(this->getPath());
	std::vector<std::string> ret;
	std::string pathPart;

	while (std::getline(stream, pathPart, '/'))
	{
		ret.push_back(pathPart);
	}

	return ret;
}

/**
 * A simple hex conversion function.
 * @param ch
 * @return
 */
inline char from_hex(char ch)
{
	return isdigit(ch) ? ch - '0' : tolower(ch) - 'a' + 10;
}

/**
 * Decode a URL
 * @param text the text to decode
 * @return the decoded string.
 */
std::string HttpRequest::urlDecode(std::string text)
{
	char h;
	std::ostringstream escaped;
	escaped.fill('0');

	for (auto i = text.begin(), n = text.end(); i != n; ++i)
	{
		std::string::value_type c = (*i);

		if (c == '%')
		{
			if (i[1] && i[2])
			{
				h = from_hex(i[1]) << 4 | from_hex(i[2]);
				escaped << h;
				i += 2;
			}
		}
		else if (c == '+')
		{
			escaped << ' ';
		}
		else
		{
			escaped << c;
		}
	}

	return escaped.str();
}