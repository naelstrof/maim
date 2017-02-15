#include <iostream>
#include <slop.hpp>
#include <glm/glm.hpp>

#include "x.hpp"
#include "options.hpp"
#include "image.hpp"

class MaimOptions {
public:
    MaimOptions();
    std::string savepath;
    std::string format;
    Window window;
    glm::vec4 geometry;
    float delay;
    bool hideCursor;
};

MaimOptions::MaimOptions() {
    savepath = "";
    format = "";
    window = None;
    delay = 0;
    hideCursor = false;
}

MaimOptions* getMaimOptions( Options& options ) {
    MaimOptions* foo = new MaimOptions();
    options.getWindow("window", 'i', foo->window);
    options.getGeometry("geometry", 'g', foo->geometry);
    options.getFloat("delay", 'd', foo->delay);
    options.getBool("hidecursor", 'u', foo->hideCursor);
    options.getString("format", 'f', foo->format);
    if (!options.getFloatingString(0, foo->savepath)) {
        foo->savepath = "/dev/stdout";
    }
    return foo;
}

SlopOptions* getSlopOptions( Options& options ) {
    SlopOptions* foo = new SlopOptions();
    options.getFloat("bordersize", 'b', foo->borderSize);
    options.getFloat("padding", 'p', foo->padding);
    options.getFloat("tolerance", 't', foo->tolerance);
    glm::vec4 color = glm::vec4( foo->r, foo->g, foo->b, foo->a );
    options.getColor("color", 'c', color);
    options.getBool("nokeyboard", 'k', foo->nokeyboard);
    options.getString( "xdisplay", 'x', foo->xdisplay );
    options.getString( "shader", 's', foo->shader );
    foo->r = color.r;
    foo->g = color.g;
    foo->b = color.b;
    foo->a = color.a;
    options.getBool("highlight", 'l', foo->highlight);
    options.getInt("nodecorations", 'n', foo->nodecorations);
    return foo;
}

int app( int argc, char** argv ) {
    Options options( argc, argv );
    SlopOptions* slopOptions = getSlopOptions( options );
    MaimOptions* maimOptions = getMaimOptions( options );
    bool cancelled = false;
    SlopSelection selection = SlopSelect(slopOptions, &cancelled, false);
    if ( cancelled ) {
        return 1;
    }

    X11* x11 = new X11(slopOptions->xdisplay);
    XImage* image = XGetImage( x11->display, x11->root, selection.x, selection.y, selection.w, selection.h, 0xffffffff, ZPixmap );
    RGBAImage convert(image);
    convert.writePNG(std::cout);
    delete x11;
    delete maimOptions;
    delete slopOptions;

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
