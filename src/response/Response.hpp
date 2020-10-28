/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Response.hpp                                       :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/08/26 16:35:55 by abobas        #+#    #+#                 */
/*   Updated: 2020/10/28 20:23:27 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Data.hpp"
#include "Resource.hpp"
#include "Proxy.hpp"
#include "../Log.hpp"

/**
* @brief Response builder.
*/
class Response
{
public:
	Response(Data &&data);
	Socket getProxySocket();
	std::string getProxyRequest();
	bool getProxyValue();

private:
	Data data;
	Log *log;
	Socket proxy_socket;
	std::string proxy_request;
	bool proxy_value = false;

	void handleProxy();
	void handleResource();
	bool isProxy();
	void setProxyValue();
	
	bool isValid();
	bool acceptedMethod();
	bool isResource();
};
