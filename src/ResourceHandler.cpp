/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   ResourceHandler.cpp                                :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/08/27 22:06:27 by abobas        #+#    #+#                 */
/*   Updated: 2020/08/31 19:40:45 by abobas        ########   odam.nl         */
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

ResourceHandler::ResourceHandler(Json::Json &config, HttpRequest &request, HttpResponse &response, int index)
    : config(config), request(request), response(response), index(index) {}

ResourceHandler::~ResourceHandler() {}

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
