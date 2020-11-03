/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Config.hpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/11/03 02:12:45 by abobas        #+#    #+#                 */
/*   Updated: 2020/11/03 02:26:19 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Json.hpp"
#include <string>
#include <fstream>

class Config
{
public:
	static void initializeConfig(const char *argument);
	static Json getConfig();

private:
	static Json config;
	Config();
};
