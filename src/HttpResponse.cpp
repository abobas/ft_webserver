/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   HttpResponse.cpp                                   :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/08/26 19:27:31 by abobas        #+#    #+#                 */
/*   Updated: 2020/08/26 22:20:11 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include <sstream>
#include <fstream>
#include <utility>
#include "HttpRequest.hpp"
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

HttpResponse::HttpResponse(HttpRequest *request)
{
	this->request = request;
	this->status = 200;
	this->header_set = false;
}

HttpResponse::~HttpResponse() = default;

void HttpResponse::addHeader(const std::string &name, const std::string &value)
{
	if (!this->header_set)
		this->response_headers.insert(std::pair<std::string, std::string>(name, value));
}

void HttpResponse::close()
{
	if (!this->header_set)
		this->sendHeader();
}

std::string HttpResponse::getHeader(const std::string &name)
{
	if (this->response_headers.find(name) == this->response_headers.end())
		return "";
	return this->response_headers.at(name);
}

std::map<std::string, std::string> HttpResponse::getHeaders()
{
	return this->response_headers;
}

void HttpResponse::sendData(std::string data)
{

	if (!this->header_set)
		this->sendHeader();

	this->request->getSocket().send(std::move(data));
}

void HttpResponse::sendData(char *packet_data)
{

	if (!header_set)
		sendHeader();

	request->getSocket().send(packet_data);
}

void HttpResponse::sendFile(const std::string &file_name, size_t buffer_size)
{
	std::ifstream if_stream;
	if_stream.open(file_name, std::ifstream::in | std::ifstream::binary);

	if (!if_stream.is_open())
	{
		this->setStatus(HttpResponse::HTTP_STATUS_NOT_FOUND, "Not Found");
		this->addHeader(HttpRequest::HTTP_HEADER_CONTENT_TYPE, "text/plain");
		this->sendData("Not Found");
		this->close();
		return;
	}

	this->setStatus(HttpResponse::HTTP_STATUS_OK, "OK");
	this->sendHeader();

	char *packet_data = new char[buffer_size];
	while (!if_stream.eof())
	{
		if_stream.read((char *)packet_data, buffer_size);
		this->sendData(packet_data);
	}

	delete[] packet_data;
	if_stream.close();
	close();
}

void HttpResponse::sendHeader()
{
	if (this->header_set)
		return;

	std::ostringstream oss;
	oss << this->request->getVersion() << " " << this->status << " " << this->status_message << lineTerminator;

	for (auto &header : this->response_headers)
	{
		oss << header.first.c_str() << ": " << header.second.c_str() << lineTerminator;
	}

	oss << lineTerminator;
	this->header_set = true;
	this->request->getSocket().send(oss.str());
}

void HttpResponse::setStatus(const int http_status, const std::string &message)
{
	if (this->header_set)
		return;

	this->status = http_status;
	this->status_message = message;
}
