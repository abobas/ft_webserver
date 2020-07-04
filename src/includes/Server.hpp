/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Server.hpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/07/04 21:23:08 by abobas        #+#    #+#                 */
/*   Updated: 2020/07/04 21:34:51 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/select.h>

class Server
{
	private:
		int listen_socket;
		int fd_max;
		fd_set read_set;
		fd_set write_set;
		std::vector<int> read_sockets;
		std::vector<int> write_sockets;
		void create_listen_socket();
		void accept_connections();
		void check_sets();
		void accept_client();
		void receive_data(int socket);
		void set_max_fd();
	public:
		Server();
		~Server();
};

#endif
