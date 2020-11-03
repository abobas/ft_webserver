/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Evaluator.hpp                                      :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/11/03 00:54:14 by abobas        #+#    #+#                 */
/*   Updated: 2020/11/03 16:31:39 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Parser.hpp"
#include "Matcher.hpp"
#include "Validator.hpp"
#include "../logger/Log.hpp"
#include "../config/Config.hpp"
#include "../config/Json.hpp"
#include "../respond/Responder.hpp"
#include "../respond/Directory.hpp"
#include "../respond/Upload.hpp"
#include <string>
#include <sys/stat.h>

class Evaluator
{
public:
	static Evaluator getEvaluator(int socket, std::string &message);

private:
	static Log *log;
	static Json config;
	int socket;
	
	Evaluator(int socket, std::string &message);
	void evaluateRequest(const Parser &parsed, const Matcher &matched);

	void resolveProxyRequest();
	void resolveCgiRequest();
	void resolveUploadRequest(const Matcher &matched, const Parser &parsed);
	void resolveDirectoryRequest(const Matcher &matched, const Parser &parsed);
	void resolveFileRequest(const Matcher &matched);

	bool isProxy(const Matcher &matched);
	bool isCgi(const Parser &parsed);
	bool isUpload(const Parser &parsed);
	bool isFile(const Matcher &matched, struct stat *file);
	bool isRegular(struct stat *file);
	bool isDirectory(struct stat *file);

};