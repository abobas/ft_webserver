/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   HttpRequest.hpp                                    :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/08/26 18:48:17 by abobas        #+#    #+#                 */
/*   Updated: 2020/10/27 13:37:01 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "HttpParser.hpp"
#include "Utils.hpp"
#include "../Socket.hpp"
#include <sstream>
#include <vector>
#include <algorithm>
#include <string>
#include <map>

class HttpRequest
{
public:
	static const char HTTP_HEADER_ACCEPT[];
	static const char HTTP_HEADER_ALLOW[];
	static const char HTTP_HEADER_CONNECTION[];
	static const char HTTP_HEADER_CONTENT_LENGTH[];
	static const char HTTP_HEADER_CONTENT_TYPE[];
	static const char HTTP_HEADER_COOKIE[];
	static const char HTTP_HEADER_HOST[];
	static const char HTTP_HEADER_LAST_MODIFIED[];
	static const char HTTP_HEADER_ORIGIN[];
	static const char HTTP_HEADER_SEC_WEBSOCKET_ACCEPT[];
	static const char HTTP_HEADER_SEC_WEBSOCKET_PROTOCOL[];
	static const char HTTP_HEADER_SEC_WEBSOCKET_KEY[];
	static const char HTTP_HEADER_SEC_WEBSOCKET_VERSION[];
	static const char HTTP_HEADER_UPGRADE[];
	static const char HTTP_HEADER_USER_AGENT[];
	static const char HTTP_METHOD_CONNECT[];
	static const char HTTP_METHOD_DELETE[];
	static const char HTTP_METHOD_GET[];
	static const char HTTP_METHOD_HEAD[];
	static const char HTTP_METHOD_OPTIONS[];
	static const char HTTP_METHOD_PATCH[];
	static const char HTTP_METHOD_POST[];
	static const char HTTP_METHOD_PUT[];

	HttpRequest(Socket client, std::string request);
	std::string getBody();
	std::string getHeader(const std::string &name);
	std::map<std::string, std::string> getHeaders();
	std::string getMethod();
	std::string getPath();
	std::string getQueryString();
	Socket getSocket();
	std::string getVersion();

private:
	Socket client;
	HttpParser parser;
};
