/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   ResourceHandler.hpp                                :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/08/27 18:23:04 by abobas        #+#    #+#                 */
/*   Updated: 2020/09/17 20:51:54 by abobas        ########   odam.nl         */
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
    ResourceHandler(HttpRequest &request, HttpResponse &response, Json::Json::object &server, Json::Json::object &location, std::string &path);
    ~ResourceHandler();
    void resolve();

private:
    HttpRequest request;
    HttpResponse response;
    std::string path;
    struct stat file;
    Json::Json::object server;
    Json::Json::object location;

    int setStat();

    // debugging
    void debug();
};
