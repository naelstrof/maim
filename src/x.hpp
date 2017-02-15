/* x.hpp: initializes x11
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

#ifndef N_X_H_
#define N_X_H_

#include <iostream>
#include <X11/Xlib.h>
#include <string>
#include <stdexcept>

class X11 {
public:
    X11( std::string displayName );
    ~X11();
    Display* display;
    Visual* visual;
    Screen* screen;
    Window root;
};

#endif
