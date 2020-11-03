/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Upload.hpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/10/23 20:28:07 by abobas        #+#    #+#                 */
/*   Updated: 2020/11/03 16:29:57 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Responder.hpp"
#include "../evaluate/Matcher.hpp"
#include "../evaluate/Parser.hpp"
#include "../config/Config.hpp"
#include "../config/Json.hpp"
#include <stdio.h>
#include <fstream>
#include <unistd.h>
#include <fcntl.h>

/**
* @brief Resolves upload requests.
*/
class Upload
{
public:
	static void resolveUploadRequest(int socket, const Matcher &matched, const Parser &parsed);

private:
	static Json config;
	int socket;
	bool modified = false;

	Upload(int socket, const Matcher &matched, const Parser &parsed);
	void addFile(const Matcher &matched, const Parser &parsed);
	void deleteFile(const Matcher &matched);
	bool isExistingFile(const Matcher &matched);
	bool isMaxBodyLimit(const Parser &parsed);
};