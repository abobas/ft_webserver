/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Resource.hpp                                       :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/08/27 18:23:04 by abobas        #+#    #+#                 */
/*   Updated: 2020/10/28 21:02:29 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Data.hpp"
#include "Directory.hpp"
#include "Cgi.hpp"
#include "Upload.hpp"
#include "../Log.hpp"
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
