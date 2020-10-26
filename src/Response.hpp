/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Response.hpp                                       :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/08/26 16:35:55 by abobas        #+#    #+#                 */
/*   Updated: 2020/10/26 15:15:47 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Data.hpp"
#include "File.hpp"
#include "Cgi.hpp"
#include "Proxy.hpp"
#include "Upload.hpp"

/**
* @brief Response builder.
*/
class Response
{
public:
	Response(Data &&data);

private:
	Data data;

	bool isValid();
	bool isProxy();
	bool isCgi();
	bool isFile();
	bool isUpload();
	bool validMethod();
};
