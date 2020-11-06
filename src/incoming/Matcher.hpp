/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Matcher.hpp                                        :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/11/03 01:06:13 by abobas        #+#    #+#                 */
/*   Updated: 2020/11/06 22:41:38 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Parser.hpp"
#include "../config/Json.hpp"
#include <string>

class Matcher
{
public:
	Matcher();
	Matcher(Parser &parsed, Json &config);
	Matcher(const Matcher &rhs);
	Matcher &operator=(const Matcher &rhs);
	Json getConfig();
	Json::object getServer();
	Json::object getLocation();
	std::string getPath();
	bool isMatched();

private:
	Json::object server;
	Json::object location;
	std::string matched_path;
	Json config;
	Parser parsed;
	bool match;

	void matchServer();
	void matchLocation();
};