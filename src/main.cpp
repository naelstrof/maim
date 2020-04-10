#include <cstdio>
#include <unistd.h>
#include <iostream>
#include <slop.hpp>
#include <glm/glm.hpp>
#include <fstream>
#include <sstream>
#include <thread>
#include <X11/extensions/shape.h>

#include "cxxopts.hpp"
#include "x.hpp"
#include "image.hpp"

class MaimOptions {
public:
    MaimOptions();
    std::string savepath;
    std::string format;
    Window window;
    Window parent;
    glm::vec4 geometry;
    float delay;
    int quality;
    bool select;
    bool hideCursor;
    bool geometryGiven;
    bool quiet;
    bool windowGiven;
    bool parentGiven;
    bool formatGiven;
    bool version;
    bool help;
    bool savepathGiven;
    bool captureBackground;
};

MaimOptions::MaimOptions() {
    savepath = "";
    window = None;
    parent = None;
    quality = 7;
    quiet = false;
    delay = 0;
    format = "png";
    select = false;
    parentGiven = false;
    hideCursor = false;
    geometryGiven = false;
    savepathGiven = false;
    windowGiven = false;
    formatGiven = false;
    captureBackground = false;
}

Window parseWindow( std::string win, X11* x11 ) {
    if ( win == "root" ) {
        return x11->root;
    }
    Window retwin;
    std::string::size_type sz;
    try {
        retwin = std::stoi(win,&sz);
    } catch ( ... ) {
        try {
            retwin = std::stoul(win,&sz,16);
        } catch ( ... ) {
            throw new std::invalid_argument("Unable to parse value " + win + " as a window. Expecting integer, hex, or `root`.");
        }
    }
    return retwin;
}

glm::vec4 parseColor( std::string value ) {
    std::string valuecopy = value;
    const auto check_char = [&] (char c) {
        if (c != ',') {
            throw new std::invalid_argument("Unable to parse value `" + valuecopy + "` as a color. You need to use commas as separators.");
        }
    };
    glm::vec4 found;
    std::string::size_type sz;
    try {
        found[0] = std::stof(value,&sz);
        check_char(value.at(sz));
        value = value.substr(sz+1);
        found[1] = std::stof(value,&sz);
        check_char(value.at(sz));
        value = value.substr(sz+1);
        found[2] = std::stof(value,&sz);
        if ( value.size() != sz ) {
            check_char(value.at(sz));
            value = value.substr(sz+1);
            found[3] = std::stof(value,&sz);
            if ( value.size() != sz ) {
                throw "dur";
            }
        } else {
            found[3] = 1;
        }
    } catch (std::invalid_argument* const e) {
       throw e; 
    } catch ( ... ) {
        throw new std::invalid_argument("Unable to parse value `" + valuecopy + "` as a color. Should be in the format r,g,b or r,g,b,a. Like 1,1,1,1.");
    }
    return found;
}

glm::vec4 parseGeometry( std::string value ) {
    glm::vec4 found;
    std::string valuecopy = value;
    std::string::size_type sz = 0;
    glm::vec2 dim(0,0);
    int curpos = 0;
    glm::vec2 pos(0,0);
    try {
        if ( std::count(value.begin(), value.end(), '+') > 2 ) {
            throw "dur";
        }
        if ( std::count(value.begin(), value.end(), '-') > 2 ) {
            throw "dur";
        }
        if ( std::count(value.begin(), value.end(), 'x') > 1 ) {
            throw "dur";
        }
        while( value != "" ) {
            switch( value[0] ) {
                case 'x':
                    dim.y = std::stof(value.substr(1),&sz);
                    sz++;
                    break;
                case '+':
                    pos[curpos++] = std::stof(value.substr(1), &sz);
                    sz++;
                    break;
                case '-':
                    pos[curpos++] = -std::stof(value.substr(1), &sz);
                    sz++;
                    break;
                default:
                    dim.x = std::stof(value,&sz);
                    break;
            }
            value = value.substr(sz);
        }
    } catch ( ... ) {
        throw new std::invalid_argument("Unable to parse value `" + valuecopy + "` as a geometry. Should be in the format wxh+x+y, +x+y, or wxh. Like 600x400+10+20.");
    }
    found.x = pos.x;
    found.y = pos.y;
    found.z = dim.x;
    found.w = dim.y;
    return found;
}

