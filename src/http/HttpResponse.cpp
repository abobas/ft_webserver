/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   HttpResponse.cpp                                   :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/08/26 19:27:31 by abobas        #+#    #+#                 */
/*   Updated: 2020/10/29 13:59:42 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "HttpResponse.hpp"

const int HttpResponse::CONTINUE = 100;
const int HttpResponse::SWITCHING_PROTOCOL = 101;
const int HttpResponse::OK = 200;
const int HttpResponse::CREATED = 201;
const int HttpResponse::MOVED_PERMANENTLY = 301;
const int HttpResponse::BAD_REQUEST = 400;
const int HttpResponse::UNAUTHORIZED = 401;
const int HttpResponse::FORBIDDEN = 403;
const int HttpResponse::NOT_FOUND = 404;
const int HttpResponse::METHOD_NOT_ALLOWED = 405;
const int HttpResponse::INTERNAL_SERVER_ERROR = 500;
const int HttpResponse::NOT_IMPLEMENTED = 501;
const int HttpResponse::SERVICE_UNAVAILABLE = 503;
const std::string HttpResponse::CONNECTION_TYPE = "keep-alive";

static std::string lineTerminator = "\r\n";

HttpResponse::HttpResponse(HttpRequest &request) : request(request)
{
}

void HttpResponse::addHeader(std::string name, std::string value)
{
	response_headers.insert(std::pair<std::string, std::string>(name, value));
}

void HttpResponse::sendData(std::string &&data)
{
	addStatusHeader(OK, "OK");
	addConnectionHeader(CONNECTION_TYPE);
	sendHeaders();
	request.getSocket().sendData(data);
}

void HttpResponse::sendData(std::string &data)
{
	addStatusHeader(OK, "OK");
	addConnectionHeader(CONNECTION_TYPE);
	sendHeaders();
	request.getSocket().sendData(data);
}

void HttpResponse::sendData(char const *data)
{
	addStatusHeader(OK, "OK");
	addConnectionHeader(CONNECTION_TYPE);
	sendHeaders();
	request.getSocket().sendData(data);
}

void HttpResponse::sendFile(std::string &path)
{
	addStatusHeader(OK, "OK");
	addConnectionHeader(CONNECTION_TYPE);
	addFileHeaders(path);
	sendHeaders();
	request.getSocket().sendFile(path);
}

void HttpResponse::sendFileHeaders(std::string &path)
{
	addStatusHeader(OK, "OK");
	addConnectionHeader(CONNECTION_TYPE);
	addFileHeaders(path);
	sendHeaders();
}

void HttpResponse::sendCreated(std::string &&path)
{
	addStatusHeader(CREATED, "Created");
	addConnectionHeader(CONNECTION_TYPE);
	addHeader("content-location", path);
	addFileHeaders(path);
	sendHeaders();
	request.getSocket().sendData("201: Created");
}

void HttpResponse::sendModified(std::string &&path)
{
	addStatusHeader(OK, "OK");
	addConnectionHeader(CONNECTION_TYPE);
	addHeader("content-location", path);
	addFileHeaders(path);
	sendHeaders();
	request.getSocket().sendData("200: OK (Modified)");
}

void HttpResponse::sendNotFound()
{
	addStatusHeader(NOT_FOUND, "Not Found");
	addHeader("content-type", "text/plain");
	addConnectionHeader(CONNECTION_TYPE);
	sendHeaders();
	request.getSocket().sendData("404: Not found");
}

void HttpResponse::sendBadRequest()
{
	addStatusHeader(BAD_REQUEST, "Bad Request");
	addHeader("content-type", "text/plain");
	addConnectionHeader(CONNECTION_TYPE);
	sendHeaders();
	request.getSocket().sendData("400: Bad request");
}

void HttpResponse::sendBadMethod()
{
	addStatusHeader(METHOD_NOT_ALLOWED, "Method Not Allowed");
	addHeader("content-type", "text/plain");
	addConnectionHeader(CONNECTION_TYPE);
	sendHeaders();
	request.getSocket().sendData("405: Method not allowed");
}

void HttpResponse::sendInternalError()
{
	addStatusHeader(INTERNAL_SERVER_ERROR, "Internal Server Error");
	addHeader("content-type", "text/plain");
	addConnectionHeader(CONNECTION_TYPE);
	sendHeaders();
	request.getSocket().sendData("500: Internal server error");
}

void HttpResponse::sendNotImplemented()
{
	addStatusHeader(NOT_IMPLEMENTED, "Not Implemented");
	addHeader("content-type", "text/plain");
	addConnectionHeader(CONNECTION_TYPE);
	sendHeaders();
	request.getSocket().sendData("501: Not implemented");
}

void HttpResponse::sendServiceUnavailable()
{
	addStatusHeader(SERVICE_UNAVAILABLE, "Service Unavailable");
	addHeader("content-type", "text/plain");
	addConnectionHeader(CONNECTION_TYPE);
	sendHeaders();
	request.getSocket().sendData("503: Service unavailable");
}

void HttpResponse::sendHeaders()
{
	std::ostringstream oss;
	oss << request.getVersion() << " " << status << " " << status_message << lineTerminator;

	for (auto &header : response_headers)
		oss << header.first.c_str() << ": " << header.second.c_str() << lineTerminator;

	oss << lineTerminator;
	request.getSocket().sendData(oss.str());
}

void HttpResponse::addStatusHeader(const int http_status, const std::string message)
{
	status = http_status;
	status_message = message;
}

void HttpResponse::addFileHeaders(std::string &path)
{
	addContentLengthHeader(path);
	addContentTypeHeader(path);
	addDateHeader();
	addLastModifiedHeader(path);
	addServerHeader();
}

void HttpResponse::addContentLengthHeader(std::string &path)
{
	struct stat file;
	if (stat(path.c_str(), &file) < 0)
		return;
	addHeader("content-length", std::to_string(file.st_size));
}

void HttpResponse::addContentTypeHeader(std::string &path)
{
	std::string type;
	size_t pos = path.find('.');
	
	if (pos == std::string::npos)
		return;
	if (path.substr(pos + 1) == "html" || path.substr(pos + 1) == "txt")
		type = "text/";
	else if (path.substr(pos + 1) == "jpg" || path.substr(pos + 1) == "jpg" || path.substr(pos + 1) == "png")
		type = "image/";
	else
		return ;
	type.append(path.substr(pos + 1));
	addHeader("content-type", type);
}

void HttpResponse::addDateHeader()
{
	struct timeval time;
	struct tm *tmp;
	char string[128];

	if (gettimeofday(&time, NULL))
		return;
	tmp = localtime(&time.tv_sec);
	strftime(string, 128, "%a, %d %b %C%y %T %Z", tmp);
	addHeader("date", string);
}

void HttpResponse::addLastModifiedHeader(std::string &path)
{
	struct stat file;
	struct tm *tmp;
	char string[128];

	if (stat(path.c_str(), &file) < 0)
		return;
	tmp = localtime(&file.st_mtime);
	strftime(string, 128, "%a, %d %b %C%y %T %Z", tmp);
	addHeader("last-modified", string);
}

void HttpResponse::addServerHeader()
{
	if (!request.getHeader("host").empty())
		addHeader("server", request.getHeader("host"));
}

void HttpResponse::addConnectionHeader(std::string value)
{
	addHeader("connection", value);
}