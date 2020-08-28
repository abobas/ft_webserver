/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   ResourceHandler.hpp                                :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/08/27 18:23:04 by abobas        #+#    #+#                 */
/*   Updated: 2020/08/28 20:54:11 by abobas        ########   odam.nl         */
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
    ResourceHandler(Json::Json &config, HttpRequest &request, HttpResponse &response);
    ~ResourceHandler();
    void resolve();

private:
    Json::Json config;
    HttpRequest request;
    HttpResponse response;
    std::string path;
    struct stat file;
    int index;

    void setValues();
    void setServerIndex();
    void setPath();
    void setStat();

    // debugging
    void debug();
};
