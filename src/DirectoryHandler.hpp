/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   DirectoryHandler.hpp                               :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/08/28 20:37:12 by abobas        #+#    #+#                 */
/*   Updated: 2020/08/28 20:47:56 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "ResourceHandler.hpp"
#include "Socket.hpp"
#include "Json.hpp"
#include "ResponseHandler.hpp"
#include "HttpParser.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include <string>

class DirectoryHandler
{
public:
    DirectoryHandler(HttpRequest &request, HttpResponse &response, Json::Json &config, int &index, std::string &path);
    ~DirectoryHandler();

    void resolve();

private:
    Json::Json config;
    HttpRequest request;
    HttpResponse response;
    std::string path;
    int index;

    void handleDirListing();
    void writeDirTitle(std::string &data);
    void writeDirFile(std::string &data, std::string &&file);
    void handleDirIndex();
};
