/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Validator.hpp                                      :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/11/03 02:44:13 by abobas        #+#    #+#                 */
/*   Updated: 2020/11/11 15:16:02 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Parser.hpp"
#include "Matcher.hpp"
#include "../logger/Log.hpp"
#include <string>
#include <ctype.h>

class Validator
{
public:
	Validator();
	Validator(int socket, Parser &parsed, Matcher &matched);
	Validator(const Validator &rhs);
	Validator &operator=(const Validator &rhs);
	bool isValid();
	int getError();
	std::string getMethods();

private:
	static Log *log;
	Parser parsed;
	Matcher matched;
	std::string methods;
	int socket;
	int error;
	bool valid;

	bool returnError(const char *message, int err);
	bool checkEmpty();
	bool checkMatch();
	bool checkProtocol();
	bool checkMethod();
	bool checkCgiExtension();
	bool checkCgiMethods();
	bool checkContentLength();
	bool checkMaxUri();
	bool checkMaxHeaderSize();
	bool checkHeaders();
};
