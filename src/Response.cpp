/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Response.cpp                                       :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/08/27 21:45:05 by abobas        #+#    #+#                 */
/*   Updated: 2020/10/23 18:31:58 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"

Response::Response(Data &&data) : data(data)
{
	if (!isValid())
		return ;
	if (isProxy())
		Proxy proxy(data);
	else if (isCgi())
		Cgi cgi(data);
	else if (isFile())
		File file(data);
	else if (isUpload())
		return ;
}

bool Response::isValid()
{
    if (data.not_found)
    {
        data.response.sendNotFound();
        return false;
    }
    // if (!validMethod())
    // {
    // 	data.response.sendBadMethod();
    //     return false;
    // }
    // if (!validHost())
    // {
    //     data.response.sendBadRequest();
    //     return false;
    // }
    return true;
}

bool Response::validMethod()
{
	for (auto accepted : data.location["accepted-methods"].array_items())
	{
		if (accepted == data.method)
			return true;
	}
    return false;
}

bool Response::validHost()
{
	size_t pos = data.request.getHeader("host").find(':');
	if (data.server["name"].string_value() != data.request.getHeader("host").substr(0, pos))
        return false;
    return true;
}

bool Response::isProxy()
{
	if (data.location["proxy_pass"].string_value().size() != 0)
        return true;
	return false;
}

bool Response::isCgi()
{
	if (data.path.substr(data.path.size() - 4) == std::string(".php") ||
	data.path.substr(data.path.size() - 3) == std::string(".pl"))
        return true;
	return false;
}

bool Response::isFile()
{
	if (data.method == "GET")
		return true;
	return false;
}

bool Response::isUpload()
{
	if (data.method == "POST")
		return true;
	return false;
}
