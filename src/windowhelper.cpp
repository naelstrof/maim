#include "windowhelper.hpp"

glm::vec4 getWindowGeometry( X11* x11, Window win ) {
    XWindowAttributes attr;         
    XGetWindowAttributes( x11->display, win, &attr );
    unsigned int width = attr.width;           
    unsigned int height = attr.height;         
    unsigned int border = attr.border_width;   
    int x, y;
    Window junk;
    XTranslateCoordinates( x11->display, win, attr.root, -attr.border_width, -attr.border_width, &x, &y, &junk );
    width += border*2;
    height += border*2;
    return glm::vec4( x, y, width, height );
}