MaimOptions* getMaimOptions( cxxopts::Options& options, X11* x11 ) {
    MaimOptions* foo = new MaimOptions();
    foo->parentGiven = options.count("parent") > 0;
    if ( foo->parentGiven ) {
        foo->parent = parseWindow( options["parent"].as<std::string>(), x11 );
    }
    foo->windowGiven = options.count("window") > 0;
    if ( foo->windowGiven ) {
        foo->window = parseWindow( options["window"].as<std::string>(), x11 );
    }
    foo->geometryGiven = options.count("geometry") > 0;
    if ( foo->geometryGiven ) {
        foo->geometry = parseGeometry( options["geometry"].as<std::string>() );
    }
    if ( options.count( "delay" ) > 0 ) {
        foo->delay = options["delay"].as<float>();
    }
    if ( options.count( "hidecursor" ) > 0 ) {
        foo->hideCursor = options["hidecursor"].as<bool>();
    }
    if ( options.count( "select" ) > 0 ) {
        foo->select = options["select"].as<bool>();
    }
    if ( options.count( "quiet" ) > 0 ) {
        foo->quiet = options["quiet"].as<bool>();
    }
    if ( options.count( "format" ) > 0 ) {
        foo->quiet = options["quiet"].as<bool>();
    }
    foo->formatGiven = options.count("format") > 0;
    if ( foo->formatGiven ) {
        foo->format = options["format"].as<std::string>();
        if ( foo->format != "png" && foo->format != "jpg" && foo->format != "jpeg" ) {
            throw new std::invalid_argument("Unknown format type: `" + foo->format + "`, only `png` or `jpg` is allowed." );
        }
    }
    if ( options.count( "quality" ) > 0 ) {
        foo->quality = options["quality"].as<int>();
        if ( foo->quality > 10 || foo->quality < 1 ) {
            throw new std::invalid_argument("Quality argument must be between 1 and 10");
        }
    }
    if ( options.count( "capturebackground" ) > 0 ) {
        foo->captureBackground = options["capturebackground"].as<bool>();
    }
    auto& positional = options["positional"].as<std::vector<std::string>>();
    foo->savepathGiven = positional.size() > 0;
    //std::cerr << positional[0] << "\n";
    if ( foo->savepathGiven ) {
        foo->savepath = positional[0];
    }
    return foo;
}

