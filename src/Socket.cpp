/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Socket.cpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/08/26 19:00:35 by abobas        #+#    #+#                 */
/*   Updated: 2020/11/05 23:57:51 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Socket.hpp"

#define CREATED 201
#define NOT_FOUND 404
#define INTERNAL_ERROR 500
#define NOT_IMPLEMENTED 501

Log *Socket::log = Log::getInstance();
Json Socket::config = Json();

void Socket::initializeSocket(Json &config)
{
	Socket::config = config;
	Evaluator::initializeEvaluator(config);
}

Socket::Socket()
{
}

Socket::Socket(std::string type, int socket) : type(type), socket(socket)
{
}

void Socket::handleIncoming()
{
	receiver = Receiver::getInstance(socket);
	if (!receiver->headersReceived())
	{
		log->logEntry("receiving headers socket", socket);
		receiver->receiveHeaders();
	}
	if (receiver->headersReceived())
	{
		evaluator = Evaluator::getInstance(socket);
		if (!evaluator->isEvaluated())
		{
			log->logEntry("evaluating socket", socket);
			evaluator->evaluateHeaders(receiver->getHeaders());
		}
		if (evaluator->isEvaluated())
		{
			if (!evaluator->isProcessed())
			{
				log->logEntry("processing socket", socket);
				evaluator->processRequest();
			}
			if (evaluator->isProcessed())
			{
				log->logEntry("processed socket", socket);
				Receiver::deleteInstance(socket);
				setType("client_write");
				return;
			}
		}
	}
}

void Socket::handleOutgoing()
{
	if (evaluator->getError())
		errorResponse(evaluator->getError(), evaluator->getParsed());
	else
		responseControl(evaluator->getMatched(), evaluator->getParsed());
	setType("client_read");
	Evaluator::deleteInstance(socket);
	Processor::deleteInstance(socket);
}

void Socket::responseControl(Matcher &matched, Parser &parsed)
{
	Responder respond(socket, parsed);

	try
	{
		if (evaluator->getType() == "file")
			resolveFileRequest(matched, respond);
		else if (evaluator->getType() == "dir")
			resolveDirectoryRequest(matched, parsed);
		else if (evaluator->getType() == "upload")
			resolveUploadRequest(matched, parsed);
		else
			respond.sendNotImplemented();
		// else if (evaluator->getType() == "proxy")
		// 	Directory::resolveDirectoryRequest(socket, matched, parsed);
		// else if (evaluator->getType() == "cgi")
		// 	resolveCgiRequest(matched, parsed);
	}
	catch (const char *e)
	{
		log->logError(e);
		respond.sendInternalError();
	}
}

void Socket::errorResponse(int error, Parser &parsed)
{
	Responder respond(socket, parsed);
	log->logEntry("error response", error);
	if (error == NOT_FOUND)
		respond.sendNotFound();
	else if (error == INTERNAL_ERROR)
		respond.sendInternalError();
	else if (error == NOT_IMPLEMENTED)
		respond.sendNotImplemented();
}

bool Socket::isAlive()
{
	char buf[1];
	int ret = recv(socket, buf, 1, MSG_PEEK);
	if (ret < 0)
		log->logError("recv()");
	else if (ret > 0)
		return true;
	return false;
}

// void Socket::resolveProxyRequest(Matcher &matched, Parser &parsed)
// {
// 	log->logEntry("resolving proxy request");
// 	Proxy proxy = Proxy::resolveProxyRequest(matched, parsed);
// 	proxy_socket = proxy.getProxySocket();
// 	proxy_request = proxy.getProxyRequest();
// 	proxy_set = true;
// }

// void Socket::resolveCgiRequest(Matcher &matched, Parser &parsed)
// {
// 	log->logEntry("resolving CGI request");
// 	Cgi::resolveCgiRequest(socket, matched, parsed);
// }

void Socket::resolveUploadRequest(Matcher &matched, Parser &parsed)
{
	Responder respond(socket, parsed);

	if (evaluator->getStatus() == CREATED)
		respond.sendCreated(matched.getPath(), parsed.getPath());
	else
		respond.sendModified(matched.getPath(), parsed.getPath());
}

void Socket::resolveDirectoryRequest(Matcher &matched, Parser &parsed)
{
	log->logEntry("resolving directory request");
	Directory::resolveDirectoryRequest(socket, matched, parsed);
}

void Socket::resolveFileRequest(Matcher &matched, Responder &respond)
{
	log->logEntry("resolving file request");
	respond.sendFile(matched.getPath());
}

std::string Socket::getType() const
{
	return type;
}

void Socket::setType(std::string new_type)
{
	type = new_type;
}

int Socket::getSocket() const
{
	return socket;
}
