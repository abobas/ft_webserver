/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   HttpResponse.cpp                                   :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/08/26 19:27:31 by abobas        #+#    #+#                 */
/*   Updated: 2020/10/30 02:17:24 by abobas        ########   odam.nl         */
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

static std::string CRLF = "\r\n";

HttpResponse::HttpResponse(HttpRequest &request) : request(request)
{
	log = Log::getInstance();
}

void HttpResponse::sendData(std::string &&data)
{
	addStatusHeader(OK, "OK");
	addGeneralHeaders();
	addDataHeaders(data);
	sendHeaders();
	sendBody(data);
}

void HttpResponse::sendData(std::string &data)
{
	addStatusHeader(OK, "OK");
	addGeneralHeaders();
	addDataHeaders(data);
	sendHeaders();
	sendBody(data);
}

void HttpResponse::sendFile(std::string &path)
{
	std::string buffer;

	if (readFile(path, buffer) < 0)
	{
		sendInternalError();
		return;
	}
	addStatusHeader(OK, "OK");
	addGeneralHeaders();
	addFileHeaders(path);
	sendHeaders();
	sendBody(buffer);
}

void HttpResponse::sendHeaders()
{
	std::ostringstream oss;
	oss << "HTTP/1.1" << " " << status << " " << status_message << CRLF;

	for (auto &header : response_headers)
		oss << header.first.c_str() << ": " << header.second.c_str() << CRLF;

	oss << CRLF;
	request.getSocket().sendData(oss.str());
}

void HttpResponse::addHeader(std::string name, std::string value)
{
	response_headers.insert(std::pair<std::string, std::string>(name, value));
}

void HttpResponse::sendBody(std::string &data)
{
	if (request.getMethod() == "HEAD")
		return ;
	request.getSocket().sendData(data);
}

void HttpResponse::sendBody(std::string &&data)
{
	if (request.getMethod() == "HEAD")
		return ;
	request.getSocket().sendData(data);
}

void HttpResponse::sendCgi(std::string &path)
{
	std::string buffer;

	if (readFile(path, buffer) < 0)
	{
		sendInternalError();
		return;
	}
	addStatusHeader(OK, "OK");
	addHeader("content-type", "text/html");
	addTransferEncodingHeader("chunked");
	sendHeaders();
	sendBodyChunked(buffer);
}

void HttpResponse::sendBodyChunked(std::string &data)
{
	std::stringstream stream;
	
	if (request.getMethod() == "HEAD")
		return ;
	stream << std::hex << data.size();
	request.getSocket().sendData(stream.str() + CRLF + data + CRLF);
	request.getSocket().sendData("0" + CRLF + CRLF);
}

void HttpResponse::sendBodyChunked(std::string &&data)
{
	std::stringstream stream;
	
	if (request.getMethod() == "HEAD")
		return ;
	stream << std::hex << data.size();
	request.getSocket().sendData(stream.str() + CRLF + data + CRLF);
	request.getSocket().sendData("0" + CRLF + CRLF);
}

int HttpResponse::readFile(std::string &path, std::string &buffer)
{
	char buf[257];
	int fd;
	
	fd = open(path.c_str(), O_RDONLY);
	if (fd < 0)
	{
		log->logError("open()");
		return -1;
	}
	while (1)
	{
		int ret = read(fd, buf, 256);
		if (ret < 0)
		{
			log->logError("read()");
			close(fd);
			return -1;
		}
		buf[ret] = '\0';
		buffer += buf;
		if (ret < 256)
			break;
	}
	close(fd);
	return 0;
}

void HttpResponse::sendCreated(std::string &path, std::string uri)
{
	std::string buffer;

	if (readFile(path, buffer) < 0)
	{
		sendInternalError();
		return;
	}
	addStatusHeader(CREATED, "Created");
	addHeader("content-location", uri);
	addGeneralHeaders();
	addFileHeaders(path);
	sendHeaders();
	sendBody(buffer);
}

void HttpResponse::sendModified(std::string &path, std::string uri)
{
	std::string buffer;
	
	if (readFile(path, buffer) < 0)
	{
		sendInternalError();
		return;
	}
	addStatusHeader(OK, "OK");
	addHeader("content-location", uri);
	addGeneralHeaders();
	addFileHeaders(path);
	sendHeaders();
	sendBody(buffer);
}

void HttpResponse::sendNotFound()
{
	addStatusHeader(NOT_FOUND, "not found");
	addGeneralHeaders();
	addDataHeaders("404: not found\n");
	sendHeaders();
	sendBody("404: not found\n");
}

void HttpResponse::sendBadRequest()
{
	addStatusHeader(BAD_REQUEST, "bad request");
	addGeneralHeaders();
	addDataHeaders("400: bad request\n");
	sendHeaders();
	sendBody("400: bad request\n");
}

void HttpResponse::sendForbidden()
{
	addStatusHeader(FORBIDDEN, "forbidden");
	addGeneralHeaders();
	addDataHeaders("403: forbidden\n");
	sendHeaders();
	sendBody("403: forbidden\n");
}

void HttpResponse::sendBadMethod(std::string allow)
{
	addStatusHeader(METHOD_NOT_ALLOWED, "method not allowed");
	addHeader("allow", allow);
	addGeneralHeaders();
	addDataHeaders("405: method not allowed\n");
	sendHeaders();
	sendBody("405: method not allowed\n");
}

void HttpResponse::sendInternalError()
{
	addStatusHeader(INTERNAL_SERVER_ERROR, "internal server error");
	addGeneralHeaders();
	addDataHeaders("500: internal server error\n");
	sendHeaders();
	sendBody("500: internal server error\n");
}

void HttpResponse::sendNotImplemented()
{
	addStatusHeader(NOT_IMPLEMENTED, "not implemented");
	addGeneralHeaders();
	addDataHeaders("501: not implemented\n");
	sendHeaders();
	sendBody("501: not implemented\n");
}

void HttpResponse::sendServiceUnavailable()
{
	addStatusHeader(SERVICE_UNAVAILABLE, "service unavailable");
	addGeneralHeaders();
	addDataHeaders("503: service unavailable\n");
	sendHeaders();
	sendBody("503: service unavailable\n");
}

void HttpResponse::addGeneralHeaders()
{
	addDateHeader();
	addServerHeader();
	addConnectionHeader(CONNECTION_TYPE);
}

void HttpResponse::addDataHeaders(std::string &data)
{
	addHeader("content-type", "text/html");
	addHeader("content-length", std::to_string(data.size()));
}

void HttpResponse::addDataHeaders(std::string &&data)
{
	addHeader("content-type", "text/html");
	addHeader("content-length", std::to_string(data.size()));
}

void HttpResponse::addFileHeaders(std::string &path)
{
	addFileTypeHeader(path);
	addFileLengthHeader(path);
	addLastModifiedHeader(path);
}

void HttpResponse::addStatusHeader(int http_status, std::string message)
{
	status = http_status;
	status_message = message;
}

void HttpResponse::addFileLengthHeader(std::string &path)
{
	struct stat file;
	if (stat(path.c_str(), &file) < 0)
		return;
	addHeader("content-length", std::to_string(file.st_size));
}

void HttpResponse::addFileTypeHeader(std::string &path)
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

void HttpResponse::addTransferEncodingHeader(std::string value)
{
	addHeader("transfer-encoding", value);
}