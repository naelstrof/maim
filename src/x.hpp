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
#include <X11/Xatom.h>
#include <X11/extensions/XShm.h>
#include <X11/extensions/Xrender.h>
#include <X11/extensions/shape.h>
#include <X11/extensions/Xcomposite.h>
#include <X11/extensions/Xrandr.h>
//#include <meta/meta-shadow-factory.h>
#include <sys/shm.h>
#include <string>
#include <vector>
#include <stdexcept>
#include <glm/glm.hpp>

class X11 {
private:
    bool hasClipping( Window d );
    XserverRegion findRegion( Window d );
    void unionClippingRegions( XserverRegion rootRegion, Window child );
    void unionBorderRegions( XserverRegion rootRegion, Window d );
    XImage* getImageUsingXRender( Window draw, int localx, int localy, int w, int h );
    XImage* getImageUsingXShm( Window draw, int localx, int localy, int w, int h );
public:
    bool haveXComposite;
    bool haveXRender;
    bool haveXShm;
    bool haveXFixes;
    bool haveXRR;
    X11( std::string displayName );
    ~X11();
    Display* display;
    Visual* visual;
    Screen* screen;
    Window root;
    XImage* getImage( Window d, int x, int y, int w, int h, glm::ivec2& imageloc );
    XRRScreenResources* res;
    std::vector<XRRCrtcInfo*> getCRTCS();
    void freeCRTCS( std::vector<XRRCrtcInfo*> monitors );
};

glm::ivec4 getWindowGeometry( X11* x11, Window win );

#endif
