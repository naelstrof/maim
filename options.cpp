#include "options.hpp"

maim::Options* options = new maim::Options();

maim::Options::Options() {
    m_version = "v1.1.9";
    m_xdisplay = ":0";
    m_file = "";
    m_select = false;
    m_hidecursor = false;
    m_gotGeometry = false;
    m_gotFile = false;
    m_x = 0;
    m_y = 0;
    m_w = 0;
    m_h = 0;
    m_delay = 0;
    // We set this to the root window later. See maim::Options::parseOptions()
    m_window = 0;
}

void maim::Options::printHelp() {
    printf( "Usage: maim [options] [file]\n" );
    printf( "maim (make image) makes an image out of the given area on the given X screen. Defaults to the whole screen.\n" );
    printf( "\n" );
    printf( "options\n" );
    printf( "    -h, --help                         Show this message.\n" );
    printf( "    -s, --select                       Manually select screenshot location. Only works when slop is installed.\n" );
    printf( "    -xd=STRING, --xdisplay=STRING      Set x display (STRING must be hostname:number.screen_number format)\n" );
    printf( "    -g=GEOMETRY, --geometry=GEOMETRY   Set the region to capture. GEOMETRY is in format WxH+X+Y\n" );
    printf( "    -x=INT                             Set the x coordinate for taking an image\n" );
    printf( "    -y=INT                             Set the y coordinate for taking an image\n" );
    printf( "    -w=INT                             Set the width for taking an image\n" );
    printf( "    -h=INT                             Set the height for taking an image\n" );
    printf( "    -d=FLOAT, --delay=FLOAT            Set the amount of time to wait before taking an image.\n" );
    printf( "    -id=INT, --windowid=INT            Set the window id to take a picture of, defaults to the root window id.\n" );
    printf( "    -hc, --hidecursor                  Prevent the system cursor from appearing in the screenshot.\n" );
    printf( "    -v, --version                      Prints version.\n" );
    printf( "\n" );
    printf( "slop options\n" );
    printf( "    -nkb, --nokeyboard                 Disables the ability to cancel selections with the keyboard.\n" );
    printf( "    -b=INT, --bordersize=INT           Set selection rectangle border size.\n" );
    printf( "    -p=INT, --padding=INT              Set padding size for selection.\n" );
    printf( "    -t=INT, --tolerance=INT            How far in pixels the mouse can move after clicking and still be detected\n" );
    printf( "                                       as a normal click. Setting to zero will disable window selections.\n" );
    printf( "    -c=COLOR, --color=COLOR            Set selection rectangle color, COLOR is in format FLOAT,FLOAT,FLOAT,FLOAT.\n" );
    printf( "                                       takes RGBA or RGB.\n" );
    printf( "    -w=FLOAT, --gracetime=FLOAT        Set the amount of time before slop will check for keyboard cancellations\n" );
    printf( "                                       in seconds.\n" );
    printf( "    -nd, --nodecorations               Attempts to remove decorations from window selections.\n" );
    printf( "    -min=INT, --minimumsize=INT        Sets the minimum output of width or height values, useful to avoid outputting 0\n" );
    printf( "                                       widths or heights.\n" );
    printf( "    -max=INT, --maximumsize=INT        Sets the maximum output of width or height values.\n" );
    printf( "    -hi, --highlight                   Instead of outlining the selection, slop highlights it. Only useful when\n" );
    printf( "                                       used with a --color with an alpha under 1.\n" );
    printf( "examples\n" );
    printf( "    $ # Screenshot the active window\n" );
    printf( "    $ maim -id=$(xdotool getactivewindow)\n" );
    printf( "\n" );
    printf( "    $ # Prompt a transparent red selection to screenshot.\n" );
    printf( "    $ maim -s -c=1,0,0,0.6\n" );
    printf( "\n" );
    printf( "    $ # Save a dated screenshot.\n" );
    printf( "    $ maim \"~/$(date +%%F-%%T).png\"\n" );
}

