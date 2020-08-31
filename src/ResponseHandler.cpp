/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   ResponseHandler.cpp                                :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/08/27 21:45:05 by abobas        #+#    #+#                 */
/*   Updated: 2020/08/31 20:03:20 by abobas        ########   odam.nl         */
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
    if (this->checkHeaders())
        return;
    // SWITCH / CASE implementation needed for methods
    if (this->method == "GET")
    {
        ResourceHandler resource(this->config, this->request, this->response, this->index);
        resource.resolve();
        return;
    }
}

void ResponseHandler::setServerIndex()
{
    size_t pos = this->request.getHeader("host").find(':');
    if (pos == std::string::npos)
    {
        this->index = 0;
        return;
    }
    int port = std::stoi(this->request.getHeader("host").substr(pos + 1));
    for (size_t i = 0; i < this->config["http"]["servers"].array_items().size(); i++)
    {
        if (port == this->config["http"]["servers"][i]["listen"].number_value())
        {
            this->index = i;
            return;
        }
    }
}

int ResponseHandler::checkHeaders()
{
    this->setServerIndex();
    if (this->checkHeaderHost())
        return 1;
    if (this->checkHeaderMethod())
        return 1;
    return 0;
}

int ResponseHandler::checkHeaderMethod()
{
    this->method = this->request.getMethod();
    for (size_t i = 0; i < this->config["http"]["servers"][this->index]["accepted-methods"].array_items().size(); i++)
    {
        if (this->config["http"]["servers"][this->index]["accepted-methods"][i].string_value() == this->method)
            return 0;
    }
    this->response.sendBadRequest();
    return 1;
}

int ResponseHandler::checkHeaderHost()
{
    size_t pos = this->request.getHeader("host").find(':');
    if (this->config["http"]["servers"][this->index]["name"].string_value() !=
        this->request.getHeader("host").substr(0, pos))
    {
        this->response.sendBadRequest();
        return 1;
    }
    return 0;
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