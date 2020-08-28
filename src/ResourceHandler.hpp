/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   ResourceHandler.hpp                                :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/08/27 18:23:04 by abobas        #+#    #+#                 */
/*   Updated: 2020/08/28 17:42:23 by abobas        ########   odam.nl         */
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
    ResourceHandler(Socket &client, Json::Json &config, HttpRequest &request, HttpResponse &response);
    ~ResourceHandler();
    void resolve();

private:
    Socket client;
    Json::Json config;
    HttpRequest request;
    HttpResponse response;
    std::string path;
    struct stat file;
    int server_index;

    void setValues();
    void setServerIndex();
    void setPath();
    void setStat();
    void handleResource();
    void handleDir();
    void handleDirListing();
    void writeDirTitle(std::string &data);
    void writeDirFile(std::string &data, std::string &&file_name);
    void handleDirIndex();
    void sendFile(std::string &path);
    void setContentTypeHeader(std::string &path);
    void sendDataHtml(std::string &data);
    void sendNotFound();

    // debugging
    void debug();
};
