/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   ResponseHandler.cpp                                :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/08/27 21:45:05 by abobas        #+#    #+#                 */
/*   Updated: 2020/08/31 18:32:47 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "ResponseHandler.hpp"
#include "Socket.hpp"
#include "Json.hpp"
#include "HttpParser.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "ResourceHandler.hpp"
#include <string>
#include <unistd.h>

// debugging
#include <iostream>

ResponseHandler::ResponseHandler(Socket &client, Json::Json &config, std::string &request)
    : client(client), config(config), request(client, request), response(this->request) {}

ResponseHandler::~ResponseHandler() {}

void ResponseHandler::resolve()
{
    //this->debug();
    if (this->checkHeaderHost())
        return;
    if (this->request.getMethod() == "GET")
    {
        ResourceHandler resource(this->config, this->request, this->response);
        resource.resolve();
        return;
    }
}

int ResponseHandler::checkHeaderHost()
{
    size_t pos = this->request.getHeader("host").find(':');
    if (pos == std::string::npos)
    {
        if (this->config["http"]["servers"][0]["name"].string_value() != this->request.getHeader("host"))
        {
            this->response.sendBadRequest();
            return 1;
        }
        return 0;
    }
    int port = std::stoi(this->request.getHeader("host").substr(pos + 1));
    std::string host = this->request.getHeader("host").substr(0, pos);
    for (size_t i = 0; i < this->config["http"]["servers"].array_items().size(); i++)
    {
        if (port == this->config["http"]["servers"][i]["listen"].number_value() &&
            host == this->config["http"]["servers"][i]["name"].string_value())
        {
            return 0;
        }
    }
    this->response.sendBadRequest();
    return 1;
}

// debugging
void ResponseHandler::debug()
{
    std::map<std::string, std::string> headers = this->request.getHeaders();
    std::map<std::string, std::string> queries = this->request.getQuery();
    for (auto header : headers)
        std::cout << "HEADER: " << header.first << ": " << header.second << std::endl;
    for (auto query : queries)
        std::cout << "QUERY: " << query.first << ": " << query.second << std::endl;
    std::cout << "METHOD: " << this->request.getMethod() << std::endl;
    std::cout << "BODY: " << this->request.getBody() << std::endl;
    std::cout << "PATH: " << this->request.getPath() << std::endl;
}