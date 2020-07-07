/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Server.hpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/07/04 21:23:08 by abobas        #+#    #+#                 */
/*   Updated: 2020/07/07 22:24:25 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include <vector>
#include <map>
#include <string>
#include <sys/select.h>

class Server
{
	private:
		int select_value;
		fd_set read_set;
		fd_set write_set;
		std::vector<int> server_sockets;
		std::vector<int> client_sockets_r;
		std::vector<int> client_sockets_w;
		std::map<int, std::string> requests;
		void createServerSocket();
		void initializeSets();
		int getSocketRange();
		void handleConnections();
		void acceptClient(int server_socket);
		void disconnectClient(int client_socket);
		void transformClientToWrite(int client_socket);
		void transformClientToRead(int client_socket);
		void receiveRequest(int client_socket);
		void sendResponse(int client_socket);
		void closeSockets();
	public:
		Server();
		void acceptConnections();
		~Server();
};

#endif
