/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Validator.hpp                                      :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/11/03 02:44:13 by abobas        #+#    #+#                 */
/*   Updated: 2020/11/05 23:22:52 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Parser.hpp"
#include "Matcher.hpp"
#include "../logger/Log.hpp"
#include <string>

class Validator
{
public:
	Validator();
	Validator(int socket, Parser &parsed, Matcher &matched);
	Validator(const Validator &rhs);
	Validator &operator=(const Validator &rhs);
	bool isValid();
	int getError();

private:	
	static Log *log;
	Parser parsed;
	Matcher matched;
	int socket;
	int error;
	bool valid = true;

	bool checkEmpty();
	bool checkMatch();
	bool checkProtocol();
	bool checkMethod();
	bool checkCgiExtension();
	bool checkCgiMethods();
	bool checkMaxBodyLimit();
};
