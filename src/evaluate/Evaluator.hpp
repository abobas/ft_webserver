/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Evaluator.hpp                                      :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/11/03 00:54:14 by abobas        #+#    #+#                 */
/*   Updated: 2020/11/04 12:07:59 by abobas        ########   odam.nl         */
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
	void evaluateRequest(const Parser &parsed, const Matcher &matched);
	void resolveProxyRequest(const Matcher &matched, const Parser &parsed);
	void resolveCgiRequest(const Matcher &matched, const Parser &parsed);
	void resolveUploadRequest(const Matcher &matched, const Parser &parsed);
	void resolveDirectoryRequest(const Matcher &matched, const Parser &parsed);
	void resolveFileRequest(const Matcher &matched);
	bool isProxy(const Matcher &matched);
	bool isCgi(const Matcher &matched);
	bool isUpload(const Parser &parsed);
	bool isFile(const Matcher &matched, struct stat *file);
	bool isRegular(struct stat *file);
	bool isDirectory(struct stat *file);
};