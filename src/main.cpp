#include <iostream>
#include <slop.hpp>
#include <glm/glm.hpp>
#include <fstream>
#include <X11/extensions/shape.h>

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
    int quality;
    bool select;
    bool hideCursor;
    bool geometryGiven;
    bool windowGiven;
    bool version;
    bool help;
    bool savepathGiven;
    std::string encoding;
};

MaimOptions::MaimOptions() {
    savepath = "";
    format = "";
    window = None;
    quality = 10;
    delay = 0;
    encoding = "png";
    version = false;
    help = false;
    select = false;
    hideCursor = false;
    geometryGiven = false;
    savepathGiven = false;
    windowGiven = false;
}

MaimOptions* getMaimOptions( Options& options ) {
    MaimOptions* foo = new MaimOptions();
    foo->windowGiven = options.getWindow("window", 'i', foo->window);
    foo->geometryGiven = options.getGeometry("geometry", 'g', foo->geometry);
    options.getFloat("delay", 'd', foo->delay);
    options.getBool("hidecursor", 'u', foo->hideCursor);
    options.getBool("select", 's', foo->select);
    options.getBool("version", 'v', foo->version);
    options.getBool("help", 'h', foo->help);
    options.getString("format", 'f', foo->format);
    options.getInt("quality", 'm', foo->quality);
    if ( foo->quality > 10 || foo->quality < 1 ) {
        throw new std::invalid_argument("Quality argument must be between 1 and 10");
    }
    options.getString("encoding", 'e', foo->encoding);
    if ( foo->encoding != "png" && foo->encoding != "jpg" && foo->encoding != "jpeg" ) {
        throw new std::invalid_argument("Unknown encode type: `" + foo->encoding + "`, only `png` or `jpg` is allowed." );
    }

    foo->savepathGiven = options.getFloatingString(0, foo->savepath);
    return foo;
}

slop::SlopOptions* getSlopOptions( Options& options ) {
    slop::SlopOptions* foo = new slop::SlopOptions();
    options.getFloat("bordersize", 'b', foo->borderSize);
    options.getFloat("padding", 'p', foo->padding);
    options.getFloat("tolerance", 't', foo->tolerance);
    glm::vec4 color = glm::vec4( foo->r, foo->g, foo->b, foo->a );
    options.getColor("color", 'c', color);
    options.getBool("nokeyboard", 'k', foo->nokeyboard);
    options.getString( "xdisplay", 'x', foo->xdisplay );
    options.getString( "shader", 'r', foo->shader );
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
    slop::SlopOptions* slopOptions = getSlopOptions( options );
    MaimOptions* maimOptions = getMaimOptions( options );
    if ( maimOptions->version ) {
        std::cout << MAIM_VERSION << "\n";
        return 0;
    }
    if ( maimOptions->help ) {
        std::cout << "sorry not implemented yet" << "\n";
        return 0;
    }
    bool cancelled = false;
    slop::SlopSelection selection(0,0,0,0,0);

    if ( maimOptions->geometryGiven && maimOptions->select && maimOptions->windowGiven ) {
        throw new std::invalid_argument( "You can't specify geometry, or a window ID and enable select mode at the same time!\n" );
    }

    if ( maimOptions->select ) {
        selection = SlopSelect(slopOptions, &cancelled, false);
        if ( cancelled ) {
            return 1;
        }
    }

    // Boot up x11
    X11* x11 = new X11(slopOptions->xdisplay);

    if ( !maimOptions->windowGiven ) {
        maimOptions->window = x11->root;
    }
    if ( !maimOptions->geometryGiven ) {
        maimOptions->geometry = glm::vec4( 0, 0, WidthOfScreen( x11->screen ), HeightOfScreen( x11->screen ) );
    }
    if ( !maimOptions->select ) {
        selection.x = maimOptions->geometry.x;
        selection.y = maimOptions->geometry.y;
        selection.w = maimOptions->geometry.z;
        selection.h = maimOptions->geometry.w;
        selection.id = maimOptions->window;
    }
    std::ostream* out;
    if ( maimOptions->savepathGiven ) {
        std::ofstream* file = new std::ofstream();
        file->open(maimOptions->savepath.c_str());
        out = file;
    } else {
        out = &std::cout;
    }

    XWindowAttributes attr, rattr;
    XGetWindowAttributes( x11->display, selection.id, &attr );
    // Move the selection into our local coordinates
    selection.x -= attr.x;
    selection.y -= attr.y;

    // Then we grab the pixel buffer of the provided window/selection.
    glm::ivec2 imageloc;
    XImage* image = x11->getImage( selection.id, selection.x, selection.y, selection.w, selection.h, imageloc);
    if ( maimOptions->encoding == "png" ) {
        // Convert it to an ARGB format, clipping it to the selection.
        ARGBImage convert(image, imageloc, glm::vec4(selection.x, selection.y, selection.w, selection.h), 4 );
        // Then output it in the desired format.
        convert.writePNG(*out, maimOptions->quality );
    } else if ( maimOptions->encoding == "jpg" || maimOptions->encoding == "jpeg" ) {
        // Convert it to a RGB format, clipping it to the selection.
        ARGBImage convert(image, imageloc, glm::vec4(selection.x, selection.y, selection.w, selection.h), 3 );
        // Then output it in the desired format.
        convert.writeJPEG(*out, maimOptions->quality );
    }

    if ( maimOptions->savepathGiven ) {
        std::ofstream* file = (std::ofstream*)out;
        file->close();
        delete (std::ofstream*)out;
    }
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
