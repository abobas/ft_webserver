/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   HttpResponse.hpp                                   :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/08/26 19:10:00 by abobas        #+#    #+#                 */
/*   Updated: 2020/09/17 21:36:06 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <map>
#include "HttpRequest.hpp"

class HttpResponse
{
public:
	static const int HTTP_STATUS_CONTINUE;
	static const int HTTP_STATUS_SWITCHING_PROTOCOL;
	static const int HTTP_STATUS_OK;
	static const int HTTP_STATUS_MOVED_PERMANENTLY;
	static const int HTTP_STATUS_BAD_REQUEST;
	static const int HTTP_STATUS_UNAUTHORIZED;
	static const int HTTP_STATUS_FORBIDDEN;
	static const int HTTP_STATUS_NOT_FOUND;
	static const int HTTP_STATUS_METHOD_NOT_ALLOWED;
	static const int HTTP_STATUS_INTERNAL_SERVER_ERROR;
	static const int HTTP_STATUS_NOT_IMPLEMENTED;
	static const int HTTP_STATUS_SERVICE_UNAVAILABLE;

	HttpResponse(HttpRequest &httpRequest);
	virtual ~HttpResponse();

	void sendDataRaw(std::string &data);
	void sendData(std::string &data);
	void sendData(std::string &&data);
	void sendData(char const *data);
	void sendFile(std::string &path);
	void sendNotFound();
	void sendBadRequest();
	void sendBadMethod();
	void sendInternalError();
	void sendServiceUnavailable();
	void addHeader(std::string name, std::string value);

private:
	HttpRequest request;
	std::map<std::string, std::string> response_headers;
	int status;
	std::string status_message;

	void sendHeaders();
	
	void addStatusHeader(const int http_status = HttpResponse::HTTP_STATUS_OK, const std::string message = "OK");
	void addFileHeaders(std::string &path);
	void addContentTypeHeader(std::string &path);
	void addContentLengthHeader(std::string &path);
	void addLastModifiedHeader(std::string &path);
	void addDateHeader();
	void addServerHeader();
};

