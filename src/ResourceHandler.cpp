/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   ResourceHandler.cpp                                :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/08/27 22:06:27 by abobas        #+#    #+#                 */
/*   Updated: 2020/08/31 22:17:10 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "ResourceHandler.hpp"
#include "DirectoryHandler.hpp"
#include "Socket.hpp"
#include "Json.hpp"
#include "ResponseHandler.hpp"
#include "HttpParser.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include <string>
#include <sys/types.h>
#include <sys/stat.h>

// debugging
#include <iostream>

ResourceHandler::ResourceHandler(HttpRequest &request, HttpResponse &response, Json::Json::object &server, Json::Json::object &location, std::string &url)
    : request(request), response(response), url(url), server(server), location(location)  {}

ResourceHandler::~ResourceHandler() {}

void ResourceHandler::resolve()
{
    if (this->setValues())
        return ;
    if (S_ISDIR(this->file.st_mode))
    {
        DirectoryHandler directory(this->request, this->response, this->location, this->path);
        directory.resolve();
        return;
    }
    else if (S_ISREG(this->file.st_mode))
        this->response.sendFile(this->path);
    else
        this->response.sendNotFound();
}

int ResourceHandler::setValues()
{
    this->setPath();
    this->debug();
    if (this->setStat())
        return 1;
    return 0;
}

void ResourceHandler::setPath()
{
    this->path = this->location["root"].string_value();
    this->path.append(this->url);
}

int ResourceHandler::setStat()
{
    if (stat(this->path.c_str(), &this->file) < 0)
    {
        this->response.sendNotFound();
        return 1;
    }
    return 0;
}

// debugging
void ResourceHandler::debug()
{
    std::cout << "PATH: " << this->path << std::endl;
}