/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   DirectoryHandler.cpp                               :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/08/28 20:37:10 by abobas        #+#    #+#                 */
/*   Updated: 2020/09/01 17:34:24 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "DirectoryHandler.hpp"
#include "ResourceHandler.hpp"
#include "Socket.hpp"
#include "Json.hpp"
#include "ResponseHandler.hpp"
#include "HttpParser.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

//debugging
#include <iostream>

DirectoryHandler::DirectoryHandler(HttpRequest &request, HttpResponse &response, Json::Json::object &location, std::string &path)
    : request(request), response(response), location(location), path(path) {}

DirectoryHandler::~DirectoryHandler() {}

void DirectoryHandler::resolve()
{
    if (this->path[this->path.size() - 1] != '/')
        this->path.append("/");
    if (this->location["autoindex"].string_value() == "on")
        this->handleDirListing();
    else
        this->handleDirIndex();
}

void DirectoryHandler::handleDirListing()
{
    std::string data;
    DIR *dir;

    this->writeDirTitle(data);
    dir = opendir(this->path.c_str());
    if (!dir)
        throw "error: opendir failed in DirectoryHandler::handleDirListing()";
    for (struct dirent *dirent = readdir(dir); dirent != 0; dirent = readdir(dir))
        this->writeDirFile(data, dirent->d_name);
    closedir(dir);
    this->response.sendData(data);
}

void DirectoryHandler::writeDirTitle(std::string &data)
{
    data.append("<p><h1>Index of ");
    data.append(this->request.getPath());
    data.append("</h1></p>");
}

void DirectoryHandler::writeDirFile(std::string &data, std::string &&file)
{
    data.append("<a href=\"");
    data.append(this->request.getPath());
    if (data[data.size() - 1] != '/')
        data.append("/");
    data.append(file);
    data.append("\">");
    data.append(file);
    data.append("</a><br>");
}

void DirectoryHandler::handleDirIndex()
{
    DIR *dir;
    dir = opendir(this->path.c_str());
    if (!dir)
        throw "error: opendir failed in DirectoryHandler::handleDirIndex()";
    for (struct dirent *dirent = readdir(dir); dirent != 0; dirent = readdir(dir))
    {
        for (auto index_file : this->location["index"].array_items())
        {
            if (index_file.string_value() == dirent->d_name)
            {
                closedir(dir);
                this->path.append(index_file.string_value());
                this->response.sendFile(this->path);
                return;
            }
        }
    }
    closedir(dir);
    this->response.sendNotFound();
}

//debugging
void DirectoryHandler::debug()
{
    std::cout << "PATH: " << this->path << std::endl;
}