/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   ResourceHandler.cpp                                :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/08/27 22:06:27 by abobas        #+#    #+#                 */
/*   Updated: 2020/08/28 21:05:25 by abobas        ########   odam.nl         */
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

ResourceHandler::ResourceHandler(Json::Json &config, HttpRequest &request, HttpResponse &response)
    : config(config), request(request), response(response)
{
}

ResourceHandler::~ResourceHandler()
{
}

void ResourceHandler::resolve()
{
    this->setValues();
    if (S_ISDIR(this->file.st_mode))
    {
        DirectoryHandler directory(this->request, this->response, this->config, this->index, this->path);
        directory.resolve();
        return;
    }
    else if (S_ISREG(this->file.st_mode))
        this->response.sendFile(this->path);
    else
        this->response.sendNotFound();
}

void ResourceHandler::setValues()
{
    this->setServerIndex();
    this->setPath();
    this->setStat();
    //this->debug();
}

// debugging
void ResourceHandler::debug()
{
    std::cout << "PATH: " << this->path << std::endl;
    std::cout << "SERVER_INDEX: " << this->index << std::endl;
}

void ResourceHandler::setServerIndex()
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

void ResourceHandler::setPath()
{
    this->path = this->config["http"]["servers"][this->index]["root"].string_value();
    this->path.append(this->request.getPath());
}

void ResourceHandler::setStat()
{
    if (stat(this->path.c_str(), &this->file) < 0)
        this->response.sendNotFound();
}
