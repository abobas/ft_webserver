/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Validator.hpp                                      :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/11/03 02:44:13 by abobas        #+#    #+#                 */
/*   Updated: 2020/11/04 21:40:18 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Parser.hpp"
#include "Matcher.hpp"
#include "../logger/Log.hpp"
#include "../respond/Responder.hpp"
#include <string>

class Validator
{
public:
	static Validator getValidated(int socket, Parser &parsed, Matcher &matched);
	bool isValid() const;

private:
	static Log *log;
	Parser &parsed;
	Matcher &matched;
	Responder respond;
	int socket;
	bool valid = true;

	Validator(int socket, Parser &parsed, Matcher &matched);
	bool checkEmpty();
	bool checkMatch();
	bool checkProtocol();
	bool checkMethod();
	bool checkMaxBodyLimit();
	bool checkCgiExtension();
	bool checkCgiMethods();
};
