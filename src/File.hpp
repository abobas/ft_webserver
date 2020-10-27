/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   File.hpp                                           :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/08/27 18:23:04 by abobas        #+#    #+#                 */
/*   Updated: 2020/10/27 21:50:24 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Data.hpp"
#include "Directory.hpp"
#include "Cgi.hpp"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

/**
* @brief Analyzes request and sends file to client.
*/
class File
{
public:
    File(Data &data);

private:
    Data data;
    struct stat file;

    bool setStat();
	bool isCgi();
};