slop::SlopOptions* getSlopOptions( cxxopts::Options& options ) {
    slop::SlopOptions* foo = new slop::SlopOptions();
    if ( options.count( "bordersize" ) > 0 ) {
        foo->border = options["bordersize"].as<float>();
    }
    if ( options.count( "padding" ) > 0 ) {
        foo->padding = options["padding"].as<float>();
    }
    if ( options.count( "tolerance" ) > 0 ) {
        foo->tolerance = options["tolerance"].as<float>();
    }
    glm::vec4 color = glm::vec4( foo->r, foo->g, foo->b, foo->a );
    if ( options.count( "color" ) > 0 ) {
        color = parseColor( options["color"].as<std::string>() );
    }
    foo->r = color.r;
    foo->g = color.g;
    foo->b = color.b;
    foo->a = color.a;
    if ( options.count( "nokeyboard" ) > 0 ) {
        foo->nokeyboard = options["nokeyboard"].as<bool>();
    }
    if ( options.count( "noopengl" ) > 0 ) {
        foo->noopengl = options["noopengl"].as<bool>();
    }
    if ( options.count( "xdisplay" ) > 0 ) {
        std::string xdisplay = options["xdisplay"].as<std::string>();
        char* cxdisplay = new char[xdisplay.length()+1];
        memcpy( cxdisplay, xdisplay.c_str(), xdisplay.length() );
        cxdisplay[xdisplay.length()]='\0';
        foo->xdisplay = cxdisplay;
    }
    if ( options.count( "shader" ) > 0 ) {
        std::string shaders = options["shader"].as<std::string>();
        char* cshaders = new char[shaders.length()+1];
        memcpy( cshaders, shaders.c_str(), shaders.length() );
        cshaders[shaders.length()]='\0';
        foo->shaders = cshaders;
    }
    if ( options.count( "quiet" ) > 0 ) {
        foo->quiet = options["quiet"].as<bool>();
    }
    if ( options.count( "highlight" ) > 0 ) {
        foo->highlight = options["highlight"].as<bool>();
    }
    if ( options.count( "nodrag" ) > 0 ) {
        foo->nodrag = options["nodrag"].as<bool>();
    }
    if ( options.count( "nodecorations" ) > 0 ) {
        foo->nodecorations = options["nodecorations"].as<int>();
        if ( foo->nodecorations < 0 || foo->nodecorations > 2 ) {
            throw new std::invalid_argument( "--nodecorations must be between 0 and 2. Or be used as a flag." );
        }
    }
    return foo;
}

const auto HELP_MESSAGE = R"MULTI_STRING(
maim - make image

SYNOPSIS
       maim [OPTIONS] [FILEPATH]

DESCRIPTION
       maim  (make image) is an utility that takes a screenshot of your desktop,
       and encodes a png or jpg image of it. By default it outputs  the  encoded
       image data directly to standard output.

OPTIONS
       -h, --help
              Print help and exit.

       -v, --version
              Print version and exit.

       -x, --xdisplay=hostname:number.screen_number
              Sets the xdisplay to use.

       -f, --format=STRING
              Sets  the  desired  output format, by default maim will attempt to
              determine the desired output format automatically from the  output
              file.  If  that  fails  it defaults to a lossless png format. Cur‐
              rently only supports `png` or `jpg`.

       -i, --window=INT
              Sets the desired window to capture, defaults to the root window.

       -g, --geometry=GEOMETRY
              Sets the region to capture, uses local coordinates from the  given
              window. So -g 10x30-5+0 would represent the rectangle wxh+x+y where
              w=10, h=30, x=-5, and y=0. x and y are the upper left location  of
              this rectangle.

       -d, --delay=FLOAT
              Sets  the  time  in  seconds  to  wait before taking a screenshot.
              Prints a simple message to show how many seconds are left before a
              screenshot is taken. See --quiet for muting this message.

       -u, --hidecursor
              By  default  maim super-imposes the cursor onto the image, you can
              disable that behavior with this flag.

       -m, --quality
              An integer from 1 to 10 that determines the compression quality. 1
              is  the  highest (and lossiest) compression available for the pro‐
              vided format. For example a setting of `1` with  png  (a  lossless
              format)  would increase filesize and speed up encoding dramatical-
              ly. While a setting of `1` on a jpeg would create a pixel mush.

       -s, --select
              Enables an interactive selection mode where  you  may  select  the
              desired region or window before a screenshot is captured. Uses the
              settings below to determine the visuals and settings of slop.

       -w, --parent=WINDOW
              By  default, maim assumes the --geometry values are in respect to
              the provided --window (or root if not provided). This parameter
              overrides this behavior by making the  geometry  be  in  respect to
              whatever window you provide to --parent. Allows for an integer,
              hex, or `root` for input.

       -B, --capturebackground
              By default, when capturing a window, maim will ignore anything
              beneath the specified window. This parameter overrides this and
              also captures elements underneath the window.

