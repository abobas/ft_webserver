/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Matcher.hpp                                        :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/11/03 01:06:13 by abobas        #+#    #+#                 */
/*   Updated: 2020/11/03 15:18:54 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Parser.hpp"
#include "../config/Config.hpp"
#include "../config/Json.hpp"
#include <string>

class Matcher
{
public:

	static const Matcher &getMatched(const Parser &parsed);
	Json::object getServer() const;
	Json::object getLocation() const;
	std::string getPath() const;
	bool isMatched() const;

private:
	Json::object server;
	Json::object location;
	std::string matched_path;
	static Json config;
	bool no_match = false;

	Matcher(const Parser &parsed);
	void matchServer(const Parser &parsed);
	void matchLocation(const Parser &parsed);
};