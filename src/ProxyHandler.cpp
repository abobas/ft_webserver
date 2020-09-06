/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   ProxyHandler.cpp                                   :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/09/06 18:32:50 by abobas        #+#    #+#                 */
/*   Updated: 2020/09/06 22:06:42 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "ProxyHandler.hpp"
#include "Socket.hpp"
#include "Json.hpp"
#include "ResponseHandler.hpp"
#include "HttpParser.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sstream>

//debugging
#include <iostream>
#include <errno.h>

static std::string lineTerminator = "\r\n";

ProxyHandler::ProxyHandler(HttpRequest &request, HttpResponse &response, Json::Json::object &location, std::string &uri): 
    request(request), response(response), location(location), uri(uri) {}

ProxyHandler::~ProxyHandler() {}

void ProxyHandler::resolve()
{
    this->setPath();
    if (this->createProxySocket())
        return ;
    if (this->setProxyAddress())
        return ;
    if (this->connectProxySocket())
        return ;
    /*  FIXME: onderstaande stappen moeten eerst door select call, 
    Socket proxy_socket is in principe alleen nog maar nodig voor de communicatie met het proxy address
    */
    this->sendProxyRequest();
    this->receiveProxyResponse();
    this->sendProxyResponse();
}

void ProxyHandler::setPath()
{
    if (this->uri.size() == 0)
        this->uri = "/";
}

int ProxyHandler::createProxySocket()
{
    int new_socket;
    int enable = 1;
    
    new_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (new_socket == -1)
    {
        this->response.sendInternalError();
        return 1;
    }
    setsockopt(new_socket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));
    this->proxy_socket = Socket("proxy", new_socket);
    return 0;
}

int ProxyHandler::setProxyAddress()
{
    size_t pos = this->location["proxy_pass"].string_value().find(':');
    if (pos == std::string::npos)
        this->proxy_addr.sin_port = htons(80);
    else
        this->proxy_addr.sin_port = htons(std::stoi(this->location["proxy_pass"].string_value().substr(pos + 1, std::string::npos)));
    this->host = this->location["proxy_pass"].string_value().substr(0, pos - 1);
    if ((inet_pton(AF_INET, this->host.c_str(), &this->proxy_addr.sin_addr)) <= 0)
    {
        this->response.sendInternalError();
        return 1;
    }
    return 0;
}

int ProxyHandler::connectProxySocket()
{
    if (connect(this->proxy_socket.getSocket(), reinterpret_cast<sockaddr *>(&this->proxy_addr), sizeof(this->proxy_addr)) < 0) 
    {
        this->response.sendInternalError();
        return 1;
    }
    return 0;
}

void ProxyHandler::sendProxyRequest()
{
    std::ostringstream oss;
    std::map<std::string, std::string> headers = this->request.getHeaders();
    
    oss << this->request.getMethod() << ' ' << this->uri << ' ' << this->request.getVersion() << lineTerminator;
    oss << "host: " << this->host << lineTerminator;
    oss << "connection: close" << lineTerminator;
    for (auto header : headers)
    {
        if (header.first != "host" && header.first != "connection")
            oss << header.first << ": " << header.second << lineTerminator;
    }
    oss << lineTerminator;
    this->proxy_socket.sendData(oss.str());
}

void ProxyHandler::receiveProxyResponse()
{
    this->raw_response = this->proxy_socket.receive();
}

void ProxyHandler::sendProxyResponse()
{
    this->response.sendDataRaw(this->raw_response);
}