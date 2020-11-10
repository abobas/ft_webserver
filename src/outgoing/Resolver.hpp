/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Resolver.hpp                                       :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/11/07 11:46:44 by abobas        #+#    #+#                 */
/*   Updated: 2020/11/10 14:21:53 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Responder.hpp"
#include "Directory.hpp"
#include "Proxy.hpp"
#include "../processor/Cgi.hpp"
#include "../incoming/Evaluator.hpp"
#include "../incoming/Matcher.hpp"
#include "../incoming/Parser.hpp"
#include "../incoming/Receiver.hpp"
#include "../logger/Log.hpp"

class Proxy;

class Resolver
{
public:
	static Resolver *getInstance(int socket, Evaluator *evaluated);
	static void deleteInstance(int socket);
	void resolveProxyOutgoing(int proxy_socket);
	void resolveProxyIncoming(int proxy_socket);
	void resolveRequest();
	bool isResolved();
	
private:
	static Log *log;
	static std::map<int, Resolver *> resolvers;
	Evaluator *evaluated;
	Cgi *cgi;
	Proxy *proxy;
	int socket;
	bool resolved;
	
	Resolver(int socket, Evaluator *Evaluated);
	void resolveError(int error, Parser &parsed);
	void resolveResponse(Matcher &matched, Parser &parsed);
	void resolveProxyRequest(Matcher &matched, Parser &parsed);
	void resolveCgiRequest(Matcher &matched, Parser &parsed);
	void resolveUploadRequest(Parser &parsed);
	void resolveDirectoryRequest(Matcher &matched, Parser &parsed);
	void resolveFileRequest(Matcher &matched, Responder &respond);
};
