/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   ResponseHandler.cpp                                :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/08/27 21:45:05 by abobas        #+#    #+#                 */
/*   Updated: 2020/08/28 17:11:19 by abobas        ########   odam.nl         */
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
#include <iostream>
#include <unistd.h>

ResponseHandler::ResponseHandler(Socket &client, Json::Json &config, std::string &request)
    : client(client), config(config), request(client, request), response(this->request)
{ 
}

ResponseHandler::~ResponseHandler()
{
}

void ResponseHandler::resolve()
{
    //this->debug();
    ResourceHandler resource(this->client, this->config, this->request, this->response);
    resource.resolve();
}

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