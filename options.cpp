#include "options.hpp"

maim::Options* options = new maim::Options();

maim::Options::Options() {
    m_xdisplay = ":0";
    m_file = "";
    m_select = false;
    m_gotGeometry = false;
    m_gotFile = false;
    m_x = 0;
    m_y = 0;
    m_w = 0;
    m_h = 0;
}

void maim::Options::printHelp() {
    printf( "Usage: maim [options] [file]\n" );
    printf( "maim (make image) makes an image out of the given area on the given X screen. Defaults to the whole screen.\n" );
    printf( "\n" );
    printf( "options\n" );
    printf( "    -h, --help                         show this message.\n" );
    printf( "    -s, --select                       manually select screenshot location. Only works when slop is installed.\n" );
    printf( "    -d=STRING, --display=STRING        set x display (STRING must be hostname:number.screen_number format)\n" );
    printf( "    -g=GEOMETRY, --geometry=GEOMETRY   set the region to capture. GEOMETRY is in format WxH+X+Y\n" );
    printf( "    -x=INT                             set the x coordinate for taking an image\n" );
    printf( "    -y=INT                             set the y coordinate for taking an image\n" );
    printf( "    -w=INT                             set the width for taking an image\n" );
    printf( "    -h=INT                             set the height for taking an image\n" );
    printf( "slop options\n" );
    printf( "    -nkb, --nokeyboard                 don't try to grab the keyboard. This may fix problems with certain window managers.\n" );
    printf( "    -c=COLOR, --color=COLOR            set selection rectangle color, COLOR is in format FLOAT,FLOAT,FLOAT\n" );
    printf( "    -b=INT, --bordersize=INT           set selection rectangle border size.\n" );
    printf( "    -p=INT, --padding=INT              set m_padding size for selection.\n" );
    printf( "    -t=INT, --tolerance=INT            if you have a shaky mouse, increasing this value will make slop detect single clicks better. Rather than interpreting your shaky clicks as region selections.\n" );
    printf( "examples\n" );
    printf( "    maim -g=1920x1080+0+30\n" );
}

int maim::Options::parseOptions( int argc, char** argv ) {
    // Simple command parsing. Just uses sscanf to read each argument.
    // It looks complicated because you have to have spaces for delimiters for sscanf.
    for ( int i=1; i<argc; i++ ) {
        std::string arg = argv[i];
        if ( matches( arg, "-d=", "--display=" ) ) {
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
        } else if ( matches( arg, "-nkb", "--nokeyboard" ) ) {
            m_slopoptions += arg + " ";
        } else if ( matches( arg, "-c=", "--color=" ) ) {
            m_slopoptions += arg + " ";
        } else if ( matches( arg, "-b=", "--bordersize=" ) ) {
            m_slopoptions += arg + " ";
        } else if ( matches( arg, "-p=", "--padding=" ) ) {
            m_slopoptions += arg + " ";
        } else if ( matches( arg, "-t=", "--tolerance=" ) ) {
            m_slopoptions += arg + " ";
        } else if ( matches( arg, "-x=" ) ) {
            m_gotGeometry = true;
            int err = parseInt( arg, &m_x );
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
        } else {
            // If nothing matched, and we're at the end of the argument list. Parse the last arg as the file name.
            if ( i == argc-1 ) {
                m_file = argv[i];
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
