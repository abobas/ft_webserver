/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   ResponseHandler.cpp                                :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/08/27 21:45:05 by abobas        #+#    #+#                 */
/*   Updated: 2020/08/31 22:37:39 by abobas        ########   odam.nl         */
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
        ResourceHandler resource(this->request, this->response, this->server, this->location, this->path);
        resource.resolve();
        return;
    }
}

int ResponseHandler::checkHeaders()
{
    this->setServer();
    if (this->setServerLocation())
        return 1;
    if (this->checkHeaderHost())
        return 1;
    if (this->checkHeaderMethod())
        return 1;
    return 0;
}

void ResponseHandler::setServer()
{
    size_t pos = this->request.getHeader("host").find(':');
    if (pos == std::string::npos)
    {
        this->server = this->config["http"]["servers"][0].object_items();
        return;
    }
    int port = std::stoi(this->request.getHeader("host").substr(pos + 1));
    for (size_t i = 0; i < this->config["http"]["servers"].array_items().size(); i++)
    {
        if (port == this->config["http"]["servers"][i]["listen"].number_value())
        {
            this->server = this->config["http"]["servers"][i].object_items();
            return;
        }
    }
}

// SUPER SHIT CODE // WORK TO DO
int ResponseHandler::setServerLocation()
{
    this->path = this->request.getPath();
    size_t max = 0;
    for (size_t i = 0; i < this->server["locations"].array_items().size(); i++)
    {
        std::string location = this->server["locations"][i]["location"].string_value();
        if (this->path.substr(0, location.size()) == location && location.size() > max)
        {
            this->location = this->server["locations"][i].object_items();
            max = location.size();
        }
    }
    if (max == 0)
    {
        this->response.sendNotFound();
        return 1;
    }
    if (max == 1)
        max -= 1;
    std::cout << "MAX: " << max << std::endl;
    std::cout << "PATH: " << this->path << std::endl;
    this->path = this->path.substr(max, std::string::npos);
    std::cout << "PATH: " << this->path << std::endl;
    return 0;
}

int ResponseHandler::checkHeaderMethod()
{
    this->method = this->request.getMethod();
    for (size_t i = 0; i < this->location["accepted-methods"].array_items().size(); i++)
    {
        if (this->location["accepted-methods"][i].string_value() == this->method)
            return 0;
    }
    this->response.sendBadRequest();
    return 1;
}

int ResponseHandler::checkHeaderHost()
{
    size_t pos = this->request.getHeader("host").find(':');
    if (this->server["name"].string_value() != this->request.getHeader("host").substr(0, pos))
    {
        this->response.sendBadRequest();
        return 1;
    }
    return 0;
}

// debugging
void ResponseHandler::debug()
{
    std::cout << "HOST: " << this->request.getHeader("host") << std::endl;
    std::cout << "METHOD: " << this->request.getMethod() << std::endl;
    std::cout << "PATH: " << this->request.getPath() << std::endl;
    /*
    std::map<std::string, std::string> headers = this->request.getHeaders();
    std::map<std::string, std::string> queries = this->request.getQuery();
    for (auto header : headers)
        std::cout << "HEADER: " << header.first << ": " << header.second << std::endl;
    for (auto query : queries)
        std::cout << "QUERY: " << query.first << ": " << query.second << std::endl;
    std::cout << "METHOD: " << this->request.getMethod() << std::endl;
    std::cout << "BODY: " << this->request.getBody() << std::endl;
    std::cout << "PATH: " << this->request.getPath() << std::endl;
    */
}
