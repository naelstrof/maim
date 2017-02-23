#include <iostream>
#include <slop.hpp>
#include <glm/glm.hpp>
#include <fstream>
#include <thread>
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
    bool quiet;
    bool windowGiven;
    bool formatGiven;
    bool version;
    bool help;
    bool savepathGiven;
};

MaimOptions::MaimOptions() {
    savepath = "";
    window = None;
    quality = 10;
    quiet = false;
    delay = 0;
    format = "png";
    version = false;
    help = false;
    select = false;
    hideCursor = false;
    geometryGiven = false;
    savepathGiven = false;
    windowGiven = false;
    formatGiven = false;
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
    options.getBool("quiet", 'q', foo->quiet);
    options.getString("format", 'f', foo->format);
    if ( foo->format != "png" && foo->format != "jpg" && foo->format != "jpeg" ) {
        throw new std::invalid_argument("Unknown format type: `" + foo->format + "`, only `png` or `jpg` is allowed." );
    }
    options.getInt("quality", 'm', foo->quality);
    if ( foo->quality > 10 || foo->quality < 1 ) {
        throw new std::invalid_argument("Quality argument must be between 1 and 10");
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
        selection = SlopSelect(slopOptions, &cancelled, maimOptions->quiet);
        if ( cancelled ) {
            if ( !maimOptions->quiet ) {
                std::cerr << "Selection was cancelled by keystroke or right-click.\n";
            }
            return 1;
        }
    }

    // Boot up x11
    X11* x11 = new X11(slopOptions->xdisplay);

    if ( !maimOptions->formatGiven && maimOptions->savepathGiven && maimOptions->savepath.find_last_of(".") != std::string::npos ) {
        maimOptions->format = maimOptions->savepath.substr(maimOptions->savepath.find_last_of(".")+1);
        if ( maimOptions->format != "png" && maimOptions->format != "jpg" && maimOptions->format != "jpeg" ) {
            throw new std::invalid_argument("Unknown format type: `" + maimOptions->format + "`, only `png` or `jpg` is allowed." );
        }
    }
    if ( !maimOptions->windowGiven ) {
        maimOptions->window = x11->root;
    }

    if ( !maimOptions->geometryGiven ) {
        maimOptions->geometry = getWindowGeometry( x11, maimOptions->window );
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
    if ( maimOptions->delay ) {
        if ( !maimOptions->quiet ) {
            std::cerr << "Snapshotting in...";
        }
        while ( maimOptions->delay > 0 ) {
            std::this_thread::sleep_for(std::chrono::milliseconds(glm::clamp(glm::min(1000,(int)(maimOptions->delay*1000)),0,1000)));
            maimOptions->delay-=1;
            if ( !maimOptions->quiet ) {
                if ( maimOptions->delay <= 0 ) {
                    std::cerr << "☺";
                } else {
                    std::cerr << maimOptions->delay << " ";
                }
            }
        }
        if ( !maimOptions->quiet ) {
            std::cerr << "\n";
        }
    }
    glm::ivec2 imageloc;
    XImage* image = x11->getImage( selection.id, selection.x, selection.y, selection.w, selection.h, imageloc);
    if ( maimOptions->format == "png" ) {
        // Convert it to an ARGB format, clipping it to the selection.
        ARGBImage convert(image, imageloc, glm::vec4(selection.x, selection.y, selection.w, selection.h), 4 );
        // Then output it in the desired format.
        convert.writePNG(*out, maimOptions->quality );
    } else if ( maimOptions->format == "jpg" || maimOptions->format == "jpeg" ) {
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
