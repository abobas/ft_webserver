/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Directory.hpp                                      :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/08/28 20:37:12 by abobas        #+#    #+#                 */
/*   Updated: 2020/11/03 16:16:10 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Responder.hpp"
#include "../evaluate/Parser.hpp"
#include "../evaluate/Matcher.hpp"
#include <string>
#include <dirent.h>

/**
* @brief Resolves requests involving directory listings and index files.
*/
class Directory
{
public:
	static void resolveDirectoryRequest(int socket, const Matcher &matched, const Parser &parsed);

private:
	int socket;
	std::string dir_path;

	Directory(int socket, const Matcher &matched, const Parser &parsed);
	bool isAutoIndex(const Matcher &matched);
	void resolveDirListing(const Parser &parsed);
	void resolveDirIndex(const Matcher &matched);
	void writeDirTitle(std::string &data, const Parser &parsed);
	void writeDirFile(std::string &data, std::string &&file, const Parser &parsed);
};
