/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   ResourceHandler.hpp                                :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/08/27 18:23:04 by abobas        #+#    #+#                 */
/*   Updated: 2020/08/31 22:15:53 by abobas        ########   odam.nl         */
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
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

class ResourceHandler
{
public:
    ResourceHandler(HttpRequest &request, HttpResponse &response, Json::Json::object &server, Json::Json::object &location, std::string &url);
    ~ResourceHandler();
    void resolve();

private:
    HttpRequest request;
    HttpResponse response;
    std::string url;
    std::string path;
    struct stat file;
    Json::Json::object server;
    Json::Json::object location;

    int setValues();
    void setPath();
    int setStat();

    // debugging
    void debug();
};
