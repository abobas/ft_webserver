/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   main.cpp                                           :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/07/04 14:09:30 by abobas        #+#    #+#                 */
/*   Updated: 2020/07/04 16:46:29 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "includes/Socket.hpp"
#include <iostream>

int main()
{
    try
    {
        Socket server;
        server.accept_connections();
    }
    catch(const char *e)
    {
        std::cerr << e << std::endl;
    }
}
