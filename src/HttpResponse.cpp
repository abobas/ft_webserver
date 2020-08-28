/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   HttpResponse.cpp                                   :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/08/26 19:27:31 by abobas        #+#    #+#                 */
/*   Updated: 2020/08/28 20:27:46 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */


#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include <sstream>
#include <fstream>
#include <utility>
#include <unistd.h>

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
	this->status = 200;
}

HttpResponse::~HttpResponse()
{
}

void HttpResponse::addHeader(std::string name, std::string value)
{
	this->response_headers.insert(std::pair<std::string, std::string>(name, value));
}

void HttpResponse::sendData(std::string &data)
{
	this->setStatus(HttpResponse::HTTP_STATUS_OK, "OK");
	this->sendHeader();
	this->request.getSocket().sendData(data);
}

void HttpResponse::sendData(char const *packet_data)
{
	this->setStatus(HttpResponse::HTTP_STATUS_OK, "OK");
	this->sendHeader();
	this->request.getSocket().sendData(packet_data);
}

void HttpResponse::sendFile(std::string &path)
{
	this->setStatus(HttpResponse::HTTP_STATUS_OK, "OK");
	this->addContentTypeHeader(path);
	this->sendHeader();
	this->request.getSocket().sendFile(path);
}

void HttpResponse::sendNotFound()
{
	this->setStatus(HttpResponse::HTTP_STATUS_NOT_FOUND, "Not Found");
	this->addHeader(HttpRequest::HTTP_HEADER_CONTENT_TYPE, "text/plain");
	this->sendHeader();
	this->request.getSocket().sendData("Not Found");
}

void HttpResponse::sendHeader()
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

void HttpResponse::setStatus(const int http_status, const std::string message)
{
	this->status = http_status;
	this->status_message = message;
}

void HttpResponse::addContentTypeHeader(std::string &path)
{
    size_t pos = path.find('.');
    if (pos == std::string::npos)
		return ;
    std::string type("text/");
    type.append(path.substr(pos + 1));
    this->addHeader(HttpRequest::HTTP_HEADER_CONTENT_TYPE, type);
}


