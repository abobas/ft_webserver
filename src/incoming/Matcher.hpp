/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Matcher.hpp                                        :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/11/03 01:06:13 by abobas        #+#    #+#                 */
/*   Updated: 2020/11/05 00:49:09 by abobas        ########   odam.nl         */
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
	void operator=(const Matcher &other);
	Json getConfig();
	Json::object getServer();
	Json::object getLocation();
	std::string getPath();
	bool isMatched();

private:
	Json::object server;
	Json::object location;
	std::string matched_path;
	Json &config;
	Parser &parsed;
	bool match = false;

	void matchServer();
	void matchLocation();
};