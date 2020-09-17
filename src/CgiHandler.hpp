/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   CgiHandler.hpp                                     :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/09/17 19:28:58 by abobas        #+#    #+#                 */
/*   Updated: 2020/09/17 21:57:05 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Json.hpp"
#include "HttpParser.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include <string>
#include <vector>

class CgiHandler
{
public:
    CgiHandler(HttpRequest &request, HttpResponse &response, Json::Json &config, std::string &path);
    ~CgiHandler();

    void resolve();

private:
    HttpRequest request;
    HttpResponse response;
    Json::Json config;
    std::string path;
    std::vector<char *> env;
    int restore_fd;
    int pipe_fd[2];

    std::string convertOutput();
    void redirectOutput();
    void resetOutput();
    void executeScript();
    void child();
    void waiting();
    void setEnvironment();
    void freeShit();
    
    //debugging
    void debug();
};