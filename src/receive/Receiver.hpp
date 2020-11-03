/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Receiver.hpp                                       :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/11/01 23:35:19 by abobas        #+#    #+#                 */
/*   Updated: 2020/11/03 11:34:38 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "../logger/Log.hpp"
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
	void consumeInstance(std::string &buffer);
	void receiveMessage();
	bool isReady();

private:
	static Log *log;
	static std::map<int, Receiver *> receivers;
	std::string message;
	int socket;
	size_t content_size;
	bool ready = false;
	bool content = false;
	bool chunked = false;
	bool headers_ready = false;

	Receiver(int socket);
	void readSocket(std::string &buffer);
	void deleteInstance(int socket);
	bool headersReceived();
	void checkContent();
	void checkContentReady();
	void checkChunked();
	void checkChunkedReady();
	void decodeChunkedMessage();
};
