/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Upload.hpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: abobas <abobas@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/10/23 20:28:07 by abobas        #+#    #+#                 */
/*   Updated: 2020/10/23 20:58:36 by abobas        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Data.hpp"
#include <stdio.h>
#include <fstream>

class Upload
{
public:
    Upload(Data &data);

private:
    Data data;
    bool modified = false;

    void addFile();
    int deleteFile();
    bool existingFile();
};