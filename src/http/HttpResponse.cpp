/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   HttpResponse.cpp                                   :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/08/26 19:27:31 by abobas        #+#    #+#                 */
/*   Updated: 2020/10/23 20:59:23 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "HttpResponse.hpp"

const int HttpResponse::HTTP_STATUS_CONTINUE = 100;
const int HttpResponse::HTTP_STATUS_SWITCHING_PROTOCOL = 101;
const int HttpResponse::HTTP_STATUS_OK = 200;
const int HttpResponse::HTTP_STATUS_MOVED_PERMANENTLY = 301;
const int HttpResponse::HTTP_STATUS_BAD_REQUEST = 400;
const int HttpResponse::HTTP_STATUS_UNAUTHORIZED = 401;
const int HttpResponse::HTTP_STATUS_FORBIDDEN = 403;
const int HttpResponse::HTTP_STATUS_NOT_FOUND = 404;
const int HttpResponse::HTTP_STATUS_METHOD_NOT_ALLOWED = 405;
const int HttpResponse::HTTP_STATUS_INTERNAL_SERVER_ERROR = 500;
const int HttpResponse::HTTP_STATUS_NOT_IMPLEMENTED = 501;
const int HttpResponse::HTTP_STATUS_SERVICE_UNAVAILABLE = 503;

static std::string lineTerminator = "\r\n";

HttpResponse::HttpResponse(HttpRequest &request) : request(request)
{
}

void HttpResponse::addHeader(std::string name, std::string value)
{
	response_headers.insert(std::pair<std::string, std::string>(name, value));
}

void HttpResponse::sendDataRaw(std::string &data)
{
	request.getSocket().sendData(data);
}

void HttpResponse::sendData(std::string &&data)
{
	addStatusHeader(200, "OK");
	sendHeaders();
	request.getSocket().sendData(data);
}

void HttpResponse::sendData(std::string &data)
{
	addStatusHeader(200, "OK");
	sendHeaders();
	request.getSocket().sendData(data);
}

void HttpResponse::sendData(char const *data)
{
	addStatusHeader(200, "OK");
	sendHeaders();
	request.getSocket().sendData(data);
}

void HttpResponse::sendFile(std::string &path)
{
	addStatusHeader(200, "OK");
	addFileHeaders(path);
	sendHeaders();
	request.getSocket().sendFile(path);
}

void HttpResponse::sendCreated(std::string &&path)
{
	addStatusHeader(201, "Created");
	addHeader("content-location", path);
	sendHeaders();
	request.getSocket().sendData("201: Created");
}

void HttpResponse::sendModified(std::string &&path)
{
	addStatusHeader(200, "OK");
	addHeader("content-location", path);
	sendHeaders();
	request.getSocket().sendData("200: OK (Modified)");
}

void HttpResponse::sendNotFound()
{
	addStatusHeader(404, "Not Found");
	addHeader("content-type", "text/plain");
	sendHeaders();
	request.getSocket().sendData("404: Not found");
}

void HttpResponse::sendBadRequest()
{
	addStatusHeader(400, "Bad Request");
	addHeader("content-type", "text/plain");
	sendHeaders();
	request.getSocket().sendData("400: Bad request");
}

void HttpResponse::sendBadMethod()
{
	addStatusHeader(405, "Method Not Allowed");
	addHeader("content-type", "text/plain");
	sendHeaders();
	request.getSocket().sendData("405: Method not allowed");
}

void HttpResponse::sendInternalError()
{
	addStatusHeader(500, "Internal Server Error");
	addHeader("content-type", "text/plain");
	sendHeaders();
	request.getSocket().sendData("500: Internal server error");
}

void HttpResponse::sendInternalError(std::string error)
{
	addStatusHeader(500, "Internal Server Error");
	addHeader("content-type", "text/plain");
	sendHeaders();
	std::string full("500: Internal server error");
	full += "\n"; full += error;
	request.getSocket().sendData(full);
}

void HttpResponse::sendServiceUnavailable()
{
	addStatusHeader(503, "Service Unavailable");
	addHeader("content-type", "text/plain");
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

// add support for more types than HTML (application/octet-stream etc)
void HttpResponse::addContentTypeHeader(std::string &path)
{
	size_t pos = path.find('.');
	if (pos == std::string::npos)
		return;
	std::string type("text/");
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
	addHeader("server", "BroServer/8.1.4");
}
