/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   File.hpp                                           :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/08/27 18:23:04 by abobas        #+#    #+#                 */
/*   Updated: 2020/10/28 01:40:41 by abobas        ########   odam.nl         */
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

	void fileHandler();
    bool setStat();
	bool isCgi();
};
