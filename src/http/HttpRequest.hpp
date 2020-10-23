/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   HttpRequest.hpp                                    :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/08/26 18:48:17 by abobas        #+#    #+#                 */
/*   Updated: 2020/10/23 17:54:03 by abobas        ########   odam.nl         */
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
	HttpRequest(Socket client, std::string request);
	virtual ~HttpRequest();
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

	std::string getBody();
	std::string getHeader(const std::string &name);
	std::map<std::string, std::string> getHeaders();
	std::string getMethod();
	std::string getPath();
	std::map<std::string, std::string> getQuery();
	Socket getSocket();
	std::string getVersion();
	bool isClosed() const;
	std::vector<std::string> pathSplit();
	std::string urlDecode(std::string str);

private:
	Socket client;
	HttpParser parser;
};