SLOP OPTIONS
       -b, --bordersize=FLOAT
              Sets the selection rectangle's thickness.

       -p, --padding=FLOAT
              Sets the padding size for the selection, this can be negative.

       -t, --tolerance=FLOAT
              How far in pixels the mouse can move after clicking, and still  be
              detected  as  a  normal click instead of a click-and-drag. Setting
              this to 0 will disable window selections. Alternatively setting it
              to 9999999 would force a window selection.


       -D, --nodrag
              Select region with two clicks instead of click and drag

       -c, --color=FLOAT,FLOAT,FLOAT,FLOAT
              Sets  the selection rectangle's color. Supports RGB or RGBA input.
              Depending on the system's window manager/OpenGL support, the opac‐
              ity may be ignored.

       -r, --shader=STRING
              This sets the vertex shader, and fragment shader combo to use when
              drawing the final framebuffer to the screen. This  obviously  only
              works  when OpenGL is enabled. The shaders are loaded from ~/.con‐
              fig/maim. See https://github.com/naelstrof/slop for more  informa‐
              tion on how to create your own shaders.

       -n, --nodecorations=INT
              Sets  the  level  of  aggressiveness  when trying to remove window
              decorations. `0' is off, `1' will try lightly  to  remove  decora‐
              tions,  and  `2' will recursively descend into the root tree until
              it gets the deepest  available  visible  child  under  the  mouse.
              Defaults to `0'.

       -l, --highlight
              Instead  of outlining a selection, maim will highlight it instead.
              This is particularly useful if the color  is  set  to  an  opacity
              lower than 1.

       -q, --quiet
              Disable  any  unnecessary cerr output. Any warnings or info simply
              won't print.

       -k, --nokeyboard
              Disables the ability to cancel selections with the keyboard.

       -o, --noopengl
              Disables graphics hardware acceleration.

EXAMPLES
       Screenshot the active window and save it to the clipboard for quick past‐
       ing.

       maim -i $(xdotool getactivewindow) | xclip -selection clipboard -t image/png

       Save a desktop screenshot with a unique ordered timestamp in the Pictures
       folder.

       maim ~/Pictures/$(date +%s).png

       Prompt for a region to screenshot. Add a fancy shadow to it, then save it
       to shadow.png.

       maim -s | convert - \( +clone -background black -shadow 80x3+5+5 \) +swap \
        -background none -layers merge +repage shadow.png)MULTI_STRING";

