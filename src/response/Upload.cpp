/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Upload.cpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/10/23 20:38:10 by abobas        #+#    #+#                 */
/*   Updated: 2020/10/26 18:21:29 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Upload.hpp"

Upload::Upload(Data &data) : data(data)
{
    if (existingFile())
    {
        if (deleteFile() < 0)
            return ;
        modified = true;
    }
    addFile();
}

void Upload::addFile()
{
    std::ofstream file;
    
    file.open(data.path.c_str(), std::ofstream::out);
    if (!file.is_open())
    {
        data.response.sendInternalError();
        return ;
    }
    file << data.request.getBody();
    file.close();
    if (modified)
        data.response.sendModified(data.request.getPath());
    else
        data.response.sendCreated(data.request.getPath());
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

int Upload::deleteFile()
{
    if (remove(data.path.c_str()) < 0)
    {
		perror("remove()");
        data.response.sendInternalError();
        return -1;
    }
    return 0;
}
