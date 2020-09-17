/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   HttpResponse.cpp                                   :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/08/26 19:27:31 by abobas        #+#    #+#                 */
/*   Updated: 2020/09/17 21:36:21 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */


#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include <sstream>
#include <fstream>
#include <utility>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <time.h>

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

HttpResponse::HttpResponse(HttpRequest &request): request(request)
{
}

HttpResponse::~HttpResponse()
{
}

void HttpResponse::addHeader(std::string name, std::string value)
{
	this->response_headers.insert(std::pair<std::string, std::string>(name, value));
}

void HttpResponse::sendDataRaw(std::string &data)
{
	this->request.getSocket().sendData(data);
}

void HttpResponse::sendData(std::string &&data)
{
	this->addStatusHeader(200, "OK");
	this->sendHeaders();
	this->request.getSocket().sendData(data);
}

void HttpResponse::sendData(std::string &data)
{
	this->addStatusHeader(200, "OK");
	this->sendHeaders();
	this->request.getSocket().sendData(data);
}

void HttpResponse::sendData(char const *data)
{
	this->addStatusHeader(200, "OK");
	this->sendHeaders();
	this->request.getSocket().sendData(data);
}

void HttpResponse::sendFile(std::string &path)
{
	this->addStatusHeader(200, "OK");
	this->addFileHeaders(path);
	this->sendHeaders();
	this->request.getSocket().sendFile(path);
}

void HttpResponse::sendNotFound()
{
	this->addStatusHeader(404, "Not Found");
	this->addHeader("content-type", "text/plain");
	this->sendHeaders();
	this->request.getSocket().sendData("404: Not found");
}

void HttpResponse::sendBadRequest()
{
	this->addStatusHeader(400, "Bad Request");
	this->addHeader("content-type", "text/plain");
	this->sendHeaders();
	this->request.getSocket().sendData("400: Bad request");
}

void HttpResponse::sendBadMethod()
{
	this->addStatusHeader(405, "Method Not Allowed");
	this->addHeader("content-type", "text/plain");
	this->sendHeaders();
	this->request.getSocket().sendData("405: Method not allowed");
}

void HttpResponse::sendInternalError()
{
	this->addStatusHeader(500, "Internal Server Error");
	this->addHeader("content-type", "text/plain");
	this->sendHeaders();
	this->request.getSocket().sendData("500: Internal server error");
}

void HttpResponse::sendServiceUnavailable()
{
	this->addStatusHeader(503, "Service Unavailable");
	this->addHeader("content-type", "text/plain");
	this->sendHeaders();
	this->request.getSocket().sendData("503: Service unavailable");
}


void HttpResponse::sendHeaders()
{
	std::ostringstream oss;
	oss << this->request.getVersion() << " " << this->status << " " << this->status_message << lineTerminator;

	for (auto &header : this->response_headers)
	{
		oss << header.first.c_str() << ": " << header.second.c_str() << lineTerminator;
	}

	oss << lineTerminator;
	this->request.getSocket().sendData(oss.str());
}

void HttpResponse::addStatusHeader(const int http_status, const std::string message)
{
	this->status = http_status;
	this->status_message = message;
}

void HttpResponse::addFileHeaders(std::string &path)
{
	this->addContentLengthHeader(path);
	this->addContentTypeHeader(path);
	this->addDateHeader();
	this->addLastModifiedHeader(path);
	this->addServerHeader();
}

void HttpResponse::addContentLengthHeader(std::string &path)
{
	struct stat file;
	if (stat(path.c_str(), &file) < 0)
		return ;
	this->addHeader("content-length", std::to_string(file.st_size));
}

// add support for more types than HTML (application/octet-stream etc)
void HttpResponse::addContentTypeHeader(std::string &path)
{
    size_t pos = path.find('.');
    if (pos == std::string::npos)
		return ;
    std::string type("text/");
    type.append(path.substr(pos + 1));
    this->addHeader("content-type", type);
}

void HttpResponse::addDateHeader()
{
	struct timeval time;
	struct tm *tmp;
	char string[128];
	
	if (gettimeofday(&time, NULL))
		throw "error: gettimeofday failed in HttpResponse::addDateHeader()";
	tmp = localtime(&time.tv_sec);
	strftime(string, 128, "%a, %d %b %C%y %T %Z", tmp);
	this->addHeader("date", string);
}

void HttpResponse::addLastModifiedHeader(std::string &path)
{
	struct stat file;
	struct tm *tmp;
	char string[128];
	
	if (stat(path.c_str(), &file) < 0)
		throw "error: stat failed in HttpResponse::addLastModifiedHeader()";
	tmp = localtime(&file.st_mtime);
	strftime(string, 128, "%a, %d %b %C%y %T %Z", tmp);
	this->addHeader("last-modified", string);
}

void HttpResponse::addServerHeader()
{
	this->addHeader("server", "BroServer/8.1.4");
}


