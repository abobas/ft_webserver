/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Receiver.hpp                                       :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/11/01 23:35:19 by abobas        #+#    #+#                 */
/*   Updated: 2020/11/10 18:43:59 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "../logger/Log.hpp"
#include "../config/Json.hpp"
#include <string>
#include <map>
#include <algorithm>
#include <sys/types.h>
#include <sys/socket.h>

class Receiver
{
public:
	static Receiver *getInstance(int socket);
	static void initializeReceiver(Json &config);
	static void deleteInstance(int socket);
	bool receiveSocketRaw(std::string &buffer);
	bool receiveHeaders();
	bool headersReceived();
	std::string getHeaders();
	void receiveBody();
	const char *getBodyData();
	size_t getBodyDataLength();
	bool bodyInitialized();
	bool bodyReceived();
	void initializeBodyType(std::string type, size_t length);

private:
	static Log *log;
	static std::map<int, Receiver *> receivers;
	static Json config;
	std::string message;
	std::string headers_part;
	std::string body_part;
	std::string body_data;
	int socket;
	bool headers_received;
	bool body_initialized;
	bool body_received;
	bool content;
	bool chunked;
	size_t content_length;
	size_t content_received;

	Receiver(int socket);
	void readSocket(std::string &buffer);
	bool checkHeadersReceived();
	void splitMessage();
	void evaluateBody();
	void splitChunked();
	bool chunkedEnd();
	void decodeChunkedBody();
};
