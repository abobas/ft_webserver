/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Socket.hpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/07/04 14:11:00 by abobas        #+#    #+#                 */
/*   Updated: 2020/07/04 18:11:35 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#ifndef SOCKET_HPP
#define SOCKET_HPP

#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>

class Socket
{
    private:
        int main_socket;
        std::vector<int> connection;
        void setup_main_socket();
    public:
        Socket();
        void accept_connections();
        void receive_data();
        ~Socket();
};

#endif
