/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   DirectoryHandler.hpp                               :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/08/28 20:37:12 by abobas        #+#    #+#                 */
/*   Updated: 2020/09/01 17:26:05 by abobas        ########   odam.nl         */
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
    DirectoryHandler(HttpRequest &request, HttpResponse &response, Json::Json::object &location, std::string &path);
    ~DirectoryHandler();

    void resolve();

private:
    HttpRequest request;
    HttpResponse response;
    Json::Json::object location;
    std::string path;

    void handleDirListing();
    void writeDirTitle(std::string &data);
    void writeDirFile(std::string &data, std::string &&file);
    void handleDirIndex();

    //debugging
    void debug();
};
