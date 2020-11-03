/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Resource.hpp                                       :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/08/27 18:23:04 by abobas        #+#    #+#                 */
/*   Updated: 2020/11/03 02:35:18 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Data.hpp"
#include "Directory.hpp"
#include "Cgi.hpp"
#include "Upload.hpp"
#include "../logger/Log.hpp"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

/**
* @brief Handles file requests.
*/
class Resource
{
public:
    Resource(Data &data);

private:
    Data data;
	Log *log;
    struct stat file;

	void handleFile();
	void handleDirectory();
	void handleCgi();
	void handleUpload();
    bool setStat();
	bool isDirectory();
	bool isRegular();
	bool isCgi();
	bool isUpload();
};
