// im.hpp: handles starting and managing imlib2

#ifndef MAIM_IM_H_
#define MAIM_IM_H_

#include <Imlib2.h>

#include "x.hpp"

namespace maim {

class IMEngine {
public:
                        IMEngine();
                        ~IMEngine();
    int                 init();
    int                 screenshot( std::string file, int x, int y, int w, int h );
    int                 screenshot( std::string file );
private:
    std::string         guessFormat( std::string file );
};

}

extern maim::IMEngine* imengine;

#endif // MAIM_IM_H_
