/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Validator.hpp                                      :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/11/03 02:44:13 by abobas        #+#    #+#                 */
/*   Updated: 2020/11/03 13:43:39 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Parser.hpp"
#include "Matcher.hpp"
#include "../logger/Log.hpp"
#include "../config/Config.hpp"
#include "../config/Json.hpp"
#include "../respond/Responder.hpp"
#include <string>

class Validator
{
public:
	static const Validator &getValidated(int socket, const Parser &parsed, const Matcher &matched);
	bool isValid() const;

private:
	static Json config;
	static Log *log;
	int socket;
	bool valid = true;

	Validator(int socket, const Parser &parsed, const Matcher &matched);
	bool checkEmpty(const Parser &parsed);
	bool checkMatch(const Matcher &matched);
	bool checkProtocol(const Parser &parsed);
	bool checkMethod(const Parser &parsed, const Matcher &matched);
	bool checkCgiExtension(const Parser &parsed);
	bool checkCgiMethods(const Parser &parsed);
};
