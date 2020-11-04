/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Evaluator.hpp                                      :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/11/03 00:54:14 by abobas        #+#    #+#                 */
/*   Updated: 2020/11/04 16:15:34 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Parser.hpp"
#include "Matcher.hpp"
#include "Validator.hpp"
#include "../logger/Log.hpp"
#include "../config/Json.hpp"
#include "../respond/Responder.hpp"
#include "../respond/Directory.hpp"
#include "../respond/Upload.hpp"
#include "../respond/Cgi.hpp"
#include "../respond/Proxy.hpp"
#include "../Socket.hpp"
#include <string>
#include <sys/stat.h>

class Evaluator
{
public:
	static Evaluator getEvaluator(int socket, std::string &message, Json config);
	Socket getProxySocket();
	std::string getProxyRequest();
	bool isProxyRequest();

private:
	static Log *log;
	int socket;
	Socket proxy_socket;
	std::string proxy_request;
	bool proxy_set = false;

	Evaluator(int socket, std::string &message, Json config);
	void evaluateRequest(Parser &parsed, Matcher &matched);
	void resolveProxyRequest(Matcher &matched, Parser &parsed);
	void resolveCgiRequest(Matcher &matched, Parser &parsed);
	void resolveUploadRequest(Matcher &matched, Parser &parsed);
	void resolveDirectoryRequest(Matcher &matched, Parser &parsed);
	void resolveFileRequest(Matcher &matched, Responder &respond);
	bool isProxy(Matcher &matched);
	bool isCgi(Matcher &matched);
	bool isUpload(Parser &parsed);
	bool isFile(Matcher &matched, struct stat *file);
	bool isRegular(struct stat *file);
	bool isDirectory(struct stat *file);
};