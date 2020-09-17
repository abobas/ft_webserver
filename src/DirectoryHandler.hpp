/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   DirectoryHandler.hpp                               :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/08/28 20:37:12 by abobas        #+#    #+#                 */
/*   Updated: 2020/09/17 19:29:39 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Json.hpp"
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
