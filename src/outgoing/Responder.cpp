/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Responder.cpp                                      :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/11/03 12:04:40 by abobas        #+#    #+#                 */
/*   Updated: 2020/11/11 19:58:18 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Responder.hpp"

#define IO_SIZE 1048576

Log *Responder::log = Log::getInstance();
std::string Responder::CRLF = "\r\n";
std::string Responder::CONNECTION_TYPE = "close";
int Responder::CONTINUE = 100;
int Responder::SWITCHING_PROTOCOL = 101;
int Responder::OK = 200;
int Responder::CREATED = 201;
int Responder::MOVED_PERMANENTLY = 301;
int Responder::BAD_REQUEST = 400;
int Responder::UNAUTHORIZED = 401;
int Responder::FORBIDDEN = 403;
int Responder::NOT_FOUND = 404;
int Responder::METHOD_NOT_ALLOWED = 405;
int Responder::PAYLOAD_TOO_LARGE = 413;
int Responder::URI_TOO_LARGE = 414;
int Responder::INTERNAL_SERVER_ERROR = 500;
int Responder::NOT_IMPLEMENTED = 501;
int Responder::SERVICE_UNAVAILABLE = 503;
int Responder::VERSION_NOT_SUPPORTED = 505;

Responder Responder::getResponder(int socket)
{
	return Responder(socket);
}

Responder::Responder(int socket, Parser &parsed)
	: socket(socket), parsed(&parsed)
{
}

Responder::Responder(int socket)
	: socket(socket), parsed(NULL)
{
}

void Responder::sendData(std::string &&data)
{
	addStatusHeader(OK, "OK");
	addGeneralHeaders();
	addDataHeaders(data);
	transmitHeaders();
	transmitData(data);
}

void Responder::sendData(std::string &data) // addHeader("content-type", "text/html"); // CGI zelf laten schrijven?
{
	addStatusHeader(OK, "OK");
	addGeneralHeaders();
	addDataHeaders(data);
	transmitHeaders();
	transmitData(data);
}

void Responder::sendDataRaw(std::string &data)
{
	transmitData(data);
}

void Responder::sendDataRaw(std::string &&data)
{
	transmitData(data);
}

void Responder::sendDataRaw(const char *buffer, int size)
{
	transmitData(buffer, size);
}

void Responder::sendCgiHeader()
{
	std::string data("HTTP/1.1 200 OK" + CRLF);

	if (send(socket, data.c_str(), data.size(), MSG_NOSIGNAL) < 0)
		log->logError("send()");
}

void Responder::sendFile(const std::string &path)
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
	transmitHeaders();
	transmitData(buffer);
}

void Responder::sendModified(const std::string &path, std::string uri)
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
	transmitHeaders();
	transmitData(buffer);
}

void Responder::sendCreated(const std::string &path, std::string uri)
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
	transmitHeaders();
	transmitData(buffer);
}

void Responder::sendNotFound()
{
	std::string message("404: not found\n");

	addStatusHeader(NOT_FOUND, "not found");
	addGeneralHeaders();
	addDataHeaders(message);
	transmitHeaders();
	transmitData(message);
}

void Responder::sendBadRequest()
{
	std::string message("400: bad request\n");

	addStatusHeader(BAD_REQUEST, "bad request");
	addGeneralHeaders();
	addDataHeaders(message);
	transmitHeaders();
	transmitData(message);
}

void Responder::sendForbidden()
{
	std::string message("403: forbidden\n");

	addStatusHeader(FORBIDDEN, "forbidden");
	addGeneralHeaders();
	addDataHeaders(message);
	transmitHeaders();
	transmitData(message);
}

void Responder::sendBadMethod(std::string allow)
{
	std::string message("405: method not allowed\n");

	addStatusHeader(METHOD_NOT_ALLOWED, "method not allowed");
	addHeader("allow", allow);
	addGeneralHeaders();
	addDataHeaders(message);
	transmitHeaders();
	transmitData(message);
}

void Responder::sendPayLoadTooLarge()
{
	std::string message("413: payload too large\n");

	addStatusHeader(PAYLOAD_TOO_LARGE, "payload too large");
	addGeneralHeaders();
	addDataHeaders(message);
	transmitHeaders();
	transmitData(message);
}

void Responder::sendUriTooLarge()
{
	std::string message("414: URI too large\n");

	addStatusHeader(URI_TOO_LARGE, "URI too large");
	addGeneralHeaders();
	addDataHeaders(message);
	transmitHeaders();
	transmitData(message);
}

void Responder::sendInternalError()
{
	std::string message("500: internal server error\n");

	addStatusHeader(INTERNAL_SERVER_ERROR, "internal server error");
	addGeneralHeaders();
	addDataHeaders(message);
	transmitHeaders();
	transmitData(message);
}

void Responder::sendNotImplemented()
{
	std::string message("501: not implemented\n");

	addStatusHeader(NOT_IMPLEMENTED, "not implemented");
	addGeneralHeaders();
	addDataHeaders(message);
	transmitHeaders();
	transmitData(message);
}

void Responder::sendServiceUnavailable()
{
	std::string message("503: service unavailable\n");

	addStatusHeader(SERVICE_UNAVAILABLE, "service unavailable");
	addGeneralHeaders();
	addDataHeaders(message);
	transmitHeaders();
	transmitData(message);
}

void Responder::sendVersionNotSupported()
{
	std::string message("505: version not supported\n");

	addStatusHeader(VERSION_NOT_SUPPORTED, "version not supported");
	addGeneralHeaders();
	addDataHeaders(message);
	transmitHeaders();
	transmitData(message);
}

void Responder::transmitData(const char *buffer, int size)
{
	if (send(socket, buffer, size, MSG_NOSIGNAL) < 0)
		log->logError("send()");
	//log->logBlock(buffer);
}

void Responder::transmitData(std::string &data)
{
	if (parsed != NULL)
	{
		if (parsed->getMethod() == "HEAD")
			return;
	}
	if (send(socket, data.c_str(), data.size(), MSG_NOSIGNAL) < 0)
		log->logError("send()");
	//log->logBlock(data);
}

void Responder::transmitData(std::string &&data)
{
	if (parsed != NULL)
	{
		if (parsed->getMethod() == "HEAD")
			return;
	}
	if (send(socket, data.c_str(), data.size(), MSG_NOSIGNAL) < 0)
		log->logError("send()");
	//log->logBlock(data);
}

void Responder::transmitHeaders()
{
	std::ostringstream oss;
	std::string data;

	oss << "HTTP/1.1";
	oss << " " << status << " " << status_message << CRLF;
	for (auto &header : response_headers)
		oss << header.first.c_str() << ": " << header.second.c_str() << CRLF;
	oss << CRLF;
	data = oss.str();
	if (send(socket, data.c_str(), data.size(), MSG_NOSIGNAL) < 0)
		log->logError("send()");
	log->logBlock(data);
}

int Responder::readFile(const std::string &path, std::string &buffer)
{
	char buf[IO_SIZE + 1];
	int fd;

	fd = open(path.c_str(), O_RDONLY);
	if (fd < 0)
	{
		log->logError("open()");
		return -1;
	}
	while (1)
	{
		int ret = read(fd, buf, IO_SIZE);
		if (ret < 0)
		{
			log->logError("read()");
			close(fd);
			return -1;
		}
		buf[ret] = '\0';
		buffer += buf;
		if (ret < IO_SIZE)
			break;
	}
	close(fd);
	return 0;
}

void Responder::addHeader(std::string name, std::string value)
{
	response_headers.insert(std::pair<std::string, std::string>(name, value));
}

void Responder::addStatusHeader(int http_status, std::string message)
{
	status = http_status;
	status_message = message;
}

void Responder::addDataHeaders(std::string &data)
{
	addHeader("content-type", "text/html");
	addHeader("content-length", std::to_string(data.size()));
}

void Responder::addDataHeaders(std::string &&data)
{
	addHeader("content-type", "text/html");
	addHeader("content-length", std::to_string(data.size()));
}

void Responder::addGeneralHeaders()
{
	addDateHeader();
	if (parsed != NULL)
		addServerHeader();
	addConnectionHeader(CONNECTION_TYPE);
}

void Responder::addFileHeaders(const std::string &path)
{
	addFileTypeHeader(path);
	addFileLengthHeader(path);
	addLastModifiedHeader(path);
}

void Responder::addFileLengthHeader(const std::string &path)
{
	struct stat file;

	if (stat(path.c_str(), &file) < 0)
		return;
	addHeader("content-length", std::to_string(file.st_size));
}

void Responder::addFileTypeHeader(const std::string &path)
{
	std::string type;
	size_t pos;

	pos = path.find('.');
	if (pos == std::string::npos)
		return;
	if (path.substr(pos + 1) == "txt")
		type = "text/plain";
	else if (path.substr(pos + 1) == "jpg" || path.substr(pos + 1) == "jpeg" || path.substr(pos + 1) == "png")
	{
		type = "image/";
		type.append(path.substr(pos + 1));
	}
	else
		return;
	addHeader("content-type", type);
}

void Responder::addLastModifiedHeader(const std::string &path)
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

void Responder::addServerHeader()
{
	if (!parsed->getHeader("host").empty())
		addHeader("server", parsed->getHeader("host"));
}

void Responder::addDateHeader()
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

void Responder::addConnectionHeader(std::string value)
{
	addHeader("connection", value);
}

void Responder::addTransferEncodingHeader(std::string value)
{
	addHeader("transfer-encoding", value);
}