/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   ResponseHandler.hpp                                :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/08/26 16:35:55 by abobas        #+#    #+#                 */
/*   Updated: 2020/08/28 18:13:15 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Socket.hpp"
#include "Json.hpp"
#include "HttpParser.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "ResourceHandler.hpp"
#include <string>
#include <iostream>
#include <unistd.h>

class ResponseHandler
{
public:
    ResponseHandler(Socket &client, Json::Json &config, std::string &request);
    ~ResponseHandler();
    void resolve();

private:
    Socket client;
    Json::Json config;
    HttpRequest request;
    HttpResponse response;

    // debugging
    void debug();
};
