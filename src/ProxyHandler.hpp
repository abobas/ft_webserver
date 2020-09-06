/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   ProxyHandler.hpp                                   :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/09/06 18:32:53 by abobas        #+#    #+#                 */
/*   Updated: 2020/09/06 21:42:08 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Socket.hpp"
#include "Json.hpp"
#include "ResponseHandler.hpp"
#include "HttpParser.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include <netinet/in.h>

class ProxyHandler
{
public:
    ProxyHandler(HttpRequest &request, HttpResponse &response, Json::Json::object &location, std::string &uri);
    ~ProxyHandler();
    void resolve();
    
private:
    HttpRequest request;
    HttpResponse response;
    Json::Json config;
    Json::Json::object location;
    Socket proxy_socket;
    struct sockaddr_in proxy_addr;
    std::string raw_response;
    std::string uri;
    std::string host;

    void setPath();
    int createProxySocket();
    int setProxyAddress();
    int connectProxySocket();
    void sendProxyRequest();
    void receiveProxyResponse();
    void sendProxyResponse();
};
