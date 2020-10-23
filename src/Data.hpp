/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Data.hpp                                           :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/09/18 17:42:31 by abobas        #+#    #+#                 */
/*   Updated: 2020/10/23 18:43:53 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Socket.hpp"
#include "json/Json.hpp"
#include "http/HttpParser.hpp"
#include "http/HttpRequest.hpp"
#include "http/HttpResponse.hpp"
#include <string>

class Data
{
public:
	Socket client;
	Json config;
	HttpRequest request;
	HttpResponse response;
	Json::object server;
	Json::object location;
	std::string path;
	std::string method;
	size_t path_length;
	bool not_found = false;
	
	Data(Socket &client, Json &config, std::string &request);
		
private:
	void setServer();
	void setLocation();
	void setPath();
	void setMethod();
};

Data getData(Socket &client, Json &config, std::string &request);
