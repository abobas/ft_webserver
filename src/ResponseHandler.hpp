/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   ResponseHandler.hpp                                :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/08/26 16:35:55 by abobas        #+#    #+#                 */
/*   Updated: 2020/08/26 22:20:17 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Socket.hpp"
#include "Json.hpp"
#include "HttpParser.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include <iostream>
#include <unistd.h>

class ResponseHandler
{
public:
    ResponseHandler(Socket client, Json::Json config, std::string request)
        : client(client), config(config), request(client, request) {}

    ~ResponseHandler() {}

    void sendResponse()
    {
        HttpResponse response(&this->request);
        if (this->request.getMethod() == "GET")
        {
            if (this->request.getPath() == "/")
            {
                response.addHeader(HttpRequest::HTTP_HEADER_CONTENT_TYPE, "text/html");
                response.sendFile("index.html");
            }
        }
    }

private:
    Socket client;
    Json::Json config;
    HttpRequest request;

    void printRequest()
    {
        std::map<std::string, std::string> headers = this->request.getHeaders();
        std::map<std::string, std::string> queries = this->request.getQuery();
        for (auto header : headers)
            std::cout << "HEADER: " << header.first << ": " << header.second << std::endl;
        for (auto query : queries)
            std::cout << "QUERY: " << query.first << ": " << query.second << std::endl;
        std::cout << "METHOD: " << this->request.getMethod() << std::endl;
        std::cout << "BODY: " <<this->request.getBody() << std::endl;
        std::cout << "PATH: " <<this->request.getPath() << std::endl;
    }
};
