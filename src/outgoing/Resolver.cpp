/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Resolver.cpp                                       :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/11/07 11:46:41 by abobas        #+#    #+#                 */
/*   Updated: 2020/11/08 00:07:12 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Resolver.hpp"

#define CREATED 201
#define NOT_FOUND 404
#define METHOD_NOT_ALLOWED 405
#define PAYLOAD_TOO_LARGE 413
#define INTERNAL_ERROR 500
#define NOT_IMPLEMENTED 501

Log *Resolver::log = Log::getInstance();
std::map<int, Resolver *> Resolver::resolvers;

Resolver::Resolver(int socket, Evaluator *evaluated)
 : evaluated(evaluated), socket(socket)
{
	resolved = false;
}

Resolver *Resolver::getInstance(int socket, Evaluator *evaluated)
{
	if (!resolvers[socket])
	{
		resolvers[socket] = new Resolver(socket, evaluated);
		log->logEntry("created resolver", socket);
	}
	return resolvers[socket];
}

void Resolver::deleteInstance(int socket)
{
	if (resolvers[socket])
	{
		delete resolvers[socket];
		resolvers.erase(socket);
		log->logEntry("deleted resolver", socket);
		Evaluator::deleteInstance(socket);
	}
}

void Resolver::resolveRequest()
{
	if (evaluated->getError())
	{
		resolveError(evaluated->getError(), evaluated->getParsed());
		resolved = true;
		return;
	}
	resolveResponse(evaluated->getMatched(), evaluated->getParsed());
}

void Resolver::resolveResponse(Matcher &matched, Parser &parsed)
{
	Responder respond(socket, parsed);

	try
	{
		if (evaluated->getType() == "file")
			resolveFileRequest(matched, respond);
		else if (evaluated->getType() == "dir")
			resolveDirectoryRequest(matched, parsed);
		else if (evaluated->getType() == "upload")
			resolveUploadRequest(parsed);
		else if (evaluated->getType() == "cgi")
			resolveCgiRequest(matched, parsed);
		// else if (evaluated->getType() == "proxy")
		// 	resolveProxyRequest(matched, parsed);
		else
		{
			respond.sendNotImplemented();
			resolved = true;
		}
	}
	catch (const char *e)
	{
		log->logError(e);
		respond.sendInternalError();
	}
}

void Resolver::resolveError(int error, Parser &parsed)
{
	Responder respond(socket, parsed);
	
	log->logEntry("error response", error);
	if (error == NOT_FOUND)
		respond.sendNotFound();
	else if (error == INTERNAL_ERROR)
		respond.sendInternalError();
	else if (error == NOT_IMPLEMENTED)
		respond.sendNotImplemented();
	else if (error == PAYLOAD_TOO_LARGE)
		respond.sendPayLoadTooLarge();
	else if (error == METHOD_NOT_ALLOWED)
		respond.sendBadMethod(evaluated->getValidMethods());
}

// void Resolver::resolveProxyRequest(Matcher &matched, Parser &parsed)
// {
// 	return;
// }

void Resolver::resolveCgiRequest(Matcher &matched, Parser &parsed)
{
	log->logEntry("resolving CGI request");
	cgi = Cgi::getInstance(socket, parsed, matched);
	if (!cgi->isResolved())
		cgi->resolveCgiRequest();
	if (cgi->isResolved())
		resolved = true;
}

void Resolver::resolveUploadRequest(Parser &parsed)
{
	Responder respond(socket, parsed);
	log->logEntry("resolving upload request");
	if (evaluated->getUploadStatus() == CREATED)
		respond.sendCreated(evaluated->getUploadPath(), parsed.getPath());
	else
		respond.sendModified(evaluated->getUploadPath(), parsed.getPath());
	resolved = true;
}

void Resolver::resolveDirectoryRequest(Matcher &matched, Parser &parsed)
{
	log->logEntry("resolving directory request");
	Directory::resolveDirectoryRequest(socket, matched, parsed);
	resolved = true;
}

void Resolver::resolveFileRequest(Matcher &matched, Responder &respond)
{
	log->logEntry("resolving file request");
	respond.sendFile(matched.getPath());
	resolved = true;
}

// void Socket::resolveProxyRequest(Matcher &matched, Parser &parsed)
// {
// 	log->logEntry("resolving proxy request");
// 	Proxy proxy = Proxy::resolveProxyRequest(matched, parsed);
// 	proxy_socket = proxy.getProxySocket();
// 	proxy_request = proxy.getProxyRequest();
// 	proxy_set = true;
// }


bool Resolver::isResolved()
{
	return resolved;
}