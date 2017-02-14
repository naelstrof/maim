#include <iostream>
#include <CImg.h>
#include <slop.hpp>
#include "x.hpp"
#include "cimg.hpp"

using namespace cimg_library;

X11* x11;

int app( int argc, char** argv ) {
    SlopOptions selectOptions = SlopOptions();
    bool cancelled = false;
    SlopSelection selection = SlopSelect(&selectOptions, &cancelled, false);
    x11 = new X11(":0");
    XImage* image = XGetImage( x11->display, x11->root, selection.x, selection.y, selection.w, selection.h, 0xffffffff, ZPixmap );
    cimg_from_x11(image).save("work.png");
	return 0;
}

int main( int argc, char** argv ) {
    try {
        return app( argc, argv );
    } catch( std::exception* e ) {
        std::cerr << "Maim encountered an error:\n" << e->what() << "\n";
        return 1;
    } // let the operating system handle any other kind of exception.
    return 1;
}