int maim::Options::parseOptions( int argc, char** argv ) {
    // Simple command parsing. Just uses sscanf to read each argument.
    // It looks complicated because you have to have spaces for delimiters for sscanf.
    m_window = xengine->m_root;
    for ( int i=1; i<argc; i++ ) {
        std::string arg = argv[i];
        if ( matches( arg, "-xd=", "--xdisplay=" ) ) {
            int err = parseString( arg, &m_xdisplay );
            if ( err ) {
                return 1;
            }
        } else if ( matches( arg, "-g=", "--geometry=" ) ) {
            m_gotGeometry = true;
            int err = parseGeometry( arg, &m_x, &m_y, &m_w, &m_h );
            if ( err ) {
                return 1;
            }
        } else if ( matches( arg, "-id=", "--windowid=" ) ) {
            int test = 0;
            int err = parseInt( arg, &test );
            if ( err ) {
                return 1;
            }
            m_window = xengine->getWindowByID( test );
        } else if ( matches( arg, "-nkb", "--nokeyboard" ) ) {
            m_slopoptions += arg + " ";
        } else if ( matches( arg, "-c=", "--color=" ) ) {
            m_slopoptions += arg + " ";
        } else if ( matches( arg, "-b=", "--bordersize=" ) ) {
            m_slopoptions += arg + " ";
        } else if ( matches( arg, "-w=", "--gracetime=" ) ) {
            // Replace w with g, since slop uses g as gracetime.
            for ( int i=0;i<arg.length();i++ ) {
                if ( arg[i] == 'w' ) {
                    arg[i] = 'g';
                }
            }
            m_slopoptions += arg + " ";
        } else if ( matches( arg, "-p=", "--padding=" ) ) {
            m_slopoptions += arg + " ";
        } else if ( matches( arg, "-t=", "--tolerance=" ) ) {
            m_slopoptions += arg + " ";
        } else if ( matches( arg, "-min=", "--minimumsize=" ) ) {
            m_slopoptions += arg + " ";
        } else if ( matches( arg, "-max=", "--maximumsize=" ) ) {
            m_slopoptions += arg + " ";
        } else if ( matches( arg, "-hi", "--highlight" ) ) {
            m_slopoptions += arg + " ";
        } else if ( matches( arg, "-nd", "--nodecorations" ) ) {
            m_slopoptions += arg + " ";
        } else if ( matches( arg, "-x=" ) ) {
            m_gotGeometry = true;
            int err = parseInt( arg, &m_x );
            if ( err ) {
                return 1;
            }
        } else if ( matches( arg, "-hc", "--hidecursor" ) ) {
            m_hidecursor = true;
        } else if ( matches( arg, "-d=", "--delay=" ) ) {
            int err = parseFloat( arg, &m_delay );
            if ( err ) {
                return 1;
            }
        } else if ( matches( arg, "-y=" ) ) {
            m_gotGeometry = true;
            int err = parseInt( arg, &m_y );
            if ( err ) {
                return 1;
            }
        } else if ( matches( arg, "-w=" ) ) {
            m_gotGeometry = true;
            int err = parseInt( arg, &m_w );
            if ( err ) {
                return 1;
            }
        } else if ( matches( arg, "-h=" ) ) {
            m_gotGeometry = true;
            int err = parseInt( arg, &m_h );
            if ( err ) {
                return 1;
            }
        } else if ( arg ==  "-s" || arg ==  "--select" ) {
            m_select = true;
        } else if ( arg ==  "-h" || arg ==  "--help" ) {
            printHelp();
            return 2;
        } else if ( matches( arg, "-v", "--version" ) ) {
            printf( "maim %s\n", m_version.c_str() );
            return 2;
        } else {
            // If nothing matched, and we're at the end of the argument list. Parse the last arg as the file name.
            if ( i == argc-1 ) {
                // We're responsible for expanding ~, so we use wordexp to do everything for us.
                wordexp_t p;
                wordexp( argv[i], &p, 0 );
                if ( p.we_wordc > 1 ) {
                    fprintf( stderr, "Error: File name expanded into more than one name!\n" );
                    fprintf( stderr, "This is not intended or expected behavior. (We don't want to overwrite 30 files with a single image.)\n" );
                    fprintf( stderr, "Please use a filename that doesn't contain a * or similar wildcards. (Or escape them with \\.)\n" );
                    return 1;
                }
                m_file = p.we_wordv[0];
                wordfree( &p );
                m_gotFile = true;
                return 0;
            }
            fprintf( stderr, "Error: Unknown argument %s\n", argv[i] );
            fprintf( stderr, "Try -h or --help for help.\n" );
            return 1;
        }
    }
    if ( m_gotGeometry ) {
        if ( m_w <= 0 ) {
            fprintf( stderr, "Error: width can't be less than zero!\n" );
            return 1;
        }
        if ( m_h <= 0 ) {
            fprintf( stderr, "Error: height can't be less than zero!\n" );
            return 1;
        }
    }
    m_slopoptions += "--xdisplay=" + m_xdisplay;
    return 0;
}

int maim::Options::parseInt( std::string arg, int* returnInt ) {
    std::string copy = arg;
    int find = copy.find( "=" );
    if ( find != copy.npos ) {
        copy.at( find ) = ' ';
    }
    // Just in case we error out, grab the actual argument name into x.
    char* x = new char[ arg.size() ];
    int num = sscanf( copy.c_str(), "%s %i", x, returnInt );
    if ( num != 2 ) {
        fprintf( stderr, "Error parsing command arguments near %s\n", arg.c_str() );
        fprintf( stderr, "Usage: %s=INT\n", x );
        fprintf( stderr, "Example: %s=10 or %s=-12\n", x, x );
        fprintf( stderr, "Try -h or --help for help.\n" );
        delete[] x;
        return 1;
    }
    delete[] x;
    return 0;
}

