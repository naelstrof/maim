// im.hpp: handles starting and managing imlib2

#ifndef MAIM_IM_H_
#define MAIM_IM_H_

#include <Imlib2.h>
#include <X11/extensions/Xfixes.h>
#include <X11/extensions/Xrandr.h>
#include <vector>

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
