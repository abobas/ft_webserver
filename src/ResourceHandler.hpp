/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   ResourceHandler.hpp                                :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/08/27 18:23:04 by abobas        #+#    #+#                 */
/*   Updated: 2020/08/27 22:00:42 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

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

class ResourceHandler
{
public:
    ResourceHandler(Socket client, Json::Json config, HttpRequest request)
        : client(client), config(config), request(request)
    {
    }
    
    ~ResourceHandler() {}
    void resolve()
    {
        this->setValues();
        this->handleResource();
    }

private:
    Socket client;
    Json::Json config;
    HttpRequest request;
    std::string path;
    struct stat file;
    int server_index;

    void setValues()
    {
        this->setPath();
        this->setStat();
        this->setServerIndex();
    }

    void setServerIndex()
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

    void setPath()
    {
        this->path = this->config["http"]["servers"][this->server_index]["root"].string_value();
        this->path.append(this->request.getPath());
        std::cout << "TESTING PATH: " << this->path << std::endl;
    }

    void setStat()
    {
        stat(this->path.c_str(), &this->file);
    }

    void handleResource()
    {
        if (S_ISDIR(this->file.st_mode))
        {
            std::cout << "Request file is a directory" << std::endl;
            this->handleDir();
        }
    }

    void handleDir()
    {
        if (this->config["http"]["servers"][this->server_index]["autoindex"].string_value() == "on")
            this->handleDirListing();
        else
            this->handleIndex();
    }

    void handleDirListing()
    {
        std::string data;
        DIR *dir;

        std::cout << "TESTING: " << this->path << std::endl;
        this->writeTitle(data);
        dir = opendir(this->path.c_str());
        if (!dir)
            throw "error: opendir failed in ResourceHandler->handleDirListing()";
        for (struct dirent *dirent = readdir(dir); dirent != 0; dirent = readdir(dir))
            this->writeFile(data, dirent->d_name);
        closedir(dir);
        this->sendData(data);
    }

    void writeTitle(std::string &data)
    {
        data.append("<p><h1>Index of ");
        data.append(this->request.getPath());
        data.append("</h1></p>");
    }

    void writeFile(std::string &data, std::string file_name)
    {
        data.append("<a href=\"localhost/");
        data.append(file_name);
        data.append("\">");
        data.append(file_name);
        data.append("</a><br>");
    }

    void handleIndex()
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
        HttpResponse response(&this->request);
        response.sendNotFound();
    }

    void sendFile(std::string path)
    {
        HttpResponse response(&this->request);
        this->setContentTypeHeader(response, path);
        response.sendFile(path);
    }

    void setContentTypeHeader(HttpResponse &response, std::string path)
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

    void sendData(std::string data)
    {
        HttpResponse response(&this->request);
        response.addHeader(HttpRequest::HTTP_HEADER_CONTENT_TYPE, "text/html");
        response.sendData(data);
    }
};
