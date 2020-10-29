/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   HttpResponse.cpp                                   :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/08/26 19:27:31 by abobas        #+#    #+#                 */
/*   Updated: 2020/10/29 20:49:36 by abobas        ########   odam.nl         */
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
const std::string HttpResponse::ENCODING_TYPE = "chunked";

HttpResponse::HttpResponse(HttpRequest &request) : request(request)
{
	log = Log::getInstance();
}

void HttpResponse::sendHeaders()
{
	std::ostringstream oss;
	oss << request.getVersion() << " " << status << " " << status_message << CRLF;

	for (auto &header : response_headers)
		oss << header.first.c_str() << ": " << header.second.c_str() << CRLF;

	oss << CRLF;
	request.getSocket().sendData(oss.str());
}

void HttpResponse::addHeader(std::string name, std::string value)
{
	response_headers.insert(std::pair<std::string, std::string>(name, value));
}

void HttpResponse::sendBodyData(std::string &data)
{
	std::stringstream stream;
	
	stream << std::hex << data.size();
	request.getSocket().sendData(stream.str() + CRLF + data + CRLF);
	request.getSocket().sendData("0" + CRLF + CRLF);
}

void HttpResponse::sendBodyData(std::string &&data)
{
	std::stringstream stream;
	
	stream << std::hex << data.size();
	request.getSocket().sendData(stream.str() + CRLF + data + CRLF);
	request.getSocket().sendData("0" + CRLF + CRLF);
}

void HttpResponse::sendData(std::string &&data)
{
	addStatusHeader(OK, "OK");
	addStandardHeaders();
	sendHeaders();
	sendBodyData(data);
}

void HttpResponse::sendData(std::string &data)
{
	addStatusHeader(OK, "OK");
	addStandardHeaders();
	sendHeaders();
	sendBodyData(data);
}

void HttpResponse::sendFile(std::string &path)
{
	std::string buffer;
	struct stat file;

	if (stat(path.c_str(), &file) < 0)
	{
		sendInternalError();
		return;
	}
	if (readFile(path, buffer) < 0)
	{
		sendInternalError();
		return;
	}
	addStatusHeader(OK, "OK");
	addStandardHeaders();
	addFileHeaders(path);
	sendHeaders();
	sendBodyData(buffer);
}

void HttpResponse::sendFileRaw(std::string &path)
{
	std::string buffer;
	struct stat file;

	if (stat(path.c_str(), &file) < 0)
	{
		sendInternalError();
		return;
	}
	if (readFile(path, buffer) < 0)
	{
		sendInternalError();
		return;
	}
	sendBodyData(buffer);
}

void HttpResponse::sendFileHeaders(std::string &path)
{
	struct stat file;
	
	if (stat(path.c_str(), &file) < 0)
	{
		sendInternalError();
		return;
	}
	addStatusHeader(OK, "OK");
	addStandardHeaders();
	addFileHeaders(path);
	sendHeaders();
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

void HttpResponse::sendCreated(std::string &&path)
{
	addStatusHeader(CREATED, "Created");
	addStandardHeaders();
	addHeader("content-location", path);
	addFileHeaders(path);
	sendHeaders();
	sendBodyData("201: Created");
}

void HttpResponse::sendModified(std::string &&path)
{
	addStatusHeader(OK, "OK");
	addStandardHeaders();
	addHeader("content-location", path);
	addFileHeaders(path);
	sendHeaders();
	sendBodyData("200: OK (Modified)");
}

void HttpResponse::sendNotFound()
{
	addStatusHeader(NOT_FOUND, "Not Found");
	addPlainTextHeader();
	addStandardHeaders();
	sendHeaders();
	sendBodyData("404: Not found");
}

void HttpResponse::sendBadRequest()
{
	addStatusHeader(BAD_REQUEST, "Bad Request");
	addPlainTextHeader();
	addStandardHeaders();
	sendHeaders();
	sendBodyData("400: Bad request");
}

void HttpResponse::sendBadMethod()
{
	addStatusHeader(METHOD_NOT_ALLOWED, "Method Not Allowed");
	addPlainTextHeader();
	addStandardHeaders();
	sendHeaders();
	sendBodyData("405: Method not allowed");
}

void HttpResponse::sendInternalError()
{
	addStatusHeader(INTERNAL_SERVER_ERROR, "Internal Server Error");
	addPlainTextHeader();
	addStandardHeaders();
	sendHeaders();
	sendBodyData("500: Internal server error");
}

void HttpResponse::sendNotImplemented()
{
	addStatusHeader(NOT_IMPLEMENTED, "Not Implemented");
	addPlainTextHeader();
	addStandardHeaders();
	sendHeaders();
	sendBodyData("501: Not implemented");
}

void HttpResponse::sendServiceUnavailable()
{
	addStatusHeader(SERVICE_UNAVAILABLE, "Service Unavailable");
	addPlainTextHeader();
	addStandardHeaders();
	sendHeaders();
	sendBodyData("503: Service unavailable");
}

void HttpResponse::addPlainTextHeader()
{
	addHeader("content-type", "text/plain");
}

void HttpResponse::addStatusHeader(const int http_status, const std::string message)
{
	status = http_status;
	status_message = message;
}

void HttpResponse::addStandardHeaders()
{
	addDateHeader();
	addServerHeader();
	addTransferEncoding(ENCODING_TYPE);
	addConnectionHeader(CONNECTION_TYPE);
}

void HttpResponse::addFileHeaders(std::string &path)
{
	addContentTypeHeader(path);
	addLastModifiedHeader(path);
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

void HttpResponse::addTransferEncoding(std::string value)
{
	addHeader("transfer-encoding", value);
}