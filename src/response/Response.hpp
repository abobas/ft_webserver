/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Response.hpp                                       :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/08/26 16:35:55 by abobas        #+#    #+#                 */
/*   Updated: 2020/10/28 01:35:15 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Data.hpp"
#include "File.hpp"
#include "Proxy.hpp"
#include "Upload.hpp"

/**
* @brief Response builder.
*/
class Response
{
public:
	Response(Data &&data);
	bool isProxySet();
	Socket getProxySocket();
	std::string getProxyRequest();

private:
	Data data;
	Socket proxy;
	std::string proxy_request;
	bool proxy_true = false;

	void handleProxy();
	bool isValid();
	bool isProxy();
	bool isFile();
	bool isUpload();
	bool validMethod();
};
