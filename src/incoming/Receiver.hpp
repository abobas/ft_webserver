/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Receiver.hpp                                       :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/11/01 23:35:19 by abobas        #+#    #+#                 */
/*   Updated: 2020/11/05 16:32:22 by abobas        ########   odam.nl         */
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

/**
 * @brief Stateful object that reads from a socket until the message is completely received.
 */
class Receiver
{
public:
	static Receiver *getInstance(int socket) noexcept;
	static void initializeReceiver(Json &config);
	static void deleteInstance(int socket);
	void receiveHeaders();
	bool headersReceived();
	std::string getHeaders();

	void receiveBody();
	const char *getBodyData();
	size_t getBodyDataLength();

	bool bodyInitialized();
	bool bodyReceived();
	void initializeBodyType(std::string type, size_t length);

	
	// void consumeInstance(std::string &buffer);

private:
	static Log *log;
	static std::map<int, Receiver *> receivers;
	static Json config;
	std::string message;
	std::string headers_part;
	std::string body_part;
	std::string body_data;
	int socket;
	bool headers_received = false;

	bool body_initialized = false;
	bool body_received = false;
	bool content = false;
	bool chunked = false;
	size_t content_length;
	size_t content_received;

	Receiver(int socket);
	void readSocket(std::string &buffer);
	bool checkHeadersReceived();
	void splitMessage();
	void evaluateBody();



	// void checkContentReady();
	// void checkChunkedReady();
	// void decodeChunkedMessage();
};
