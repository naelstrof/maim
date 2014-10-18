/* im.hpp: Handles starting and managing imlib2.
 *
 * Copyright (C) 2014: Dalton Nell, Maim Contributors (https://github.com/naelstrof/maim/graphs/contributors).
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

#ifndef MAIM_IM_H_
#define MAIM_IM_H_

#include <Imlib2.h>
#include <X11/extensions/Xfixes.h>
#include <X11/extensions/Xrandr.h>
#include <vector>
#include <stdlib.h>

#include "x.hpp"

namespace maim {

class IMEngine {
public:
                        IMEngine();
                        ~IMEngine();
    int                 init();
    int                 screenshot( std::string file, int x, int y, int w, int h, bool hidecursor, Window id, bool mask );
    int                 screenshot( std::string file, bool hidecursor, Window id, bool mask );
private:
    void                blendCursor();
};

}

extern maim::IMEngine* imengine;

#endif // MAIM_IM_H_
