/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Directory.hpp                                      :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/08/28 20:37:12 by abobas        #+#    #+#                 */
/*   Updated: 2020/11/04 00:42:52 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Responder.hpp"
#include "../evaluate/Parser.hpp"
#include "../evaluate/Matcher.hpp"
#include <string>
#include <dirent.h>

class Directory
{
public:
	/**
	* @brief Resolves requests involving directory listings and index files.
	*/
	static void resolveDirectoryRequest(int socket, const Matcher &matched, const Parser &parsed);

private:
	const Matcher &matched;
	const Parser &parsed;
	int socket;
	std::string dir_path;

	Directory(int socket, const Matcher &matched, const Parser &parsed);
	void setPath();
	bool isAutoIndex();
	void resolveDirListing();
	void resolveDirIndex();
	void writeDirTitle(std::string &data);
	void writeDirFile(std::string &data, std::string &&file);
};
