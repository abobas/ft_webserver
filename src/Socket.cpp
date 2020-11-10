/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Socket.cpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/08/26 19:00:35 by abobas        #+#    #+#                 */
/*   Updated: 2020/11/10 16:56:23 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Socket.hpp"

Log *Socket::log = Log::getInstance();
Json Socket::config = Json();
std::map<int, Socket *> Socket::sockets;

Socket::Socket(std::string type, int socket) : type(type), socket_fd(socket)
{
	proxy_pair = 0;
}

Socket::~Socket()
{
}

void Socket::initializeSocket(Json &config)
{
	Socket::config = config;
	Evaluator::initializeEvaluator(config);
}

std::map<int, Socket *> &Socket::getSockets()
{
	return sockets;
}

void Socket::createListenSockets()
{
	int new_socket;
	int port;
	int enable = 1;
	sockaddr_in new_address;

	for (auto server : config["http"]["servers"].array_items())
	{
		memset(&new_address, 0, sizeof(new_address));
		port = server["listen"].number_value();
		new_socket = socket(AF_INET, SOCK_STREAM, 0);
		if (setsockopt(new_socket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
			throw "setsockopt()";
		new_address.sin_family = AF_INET;
		new_address.sin_addr.s_addr = INADDR_ANY;
		new_address.sin_port = htons(port);
		if (bind(new_socket, reinterpret_cast<sockaddr *>(&new_address), sizeof(new_address)) < 0)
		throw "bind()";
		if (listen(new_socket, SOMAXCONN) < 0)
		throw "listen()";
		sockets.emplace(new_socket, new Socket("listen", new_socket));
		log->logEntry("created socket", new_socket);
	}
}

void Socket::acceptConnection()
{
	int client;
	struct sockaddr client_address;
	unsigned int client_address_length = 0;

	memset(&client_address, 0, sizeof(client_address));
	client = accept(socket_fd, &client_address, &client_address_length);
	if (client < 0)
	{
		log->logError("accept()");
		return;
	}
	sockets.emplace(client, new Socket("client_read", client));
	log->logEntry("accepted client", client);
}

void Socket::handleIncoming()
{
	if (!isAlive())
	{
		log->logEntry("client disconnected", socket_fd);
		deleteSocket();
		return;;
	}
	handleReceiving();
	if (receiver->headersReceived())
	{
		handleEvaluating();
		if (evaluator->isEvaluated())
			handleProcessing();
	}
}

void Socket::handleReceiving()
{
	receiver = Receiver::getInstance(socket_fd);
	if (!receiver->headersReceived())
		receiver->receiveHeaders();
}

void Socket::handleEvaluating()
{
	evaluator = Evaluator::getInstance(socket_fd);
	if (!evaluator->isEvaluated())
		evaluator->evaluateHeaders(receiver->getHeaders());
}

void Socket::handleProcessing()
{
	if (!evaluator->isProcessed())
		evaluator->processRequest();
	if (evaluator->isProcessed())
	{
		log->logEntry("processed socket", socket_fd);
		Receiver::deleteInstance(socket_fd);
		setType("client_write");
	}
}

void Socket::handleOutgoing()
{
	resolver = Resolver::getInstance(socket_fd, evaluator);
	if (!resolver->isResolved())
		resolver->resolveRequest();
	if (resolver->isResolved())
	{
		log->logEntry("resolved client", socket_fd);
		Resolver::deleteInstance(socket_fd);
		setType("client_read");
	}
}

void Socket::handleProxyIncoming()
{
	resolver->resolveProxyIncoming(socket_fd);
	deleteSocket();
}

void Socket::handleProxyOutgoing()
{
	evaluator = Evaluator::getInstance(proxy_pair);
	resolver = Resolver::getInstance(proxy_pair, evaluator);
	resolver->resolveProxyOutgoing(socket_fd);
	setType("proxy_read");
}

bool Socket::isAlive()
{
	char buf[1];
	int ret = recv(socket_fd, buf, 1, MSG_PEEK);
	if (ret < 0)
		log->logError("recv()");
	else if (ret > 0)
		return true;
	return false;
}

void Socket::deleteSocket()
{
	delete sockets[socket_fd];
	sockets.erase(socket_fd);
	close(socket_fd);
	log->logEntry("deleted socket", socket_fd);
}

std::string Socket::getType()
{
	return type;
}

void Socket::setType(std::string new_type)
{
	type = new_type;
}

int Socket::getSocket() const
{
	return socket_fd;
}

void Socket::setPair(int socket)
{
	proxy_pair = socket;
}