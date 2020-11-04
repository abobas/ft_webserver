/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Responder.hpp                                      :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/11/03 11:51:03 by abobas        #+#    #+#                 */
/*   Updated: 2020/11/04 16:01:41 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "../logger/Log.hpp"
#include "../evaluate/Parser.hpp"
#include <string>
#include <map>
#include <sstream>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>

class Responder
{
public:
	/**
	* @brief Returns a responder object, which allows for outgoing communication with a socket.
	* @param socket Socket to communicate with.
	*/
	static Responder getResponder(int socket);
	Responder(int socket, Parser &parsed);
	void sendData(std::string &data);
	void sendData(std::string &&data);
	void sendDataRaw(std::string &data);
	void sendChunkHeader();
	void sendChunk(const char *buffer, int size);
	void sendChunkEnd();
	void sendFile(const std::string &path);
	void sendCreated(const std::string &path, std::string uri);
	void sendModified(const std::string &path, std::string uri);
	void sendNotFound();
	void sendBadRequest();
	void sendForbidden();
	void sendBadMethod(std::string allow);
	void sendInternalError();
	void sendNotImplemented();
	void sendServiceUnavailable();

private:
	static Log *log;
	static std::string CRLF;
	static int CONTINUE;
	static int SWITCHING_PROTOCOL;
	static int OK;
	static int CREATED;
	static int MOVED_PERMANENTLY;
	static int BAD_REQUEST;
	static int UNAUTHORIZED;
	static int FORBIDDEN;
	static int NOT_FOUND;
	static int METHOD_NOT_ALLOWED;
	static int INTERNAL_SERVER_ERROR;
	static int NOT_IMPLEMENTED;
	static int SERVICE_UNAVAILABLE;
	static std::string CONNECTION_TYPE;
	static std::string ENCODING_TYPE;
	std::map<std::string, std::string> response_headers;
	int socket;
	Parser *parsed;
	int status;
	std::string status_message;

	Responder(int socket);
	void transmitData(std::string &data);
	void transmitData(std::string &&data);
	void transmitHeaders();
	int readFile(const std::string &path, std::string &buffer);
	void addHeader(std::string name, std::string value);
	void addStatusHeader(int http_status, std::string message);
	void addGeneralHeaders();
	void addDateHeader();
	void addServerHeader();
	void addConnectionHeader(std::string value);
	void addDataHeaders(std::string &data);
	void addDataHeaders(std::string &&data);
	void addTransferEncodingHeader(std::string value);
	void addFileHeaders(const std::string &path);
	void addFileTypeHeader(const std::string &path);
	void addFileLengthHeader(const std::string &path);
	void addLastModifiedHeader(const std::string &path);
};