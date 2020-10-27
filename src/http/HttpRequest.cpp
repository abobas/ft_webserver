/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   HttpRequest.cpp                                    :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/08/26 18:47:45 by abobas        #+#    #+#                 */
/*   Updated: 2020/10/27 13:36:30 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "HttpRequest.hpp"

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
	parser.parse(request);
	std::vector<std::string> parts = utils::split(getHeader(HTTP_HEADER_CONNECTION), ',');
}

std::string HttpRequest::getBody()
{
	return parser.getBody();
}

std::string HttpRequest::getHeader(const std::string &name)
{
	return parser.getHeader(name);
}

std::map<std::string, std::string> HttpRequest::getHeaders()
{
	return parser.getHeaders();
}

std::string HttpRequest::getMethod()
{
	return parser.getMethod();
}

std::string HttpRequest::getPath()
{
	std::string full_string = parser.getURL();
	int qindex = full_string.find_first_of('?');
	if (qindex < 0)
		return full_string;
	return full_string.substr(0, qindex);
}

std::string HttpRequest::getQueryString()
{
	std::string possible_query_string = parser.getURL();
	int qindex = possible_query_string.find_first_of('?');
	if (qindex < 0)
		return std::string("");
	return possible_query_string.substr(qindex + 1, -1);
}

Socket HttpRequest::getSocket()
{
	return client;
}

std::string HttpRequest::getVersion()
{
	return parser.getVersion();
}