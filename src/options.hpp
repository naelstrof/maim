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
#include <exception>
#include <stdexcept>
#include <vector>
#include <glm/glm.hpp>

static std::string validStringArguments[] = { "bordersize", "padding", "color", "shader", "highlight", "format", "tolerance", "nodecorations", "nokeyboard", "help", "xdisplay", "version" };
static char validCharArguments[] = { 'b', 'p', 'c', 's', 'l', 'f', 't', 'n', 'k', 'h', 'x', 'v' };
static unsigned int isFlagArgument[] = { false, false, false, false, true, false, false, true, true, true, false, true };
static unsigned int validArgumentCount = 12;
static unsigned int maxFloatingValues = 0;

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
};

#endif // N_OPTIONS_H_
