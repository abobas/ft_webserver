/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   HttpResponse.hpp                                   :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/08/26 19:10:00 by abobas        #+#    #+#                 */
/*   Updated: 2020/08/28 18:08:01 by abobas        ########   odam.nl         */
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

	void addHeader(const std::string &name, const std::string &value);
	void setStatus(int status, const std::string &message);
	void sendData(std::string &data);
	void sendData(const char *packet_data);
	void sendFile(const std::string &path);
	void sendNotFound();

private:
	HttpRequest request;
	std::map<std::string, std::string> response_headers;
	int status;
	std::string status_message;
	void sendHeader();
};