int maim::Options::parseFloat( std::string arg, float* returnFloat ) {
    std::string copy = arg;
    int find = copy.find( "=" );
    if ( find != copy.npos ) {
        copy.at( find ) = ' ';
    }
    // Just in case we error out, grab the actual argument name into x.
    char* x = new char[ arg.size() ];
    int num = sscanf( copy.c_str(), "%s %f", x, returnFloat );
    if ( num != 2 ) {
        fprintf( stderr, "Error parsing command arguments near %s\n", arg.c_str() );
        fprintf( stderr, "Usage: %s=FLOAT\n", x );
        fprintf( stderr, "Example: %s=3.14 or %s=-99\n", x, x );
        fprintf( stderr, "Try -h or --help for help.\n" );
        delete[] x;
        return 1;
    }
    delete[] x;
    return 0;
}

bool maim::Options::matches( std::string arg, std::string shorthand, std::string longhand ) {
    if ( arg.substr( 0, shorthand.size() ) == shorthand ) {
        if ( arg == shorthand || shorthand[shorthand.length()-1] == '=' ) {
            return true;
        }
    }
    if ( longhand.size() && arg.substr( 0, longhand.size() ) == longhand ) {
        if ( arg == longhand || longhand[longhand.length()-1] == '=' ) {
            return true;
        }
    }
    return false;
}

bool maim::Options::matches( std::string arg, std::string shorthand ) {
    return matches( arg, shorthand, "" );
}

int maim::Options::parseString( std::string arg, std::string* returnString ) {
    std::string copy = arg;
    int find = copy.find( "=" );
    if ( find != copy.npos ) {
        copy.at( find ) = ' ';
    }
    // Just in case we error out, grab the actual argument name into x.
    char* x = new char[ arg.size() ];
    char* y = new char[ arg.size() ];
    int num = sscanf( copy.c_str(), "%s %s", x, y );
    if ( num != 2 ) {
        fprintf( stderr, "Error parsing command arguments near %s\n", arg.c_str() );
        fprintf( stderr, "Usage: %s=STRING\n", x );
        fprintf( stderr, "Example: %s=:0 or %s=hostname:0.1\n", x, x );
        fprintf( stderr, "Try -h or --help for help.\n" );
        delete[] x;
        delete[] y;
        return 1;
    }
    *returnString = y;
    delete[] x;
    delete[] y;
    return 0;
}

int maim::Options::parseColor( std::string arg, float* r, float* g, float* b ) {
    std::string copy = arg;
    int find = copy.find( "=" );
    while( find != copy.npos ) {
        copy.at( find ) = ' ';
        find = copy.find( "," );
    }

    // Just in case we error out, grab the actual argument name into x.
    char* x = new char[ arg.size() ];
    int num = sscanf( copy.c_str(), "%s %f %f %f", x, r, g, b );
    if ( num != 4 ) {
        fprintf( stderr, "Error parsing command arguments near %s\n", arg.c_str() );
        fprintf( stderr, "Usage: %s=COLOR\n", x );
        fprintf( stderr, "Example: %s=0,0,0 or %s=0.7,0.2,1\n", x, x );
        fprintf( stderr, "Try -h or --help for help.\n" );
        delete[] x;
        return 1;
    }
    delete[] x;
    return 0;
}

int maim::Options::parseGeometry( std::string arg, int* x, int* y, int* w, int* h ) {
    std::string copy = arg;
    // Replace the first =, all x's and +'s with spaces.
    int find = copy.find( "=" );
    while( find != copy.npos ) {
        copy.at( find ) = ' ';
        find = copy.find( "x" );
    }
    find = copy.find( "+" );
    while( find != copy.npos ) {
        copy.at( find ) = ' ';
        find = copy.find( "+" );
    }

    // Just in case we error out, grab the actual argument name into x.
    char* foo = new char[ arg.size() ];
    int num = sscanf( copy.c_str(), "%s %d %d %d %d", foo, w, h, x, y );
    if ( num != 5 ) {
        fprintf( stderr, "Error parsing command arguments near %s\n", arg.c_str() );
        fprintf( stderr, "Usage: %s=GEOMETRY\n", foo );
        fprintf( stderr, "Example: %s=1920x1080+0+0 or %s=256x256+100+-200\n", foo, foo );
        fprintf( stderr, "Try -h or --help for help.\n" );
        delete[] foo;
        return 1;
    }
    delete[] foo;
    return 0;
}
