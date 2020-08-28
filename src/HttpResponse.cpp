/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   HttpResponse.cpp                                   :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/08/26 19:27:31 by abobas        #+#    #+#                 */
/*   Updated: 2020/08/28 18:09:25 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */


#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include <sstream>
#include <fstream>
#include <utility>
#include <unistd.h>
#include <fcntl.h>

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
	this->response_headers.clear();
}

HttpResponse::~HttpResponse()
{
}

void HttpResponse::addHeader(const std::string &name, const std::string &value)
{
	this->response_headers.insert(std::pair<std::string, std::string>(name, value));
}

void HttpResponse::sendData(std::string &data)
{
	this->request.getSocket().send(data);
}

void HttpResponse::sendData(const char *packet_data)
{
	request.getSocket().send(packet_data);
}

void HttpResponse::sendFile(const std::string &path)
{
	char buf[257];
    std::string buffer;

	int fd = open(path.c_str(), O_RDONLY);
    while (1)
    {
        int ret = read(fd, buf, 256);
        buf[ret] = '\0';
        buffer += buf;
        if (ret < 256)
            break;
    }
	close(fd);
	this->setStatus(HttpResponse::HTTP_STATUS_OK, "OK");
	this->sendHeader();
	this->sendData(buffer);
}

void HttpResponse::sendNotFound()
{
	this->setStatus(HttpResponse::HTTP_STATUS_NOT_FOUND, "Not Found");
	this->addHeader(HttpRequest::HTTP_HEADER_CONTENT_TYPE, "text/plain");
	this->sendData("Not Found");
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
	this->request.getSocket().send(oss.str());
}

void HttpResponse::setStatus(const int http_status, const std::string &message)
{
	this->status = http_status;
	this->status_message = message;
}
