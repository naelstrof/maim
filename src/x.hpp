/* x.hpp: Handles starting and managing X
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

#ifndef MAIM_X_H_
#define MAIM_X_H_

#include <X11/Xlib.h>
#include <X11/extensions/Xrandr.h>
#include <cstdlib>
#include <vector>
#include <string>
#include <cstdio>

namespace maim {

class XEngine {
public:
                        XEngine();
                        ~XEngine();
    int                 init( std::string display );
    Display*            m_display;
    Visual*             m_visual;
    Screen*             m_screen;
    Colormap            m_colormap;
    Window              m_root;
    XRRScreenResources* m_res;
    bool                m_good;
    Window              getWindowByID( int id );
    std::vector<XRRCrtcInfo*>        getCRTCS();
    void                freeCRTCS( std::vector<XRRCrtcInfo*> monitors );
};

}

extern maim::XEngine* xengine;

#endif // MAIM_X_H_
