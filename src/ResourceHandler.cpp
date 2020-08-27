/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   ResourceHandler.cpp                                :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/08/27 22:06:27 by abobas        #+#    #+#                 */
/*   Updated: 2020/08/27 22:18:40 by abobas        ########   odam.nl         */
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
#include <unistd.h>

ResourceHandler::ResourceHandler(Socket client, Json::Json config, HttpRequest request)
    : client(client), config(config), request(request)
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
    this->setPath();
    this->setStat();
    this->setServerIndex();
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
    std::cout << "TESTING PATH: " << this->path << std::endl;
}

void ResourceHandler::setStat()
{
    if (stat(this->path.c_str(), &this->file) < 0)
        this->sendNotFound();
}

void ResourceHandler::handleResource()
{
    if (S_ISDIR(this->file.st_mode))
    {
        std::cout << "Requested file is a directory" << std::endl;
        this->handleDir();
    }
    else if (S_ISREG(this->file.st_mode))
        this->sendFile(this->path);
}

void ResourceHandler::handleDir()
{
    if (this->config["http"]["servers"][this->server_index]["autoindex"].string_value() == "on")
        this->handleDirListing();
    else
        this->handleIndex();
}

void ResourceHandler::handleDirListing()
{
    std::string data;
    DIR *dir;

    std::cout << "TESTING: " << this->path << std::endl;
    this->writeTitle(data);
    dir = opendir(this->path.c_str());
    if (!dir)
        this->sendNotFound();
    for (struct dirent *dirent = readdir(dir); dirent != 0; dirent = readdir(dir))
        this->writeFile(data, dirent->d_name);
    closedir(dir);
    this->sendData(data);
}

void ResourceHandler::writeTitle(std::string &data)
{
    data.append("<p><h1>Index of ");
    data.append(this->request.getPath());
    data.append("</h1></p>");
}

void ResourceHandler::writeFile(std::string &data, std::string file_name)
{
    data.append("<a href=\"");
    data.append(file_name);
    data.append("\">");
    data.append(file_name);
    data.append("</a><br>");
}

void ResourceHandler::handleIndex()
{
    DIR *dir;
    dir = opendir(this->path.c_str());
    if (!dir)
        throw "error: opendir failed in ResourceHandler->handleIndex()";
    for (struct dirent *dirent = readdir(dir); dirent != 0; dirent = readdir(dir))
    {
        for (size_t i = 0; i < this->config["http"]["servers"][this->server_index]["index"].array_items().size(); i++)
        {
            if (this->config["http"]["servers"][this->server_index]["index"][i].string_value() == dirent->d_name)
            {
                closedir(dir);
                this->path.append(this->config["http"]["servers"][this->server_index]["index"][i].string_value());
                this->sendFile(this->path);
                return;
            }
        }
    }
    closedir(dir);
    this->sendNotFound();
}

void ResourceHandler::sendFile(std::string path)
{
    HttpResponse response(&this->request);
    this->setContentTypeHeader(response, path);
    response.sendFile(path);
}

void ResourceHandler::setContentTypeHeader(HttpResponse &response, std::string path)
{
    size_t pos = path.find('.');
    if (pos == std::string::npos)
    {
        response.addHeader(HttpRequest::HTTP_HEADER_CONTENT_TYPE, "text/html");
        return;
    }
    std::string type("text/");
    type.append(path.substr(pos + 1));
    response.addHeader(HttpRequest::HTTP_HEADER_CONTENT_TYPE, type);
}

void ResourceHandler::sendData(std::string data)
{
    HttpResponse response(&this->request);
    response.addHeader(HttpRequest::HTTP_HEADER_CONTENT_TYPE, "text/html");
    response.sendData(data);
}

void ResourceHandler::sendNotFound()
{
    HttpResponse response(&this->request);
    response.sendNotFound();
}
