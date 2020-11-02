/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Receiver.hpp                                       :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/11/01 23:35:19 by abobas        #+#    #+#                 */
/*   Updated: 2020/11/02 22:31:42 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "../Log.hpp"
#include <string>
#include <map>

//find()
#include <algorithm>

//recv()
#include <sys/types.h>
#include <sys/socket.h>

/**
 * @brief Handles incoming socket communication.
 */
class Receiver
{
public:
	static Receiver *getInstance(int socket) noexcept;
	void deleteInstance(int socket);
	
	void receiveMessage();
	bool isReady();
	void getMessage(std::string &buffer);

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
	
	// receiving message
	void readSocket(std::string &buffer);
	bool headersReceived();

	// message body
	void checkContent();
	void checkContentReady();
	void checkChunked();
	void checkChunkedReady();
	void decodeChunkedMessage();
};
