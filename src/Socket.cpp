/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Socket.cpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/07/04 14:18:19 by abobas        #+#    #+#                 */
/*   Updated: 2020/07/04 18:17:09 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "includes/Socket.hpp"
#include <iostream>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#define PORT 80

Socket::Socket()
{
    this->setup_main_socket();
}

/*  
    setup_main_socket() creates the main socket which will listen on specified port(s)
    for incoming requests and handles connections by opening new sockets
*/

void Socket::setup_main_socket()
{
    if ((this->main_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        throw strerror(errno);
    else
        std::cout << "Socket succesfully created" << std::endl;
    sockaddr_in main_address;
    main_address.sin_family = AF_INET; 
    main_address.sin_addr.s_addr = INADDR_ANY;                               // no need to convert to big-endian because all 0 bytes
    main_address.sin_port = ((PORT & 0xFF) << 8);                            // converted to big-endian
    for (uint32_t i = 0; i < sizeof(main_address.sin_zero); i++)
        main_address.sin_zero[i] = 0;
    if (bind(this->main_socket, reinterpret_cast<sockaddr*>(&main_address), sizeof(main_address)) < 0) 
        throw strerror(errno);
    else
        std::cout << "Socket was succesfully assigned an address" << std::endl;
    if (listen(this->main_socket, SOMAXCONN) < 0) 
        throw strerror(errno);
    else
        std::cout << "Socket is now listening to incoming connections" << std::endl;
}

void Socket::accept_connections()
{
    int new_socket;
    struct sockaddr new_address;
    unsigned int new_address_length;
    if ((new_socket = accept(this->main_socket, &new_address, &new_address_length)) < 0)
            throw strerror(errno);
    else
        std::cout << "Incoming connection accepted" << std::endl;
    this->connection.push_back(new_socket);
    this->receive_data();
}

void Socket::receive_data()
{
    char buf[6000];
    int ret = read(this->connection[0], buf, 6000);
    buf[ret] = '\0';
    std::cout << buf << std::endl;
}

Socket::~Socket()
{
    close(this->main_socket);
    close(this->connection[0]);
}


// https://stackoverflow.com/questions/36262070/what-does-htons-do-on-a-big-endian-system
