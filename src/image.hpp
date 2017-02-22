/* image.hpp: image helper
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

#ifndef N_IMAGE_H_
#define N_IMAGE_H_

#include <iostream>
#include <png.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdexcept>
#include <glm/glm.hpp>

static inline unsigned char computeRGBPixel(unsigned char* data, XImage* image, int x, int y, int roffset, int goffset, int boffset, int width, glm::ivec2 offset ) {
    unsigned int real = XGetPixel(image, x, y);
    int curpixel = ((y-offset.y)*width+((x-offset.x)))*4;
    data[curpixel] = (unsigned char)((real & image->red_mask) >> roffset);
    data[curpixel+1] = (unsigned char)((real & image->green_mask) >> goffset);
    data[curpixel+2] = (unsigned char)((real & image->blue_mask) >> boffset);
    data[curpixel+3] = 255;
}
static inline unsigned char computeRGBAPixel(unsigned char* data, XImage* image, int x, int y, int roffset, int goffset, int boffset, int aoffset, int width, glm::ivec2 offset ) {
    unsigned int real = XGetPixel(image, x, y);
    int curpixel = ((y-offset.y)*width+((x-offset.x)))*4;
    data[curpixel] = (unsigned char)((real & image->red_mask) >> roffset);
    data[curpixel+1] = (unsigned char)((real & image->green_mask) >> goffset);
    data[curpixel+2] = (unsigned char)((real & image->blue_mask) >> boffset);
    data[curpixel+3] = 255;
}

static inline int get_shift (int mask) {
    int shift = 0;
    while (mask) {
        if (mask & 1) { break; }
        shift++;
        mask >>= 1;
    }
    return shift;
}

class ARGBImage {
private:
    unsigned char* data;
    unsigned int width;
    unsigned int height;
public:
    ARGBImage( XImage* image, glm::ivec2 imageloc, glm::ivec4 selectionrect );
    ~ARGBImage();
    void writePNG( std::ostream& streamout, int quality );
};

#endif
