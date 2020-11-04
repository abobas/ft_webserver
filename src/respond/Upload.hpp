/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Upload.hpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/10/23 20:28:07 by abobas        #+#    #+#                 */
/*   Updated: 2020/11/04 16:09:04 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Responder.hpp"
#include "../evaluate/Matcher.hpp"
#include "../evaluate/Parser.hpp"
#include <stdio.h>
#include <fstream>
#include <unistd.h>
#include <fcntl.h>

class Upload
{
public:
	/**
	* @brief Resolves upload request.
	*/
	static void resolveUploadRequest(int socket, Matcher &matched, Parser &parsed);

private:
	Matcher &matched;
	Parser &parsed;
	Responder respond;
	int socket;
	bool modified = false;

	Upload(int socket, Matcher &matched, Parser &parsed);
	void addFile();
	void deleteFile();
	bool isExistingFile();
	bool isMaxBodyLimit();
};