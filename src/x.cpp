#include "x.hpp"

X11::X11( std::string displayName ) {
    // Initialize display
    display = XOpenDisplay( displayName.c_str() );
    if ( !display ) {
        throw new std::runtime_error(std::string("Error: Failed to open X display: ") + displayName );
    }
    screen = ScreenOfDisplay( display, DefaultScreen( display ) );
    visual = DefaultVisual( display, XScreenNumberOfScreen( screen ) );
    root = DefaultRootWindow( display );
}

X11::~X11() {
    XCloseDisplay( display );
}
