/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   HttpResponse.hpp                                   :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/08/26 19:10:00 by abobas        #+#    #+#                 */
/*   Updated: 2020/10/28 22:34:06 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "HttpRequest.hpp"
#include <string>
#include <map>
#include <sstream>
#include <fstream>
#include <utility>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <time.h>

class HttpResponse
{
public:
	static const int CONTINUE;
	static const int SWITCHING_PROTOCOL;
	static const int OK;
	static const int CREATED;
	static const int MOVED_PERMANENTLY;
	static const int BAD_REQUEST;
	static const int UNAUTHORIZED;
	static const int FORBIDDEN;
	static const int NOT_FOUND;
	static const int METHOD_NOT_ALLOWED;
	static const int INTERNAL_SERVER_ERROR;
	static const int NOT_IMPLEMENTED;
	static const int SERVICE_UNAVAILABLE;

	HttpResponse(HttpRequest &httpRequest);
	void sendData(std::string &data);
	void sendData(std::string &&data);
	void sendData(char const *data);
	void sendFileHeaders(std::string &path);
	void sendFile(std::string &path);
	void sendCreated(std::string &&path);
	void sendModified(std::string &&path);
	void sendNotFound();
	void sendBadRequest();
	void sendBadMethod();
	void sendInternalError();
	void sendNotImplemented();
	void sendServiceUnavailable();
	void addHeader(std::string name, std::string value);

private:
	HttpRequest request;
	std::map<std::string, std::string> response_headers;
	int status;
	std::string status_message;

	void sendHeaders();
	void addStatusHeader(const int http_status = HttpResponse::OK, const std::string message = "OK");
	void addFileHeaders(std::string &path);
	void addContentTypeHeader(std::string &path);
	void addContentLengthHeader(std::string &path);
	void addLastModifiedHeader(std::string &path);
	void addDateHeader();
	void addServerHeader();
	void addConnectionHeader(std::string value);
};
