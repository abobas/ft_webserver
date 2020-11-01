/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   HttpResponse.hpp                                   :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/08/26 19:10:00 by abobas        #+#    #+#                 */
/*   Updated: 2020/11/01 01:27:05 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "HttpRequest.hpp"
#include "../Log.hpp"
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
	static const std::string CONNECTION_TYPE;
	static const std::string ENCODING_TYPE;

	HttpResponse(HttpRequest &httpRequest);

	void sendChunkHeader();
	void sendChunk(const char *buffer, int size);
	void sendChunkEnd();

	void sendData(std::string &data);
	void sendData(std::string &&data);
	void sendFile(std::string &path);
	void sendCreated(std::string &path, std::string uri);
	void sendModified(std::string &path, std::string uri);
	void sendNotFound();
	void sendBadRequest();
	void sendForbidden();
	void sendBadMethod(std::string allow);
	void sendInternalError();
	void sendNotImplemented();
	void sendServiceUnavailable();
	
private:
	HttpRequest request;
	Log *log;
	std::map<std::string, std::string> response_headers;
	int status;
	std::string status_message;
	
	int readFile(std::string &path, std::string &buffer);
	void sendHeaders();
	void sendBody(std::string &data);
	void sendBody(std::string &&data);
	void addHeader(std::string name, std::string value);
	void addStatusHeader(int http_status, std::string message);
	void addGeneralHeaders();
	void addDateHeader();
	void addServerHeader();
	void addConnectionHeader(std::string value);
	void addDataHeaders(std::string &data);
	void addDataHeaders(std::string &&data);
	void addTransferEncodingHeader(std::string value);
	void addFileHeaders(std::string &path);
	void addFileTypeHeader(std::string &path);
	void addFileLengthHeader(std::string &path);
	void addLastModifiedHeader(std::string &path);
};
