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

void help() {
std::cout << "maim - make image\n";
std::cout << "\n";
std::cout << "SYNOPSIS\n";
std::cout << "       maim [OPTIONS] [FILEPATH]\n";
std::cout << "\n";
std::cout << "DESCRIPTION\n";
std::cout << "       maim  (make image) is an utility that takes a screenshot of your desktop,\n";
std::cout << "       and encodes a png or jpg image of it. By default it outputs  the  encoded\n";
std::cout << "       image data directly to standard output.\n";
std::cout << "\n";
std::cout << "OPTIONS\n";
std::cout << "       -h, --help\n";
std::cout << "              Print help and exit.\n";
std::cout << "\n";
std::cout << "       -v, --version\n";
std::cout << "              Print version and exit.\n";
std::cout << "\n";
std::cout << "       -x, --xdisplay=hostname:number.screen_number\n";
std::cout << "              Sets the xdisplay to use.\n";
std::cout << "\n";
std::cout << "       -f, --format=STRING\n";
std::cout << "              Sets  the  desired  output format, by default maim will attempt to\n";
std::cout << "              determine the desired output format automatically from the  output\n";
std::cout << "              file.  If  that  fails  it defaults to a lossless png format. Cur‐\n";
std::cout << "              rently only supports `png` or `jpg`.\n";
std::cout << "\n";
std::cout << "       -i, --window=INT\n";
std::cout << "              Sets the desired window to capture, defaults to the root window.\n";
std::cout << "\n";
std::cout << "       -g, --geometry=GEOMETRY\n";
std::cout << "              Sets the region to capture, uses local coordinates from the  given\n";
std::cout << "              window. So -g10x30-5+0 would represent the rectangle wxh+x+y where\n";
std::cout << "              w=10, h=30, x=-5, and y=0. x and y are the upper left location  of\n";
std::cout << "              this rectangle.\n";
std::cout << "\n";
std::cout << "       -d, --delay=FLOAT\n";
std::cout << "              Sets  the  time  in  seconds  to  wait before taking a screenshot.\n";
std::cout << "              Prints a simple message to show how many seconds are left before a\n";
std::cout << "              screenshot is taken. See --quiet for muting this message.\n";
std::cout << "\n";
std::cout << "       -u, --hidecursor\n";
std::cout << "              By  default  maim super-imposes the cursor onto the image, you can\n";
std::cout << "              disable that behavior with this flag.\n";
std::cout << "\n";
std::cout << "       -m, --quality\n";
std::cout << "              An integer from 1 to 10 that determines the compression quality. 1\n";
std::cout << "              is  the  highest (and lossiest) compression available for the pro‐\n";
std::cout << "              vided format. For example a setting of `1` with  png  (a  lossless\n";
std::cout << "              format)  would increase filesize and speed up encoding dramatical-\n";
std::cout << "              ly. While a setting of `1` on a jpeg would create a pixel mush.\n";
std::cout << "\n";
std::cout << "       -s, --select\n";
std::cout << "              Enables an interactive selection mode where  you  may  select  the\n";
std::cout << "              desired region or window before a screenshot is captured. Uses the\n";
std::cout << "              settings below to determine the visuals and settings of slop.\n";
std::cout << "\n";
std::cout << "SLOP OPTIONS\n";
std::cout << "       -b, --bordersize=FLOAT\n";
std::cout << "              Sets the selection rectangle's thickness.\n";
std::cout << "\n";
std::cout << "       -p, --padding=FLOAT\n";
std::cout << "              Sets the padding size for the selection, this can be negative.\n";
std::cout << "\n";
std::cout << "       -t, --tolerance=FLOAT\n";
std::cout << "              How far in pixels the mouse can move after clicking, and still  be\n";
std::cout << "              detected  as  a  normal click instead of a click-and-drag. Setting\n";
std::cout << "              this to 0 will disable window selections. Alternatively setting it\n";
std::cout << "              to 9999999 would force a window selection.\n";
std::cout << "\n";
std::cout << "       -c, --color=FLOAT,FLOAT,FLOAT,FLOAT\n";
std::cout << "              Sets  the selection rectangle's color. Supports RGB or RGBA input.\n";
std::cout << "              Depending on the system's window manager/OpenGL support, the opac‐\n";
std::cout << "              ity may be ignored.\n";
std::cout << "\n";
std::cout << "       -r, --shader=STRING\n";
std::cout << "              This sets the vertex shader, and fragment shader combo to use when\n";
std::cout << "              drawing the final framebuffer to the screen. This  obviously  only\n";
std::cout << "              works  when OpenGL is enabled. The shaders are loaded from ~/.con‐\n";
std::cout << "              fig/maim. See https://github.com/naelstrof/slop for more  informa‐\n";
std::cout << "              tion on how to create your own shaders.\n";
std::cout << "\n";
std::cout << "       -n, --nodecorations=INT\n";
std::cout << "              Sets  the  level  of  aggressiveness  when trying to remove window\n";
std::cout << "              decroations. `0' is off, `1' will try lightly  to  remove  decora‐\n";
std::cout << "              tions,  and  `2' will recursively descend into the root tree until\n";
std::cout << "              it gets the deepest  available  visible  child  under  the  mouse.\n";
std::cout << "              Defaults to `0'.\n";
std::cout << "\n";
std::cout << "       -l, --highlight\n";
std::cout << "              Instead  of outlining a selection, maim will highlight it instead.\n";
std::cout << "              This is particularly useful if the color  is  set  to  an  opacity\n";
std::cout << "              lower than 1.\n";
std::cout << "\n";
std::cout << "       -q, --quiet\n";
std::cout << "              Disable  any  unnecessary cerr output. Any warnings or info simply\n";
std::cout << "              won't print.\n";
std::cout << "\n";
std::cout << "       -k, --nokeyboard\n";
std::cout << "              Disables the ability to cancel selections with the keyboard.\n";
std::cout << "\n";
std::cout << "EXAMPLES\n";
std::cout << "       Screenshot the active window and save it to the clipboard for quick past‐\n";
std::cout << "       ing.\n";
std::cout << "\n";
std::cout << "              maim -i$(xdotool getactivewindow) | xclip -selection clipboard -t image/png\n";
std::cout << "\n";
std::cout << "       Save a desktop screenshot with a unique ordered timestamp in the Pictures\n";
std::cout << "       folder.\n";
std::cout << "\n";
std::cout << "              maim ~/Pictures/$(date +%s).png\n";
std::cout << "\n";
std::cout << "       Prompt for a region to screenshot. Add a fancy shadow to it, then save it\n";
std::cout << "       to shadow.png.\n";
std::cout << "\n";
std::cout << "              maim -s | convert - \\( +clone -background black -shadow 80x3+5+5 \\) +swap -background none -layers merge +repage shadow.png\n";
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
        help();
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
    // localize our selection now
    glm::ivec4 sourceGeo = getWindowGeometry( x11, selection.id );
    selection.x -= sourceGeo.x;
    selection.y -= sourceGeo.y;
    if ( maimOptions->format == "png" ) {
        // Convert it to an ARGB format, clipping it to the selection.
        ARGBImage convert(image, imageloc, glm::vec4(selection.x, selection.y, selection.w, selection.h), 4, x11 );
        if ( !maimOptions->hideCursor ) {
            convert.blendCursor( x11 );
        }
        // Mask it if we're taking a picture of root
        if ( selection.id == x11->root ) {
            convert.mask(x11);
        }
        // Then output it in the desired format.
        convert.writePNG(*out, maimOptions->quality );
    } else if ( maimOptions->format == "jpg" || maimOptions->format == "jpeg" ) {
        // Convert it to a RGB format, clipping it to the selection.
        ARGBImage convert(image, imageloc, glm::vec4(selection.x, selection.y, selection.w, selection.h), 3, x11 );
        if ( !maimOptions->hideCursor ) {
            convert.blendCursor( x11 );
        }
        // Mask it if we're taking a picture of root
        if ( selection.id == x11->root ) {
            convert.mask(x11);
        }
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
