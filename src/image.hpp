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

// Not meant to be exact, all this variable does it tweak the processing chunk size.
// It's meant to cause a lot more HITS on the cache.

// Here's some testing on my ThinkPad X220, using CACHESIZE=1 as a control group.
// Theres a variance depending on width of the image. CACHESIZEs that line up
// with the image width will benefit slightly more compared to ones that
// don't.
// Once the cache size gets bigger than one of the dimensions, it loses all optimization benefits.
// Though I don't understand why it gets worse than when the cache size is set to 1, since they
// should be nearly identical operations...
// CACHESIZE=1 probably still benefits from the optimized matrix loops.

// The image size working here is 1366x768

// CACHESIZE    CEst    Speedup
// INT_MAX      10.13   ~50% speedDOWN
// 1024         10.21   ~50% speedDOWN
// 824          10.16   ~50% speedDOWN
// 712          5.77    ~21%
// 512          6.04    ~15%
// 256          5.65    ~23%
// 128          5.66    ~23%
// 64           5.59    ~25%
// 32           5.56    ~25%
// 16           5.69    ~22%
// 8            5.64    ~24%
// 4            5.78    ~21%
// 2            6.15    ~13%
// 1            6.97    0%
#define CACHESIZE (int)32

// 32 seems like a pretty safe number, padded on both sides by similar scores.
// I did test each setting multiple times, but only with my crappy laptop...

#include <iostream>
#include <png.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdexcept>
#include <glm/glm.hpp>

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
    void writePNG( std::ostream& streamout );
};

#endif
