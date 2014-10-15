// x.hpp: handles starting and managing X

#ifndef MAIM_X_H_
#define MAIM_X_H_

#include <X11/Xlib.h>
#include <X11/extensions/Xrandr.h>
#include <vector>
#include <string>
#include <cstdio>

namespace maim {

class XEngine {
public:
                        XEngine();
                        ~XEngine();
    int                 init( std::string display );
    void                tick();
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
