/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   ResourceHandler.cpp                                :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/08/27 22:06:27 by abobas        #+#    #+#                 */
/*   Updated: 2020/08/28 20:34:14 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "ResourceHandler.hpp"
#include "Socket.hpp"
#include "Json.hpp"
#include "ResponseHandler.hpp"
#include "HttpParser.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include <string>
#include <iostream>
#include <sstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

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
    this->handleResource();
}

void ResourceHandler::setValues()
{
    this->setServerIndex();
    this->setPath();
    this->setStat();
    //this->debug();
}

void ResourceHandler::debug()
{
    std::cout << "PATH: " << this->path << std::endl;
    std::cout << "SERVER_INDEX: " << this->server_index << std::endl;
}

void ResourceHandler::setServerIndex()
{
    size_t pos = this->request.getHeader("host").find(':');
    if (pos == std::string::npos)
    {
        this->server_index = 0;
        return;
    }
    int port = std::stoi(this->request.getHeader("host").substr(pos + 1));
    for (size_t i = 0; i < this->config["http"]["servers"].array_items().size(); i++)
    {
        if (port == this->config["http"]["servers"][i]["listen"].number_value())
        {
            this->server_index = i;
            return;
        }
    }
}

void ResourceHandler::setPath()
{
    this->path = this->config["http"]["servers"][this->server_index]["root"].string_value();
    this->path.append(this->request.getPath());
}

void ResourceHandler::setStat()
{
    if (stat(this->path.c_str(), &this->file) < 0)
        this->response.sendNotFound();
}

void ResourceHandler::handleResource()
{
    if (S_ISDIR(this->file.st_mode))
        this->handleDir();
    else if (S_ISREG(this->file.st_mode))
        this->response.sendFile(this->path);
    else
        this->response.sendNotFound();
}

void ResourceHandler::sendDataHtml(std::string &data)
{
    this->response.addHeader(HttpRequest::HTTP_HEADER_CONTENT_TYPE, "text/html");
    this->response.sendData(data);
}

void ResourceHandler::handleDir()
{
    if (this->config["http"]["servers"][this->server_index]["autoindex"].string_value() == "on")
        this->handleDirListing();
    else
        this->handleDirIndex();
}

void ResourceHandler::handleDirListing()
{
    std::string data;
    DIR *dir;

    this->writeDirTitle(data);
    dir = opendir(this->path.c_str());
    if (!dir)
        throw "error: opendir failed in ResourceHandler::handleDirListing()";
    for (struct dirent *dirent = readdir(dir); dirent != 0; dirent = readdir(dir))
        this->writeDirFile(data, dirent->d_name);
    closedir(dir);
    this->sendDataHtml(data);
}

void ResourceHandler::writeDirTitle(std::string &data)
{
    data.append("<p><h1>Index of ");
    data.append(this->request.getPath());
    data.append("</h1></p>");
}

void ResourceHandler::writeDirFile(std::string &data, std::string &&file_name)
{
    data.append("<a href=\"");
    data.append(file_name);
    data.append("\">");
    data.append(file_name);
    data.append("</a><br>");
}

void ResourceHandler::handleDirIndex()
{
    DIR *dir;
    dir = opendir(this->path.c_str());
    if (!dir)
        throw "error: opendir failed in ResourceHandler::handleDirIndex()";
    for (struct dirent *dirent = readdir(dir); dirent != 0; dirent = readdir(dir))
    {
        for (size_t i = 0; i < this->config["http"]["servers"][this->server_index]["index"].array_items().size(); i++)
        {
            if (this->config["http"]["servers"][this->server_index]["index"][i].string_value() == dirent->d_name)
            {
                closedir(dir);
                this->path.append(this->config["http"]["servers"][this->server_index]["index"][i].string_value());
                this->response.sendFile(this->path);
                return;
            }
        }
    }
    closedir(dir);
    this->response.sendNotFound();
}
