/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Upload.cpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/10/23 20:38:10 by abobas        #+#    #+#                 */
/*   Updated: 2020/10/30 02:37:02 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Upload.hpp"

Upload::Upload(Data &data) : data(data)
{
	if (maxBodyLimit())
		return;
    if (existingFile())
    {
        deleteFile();
        modified = true;
    }
    addFile();
}

bool Upload::maxBodyLimit()
{
	if (data.config["http"]["max_body"].number_value() != 0)
	{
		if (data.request.getBody().size() > data.config["http"]["max_body"].number_value())
		{
			data.response.sendForbidden();
			return true;
		}
	}
	return false;
}

void Upload::addFile()
{
    std::ofstream file;
    
    file.open(data.path.c_str(), std::ofstream::out);
    if (!file.is_open())
		throw ("open()");
	/// TRANSFER ENCODING GARBAGE CLEANEN UIT BODY
    file << data.request.getBody();
    file.close();
    if (modified)
        data.response.sendModified(data.path, data.request.getPath());
    else
        data.response.sendCreated(data.path, data.request.getPath());
}

bool Upload::existingFile()
{
    int fd;
    
    fd = open(data.path.c_str(), O_RDONLY);
    if (fd < 0)
        return false;
    else
    {
        close(fd);
        return true;
    }
}

void Upload::deleteFile()
{
    if (remove(data.path.c_str()) < 0)
		throw "remove()";
}
