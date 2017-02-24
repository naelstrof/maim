/* options.hpp: parses arguments from the commandline.
 *
 * Copyright (C) 2014: Dalton Nell, Maim Contributors (https://github.com/naelstrof/slop/graphs/contributors).
 *
 * This file is part of Maim.
 *
 * Maim is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Maim is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Maim.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef N_OPTIONS_H_
#define N_OPTIONS_H_

#include <iostream>
#include <string>
#include <algorithm>
#include <exception>
#include <stdexcept>
#include <vector>
#include <glm/glm.hpp>
#include <X11/Xlib.h>

class Argument {
public:
    std::string name;
    char cname;
    bool isFlagArgument;
    Argument( std::string name, char cname, bool isFlagArgument ) : name(name), cname(cname), isFlagArgument(isFlagArgument) {}
};

static std::vector<Argument> validArguments;

static unsigned int maxFloatingValues = 1;

class Options {
private:
    std::vector<std::string> arguments;
    std::vector<std::string> values;
    std::vector<std::string> floatingValues;
    int parseCharOption( int argc, char** argv, int argumentIndex, int validIndex );
    int parseStringOption( int argc, char** argv, int argumentIndex, int validIndex );
    int validateStringOption( int argc, char** argv, int argumentIndex );
    int validateCharOption( int argc, char** argv, int argumentIndex );
    void validate( int argc, char** argv );
public:
    Options( int argc, char** argv );
    bool getFloat( std::string name, char namec, float& found );
    bool getInt( std::string name, char namec, int& found );
    bool getString( std::string name, char namec, std::string& found );
    bool getColor( std::string name, char namec, glm::vec4& found );
    bool getBool( std::string name, char namec, bool& found );
    bool getGeometry( std::string name, char namec, glm::vec4& found );
    bool getWindow( std::string name, char namec, Window& found, Window root );
    bool getFloatingString( int index, std::string& found );
};

#endif // N_OPTIONS_H_