int app( int argc, char** argv ) {
    // Use cxxopts to parse options, we pass them into a MaimOptions and SlopOptions object so we can swap out cxxopts if it's bad or whatever.
    cxxopts::Options options("maim", "Screenshot application.");
    options.add_options()
    ("h,help", "Print help and exit.")
    ("v,version", "Print version and exit.")
    ("x,xdisplay", "Sets the xdisplay to use", cxxopts::value<std::string>())
    ("f,format", "Sets  the desired output format, by default maim will attempt to determine the desired output format automatically from the output file. If that fails it defaults to a lossless png format. Currently only supports `png` or `jpg`.", cxxopts::value<std::string>())
    ("i,window", "Sets the desired window to capture, defaults to the root window. Allows for an integer, hex, or `root` for input.", cxxopts::value<std::string>())
    ("g,geometry", "Sets the region to capture, uses local coordinates from the given window. So -g10x30-5+0 would represent the rectangle wxh+x+y where w=10, h=30, x=-5, and y=0. x and y are the upper left location of this rectangle.", cxxopts::value<std::string>())
    ("w,parent", "By default, maim assumes the --geometry values are in respect to the provided --window (or root if not provided). This parameter overrides this behavior by making the geometry be in respect to whatever window you provide to --parent. Allows for an integer, hex, or `root` for input.", cxxopts::value<std::string>())
    ("B,capturebackground", "By default, when capturing a window, maim will ignore anything beneath the specified window. This parameter overrides this and also captures elements underneath the window.")
    ("d,delay", "Sets the time in seconds to wait  before taking a screenshot. Prints a simple message to show how many seconds are left before a screenshot is taken. See --quiet for muting this message.", cxxopts::value<float>()->implicit_value("5"))
    ("u,hidecursor", "By default maim super-imposes the cursor onto the image, you can disable that behavior with this flag.")
    ("m,quality", "An integer from 1 to 10 that determines the compression quality. 1 is the highest (and lossiest) compression  available for the provided format. For example a setting of `1` with png (a loss‐ less format) would increase filesize and decrease decoding time. While a setting of `1` on a jpeg would create a pixel mush.", cxxopts::value<int>())
    ("s,select", "Enables an interactive selection mode where you may select the desired region or window before a screenshot is captured. Uses the  settings below to determine the visuals and settings of slop.")
    ("b,bordersize", "Sets the selection rectangle's thickness.", cxxopts::value<float>())
    ("p,padding", "Sets the padding size for the selection, this can be negative.", cxxopts::value<float>())
    ("t,tolerance", "How far in pixels the mouse can move after clicking, and still be detected as a normal click instead of a click-and-drag. Setting this to 0 will disable window selections. Alternatively setting it to 9999999 would force a window selection.", cxxopts::value<float>())
    ("D,nodrag", "Select region with two clicks instead of click and drag")
    ("c,color", "Sets  the  selection  rectangle's  color.  Supports  RGB or RGBA input. Depending on the system's window manager/OpenGL  support, the opacity may be ignored.", cxxopts::value<std::string>())
    ("r,shader", "This  sets  the  vertex shader, and fragment shader combo to use when drawing the final framebuffer to the screen. This obviously only  works  when OpenGL is enabled. The shaders are loaded from ~/.config/maim. See https://github.com/naelstrof/slop for more information on how to create your own shaders.", cxxopts::value<std::string>())
    ("n,nodecorations", "Sets the level of aggressiveness when trying to remove window decroations. `0' is off, `1' will try lightly to remove decorations, and `2' will recursively descend into the root tree until it gets the deepest available visible child under the mouse. Defaults to `0'.", cxxopts::value<int>()->implicit_value("1"))
    ("l,highlight", "Instead of outlining a selection, maim will highlight it instead. This is particularly useful if the color is set to an opacity lower than 1.")
    ("q,quiet", "Disable any unnecessary cerr output. Any warnings or info simply won't print.")
    ("k,nokeyboard", "Disables the ability to cancel selections with the keyboard.")
    ("o,noopengl", "Disables graphics hardware acceleration.")
    ("positional", "Positional parameters", cxxopts::value<std::vector<std::string>>())
    ;
    options.parse_positional("positional");
    options.parse(argc, argv);
    
    // Version checks and help menu don't require X11, and in fact will fail if running
    // in a headless environment.
    if ( options.count( "version" ) > 0 ) {
        std::cout << MAIM_VERSION << "\n";
        return 0;
    }
    
    if ( options.count( "help" ) > 0 ) {
        std::cout << HELP_MESSAGE << std::endl;
        return 0;
    }

    slop::SlopOptions* slopOptions = getSlopOptions( options );
    // Boot up x11
    X11* x11 = new X11(slopOptions->xdisplay);
    MaimOptions* maimOptions = getMaimOptions( options, x11 );
    slop::SlopSelection selection(0,0,0,0,0,true);

    // Check if output is a tty before dumping binary data to it
    if ( isatty( fileno( stdout ) ) && !maimOptions->savepathGiven ) {
        std::cout << "Please provide an output path or redirect stdout to a file." << std::endl;
        std::cout << "Run maim --help for more information." << std::endl;
        return 0;
    }

    if ( maimOptions->select ) {
        if ( maimOptions->windowGiven || maimOptions->parentGiven || maimOptions->geometryGiven ) {
            throw new std::invalid_argument( "Interactive mode (--select) doesn't support the following parameters: --window, --parent, --geometry." );
        }
        selection = SlopSelect(slopOptions);
        if ( selection.cancelled ) {
            if ( !maimOptions->quiet ) {
                std::cerr << "Selection was cancelled by keystroke or right-click.\n";
            }
            return 1;
        }
    }

    if ( !maimOptions->formatGiven && maimOptions->savepathGiven && maimOptions->savepath.find_last_of(".") != std::string::npos ) {
        maimOptions->format = maimOptions->savepath.substr(maimOptions->savepath.find_last_of(".")+1);
        if ( maimOptions->format != "png" && maimOptions->format != "jpg" && maimOptions->format != "jpeg" ) {
            throw new std::invalid_argument("Unknown format type: `" + maimOptions->format + "`, only `png` or `jpg` is allowed." );
        }
    }
    if ( !maimOptions->windowGiven ) {
        maimOptions->window = x11->root;
    } else {
        XWindowAttributes attr;
        XGetWindowAttributes(x11->display, maimOptions->window, &attr);
        if (attr.backing_store == NotUseful && attr.width == 1 && attr.height == 1) {
            Window root, parent;
            parent = None;
            Window* children;
            unsigned int nchildren;
            Window selectedWindow;
            XQueryTree( x11->display, maimOptions->window, &root, &parent, &children, &nchildren );
            if ( parent != None ) {
                maimOptions->window = parent;
            }
        }
    }
    if ( !maimOptions->parentGiven ) {
        maimOptions->parent = maimOptions->window;
    } else if ( !maimOptions->geometryGiven ) {
        throw new std::invalid_argument( "Relative mode (--parent) requires --geometry." );
    }
    if ( !maimOptions->geometryGiven ) {
        Window junk;
        glm::ivec4 geometry = getWindowGeometry( x11, maimOptions->window );
        XTranslateCoordinates(x11->display, x11->root, maimOptions->window, geometry.x, geometry.y, &geometry.x, &geometry.y, &junk);
        maimOptions->geometry = geometry;
    }

    if ( !maimOptions->select ) {
        selection.x = maimOptions->geometry.x;
        selection.y = maimOptions->geometry.y;
        selection.w = maimOptions->geometry.z;
        selection.h = maimOptions->geometry.w;
        selection.id = maimOptions->window;
    }

    if ( maimOptions->captureBackground ) {
        selection.id = x11->root;
    }

    std::ostream* out;
    if ( maimOptions->savepathGiven ) {
        std::ofstream* file = new std::ofstream();
        file->open(maimOptions->savepath.c_str());
        if ( !file->is_open() ) {
            throw new std::runtime_error( "Failed to open file for writing: `" + maimOptions->savepath + "`." );
        }
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
    // Localize to our parent
    int px, py;
    Window junk;
    XTranslateCoordinates( x11->display, maimOptions->parent, x11->root, (int)selection.x, (int)selection.y, &px, &py, &junk);
    glm::ivec2 imageloc;
    // Snapshot the image
    XImage* image = x11->getImage( selection.id, px, py, selection.w, selection.h, imageloc);
    if ( maimOptions->format == "png" ) {
        // Convert it to an ARGB format, clipping it to the selection.
        ARGBImage convert(image, imageloc, glm::vec4(px, py, selection.w, selection.h), 4, x11 );
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
        ARGBImage convert(image, imageloc, glm::vec4(px, py, selection.w, selection.h), 3, x11 );
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
    XDestroyImage( image );

    if ( maimOptions->savepathGiven ) {
        std::ofstream* file = (std::ofstream*)out;
        file->close();
        delete (std::ofstream*)out;
    }
    delete x11;
    delete maimOptions;
    if ( options.count( "xdisplay" ) > 0 ) {
        delete slopOptions->xdisplay;
    }
    if ( options.count( "shader" ) > 0 ) {
        delete slopOptions->shaders;
    }
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
