/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Directory.hpp                                      :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/08/28 20:37:12 by abobas        #+#    #+#                 */
/*   Updated: 2020/11/04 17:00:15 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Responder.hpp"
#include "../evaluate/Parser.hpp"
#include "../evaluate/Matcher.hpp"
#include <string>
#include <dirent.h>
#include <ctype.h>

class Directory
{
public:
	/**
	* @brief Resolves requests involving directory listings and index files.
	*/
	static void resolveDirectoryRequest(int socket, Matcher &matched, Parser &parsed);

private:
	Matcher &matched;
	Parser &parsed;
	Responder respond;
	int socket;
	std::string dir_path;

	Directory(int socket, Matcher &matched, Parser &parsed);
	void setPath();
	bool isAutoIndex();
	void resolveDirListing();
	void resolveDirIndex();
	void writeDirTitle(std::string &data);
	void writeDirFile(std::string &data, std::string &&file);
};
