/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Matcher.hpp                                        :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/11/03 01:06:13 by abobas        #+#    #+#                 */
/*   Updated: 2020/11/04 01:03:28 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Parser.hpp"
#include "../config/Json.hpp"
#include <string>

class Matcher
{
public:
	static Matcher getMatched(const Parser &parsed, Json config);
	Json getConfig() const;
	Json::object getServer() const;
	Json::object getLocation() const;
	std::string getPath() const;
	bool isMatched() const;

private:
	Json::object server;
	Json::object location;
	std::string matched_path;
	Json config;
	const Parser &parsed;
	bool match = false;

	Matcher(const Parser &parsed, Json config);
	void matchServer();
	void matchLocation();
};