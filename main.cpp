#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <string>
#include <sstream>

#include "x.hpp"
#include "im.hpp"
#include "cmdline.h"

// Executes a command and gets its output. Used for executing slop for selection.
int exec( std::string cmd, std::string* ret ) {
    FILE* pipe = popen( cmd.c_str(), "r" );
    if ( !pipe ) {
        return 1;
    }
    char buffer[255];
    std::string result = "";
    while( !feof( pipe ) ) {
        if( fgets( buffer, 128, pipe ) != NULL ) {
            result += buffer;
        }
    }
    pclose( pipe );
    *ret = result;
    return 0;
}

int parseGeometry( std::string arg, int* x, int* y, int* w, int* h ) {
    std::string copy = arg;
    // Replace all x's and +'s with spaces.
    int find = copy.find( "x" );
    while( find != copy.npos ) {
        copy.at( find ) = ' ';
        find = copy.find( "x" );
    }
    find = copy.find( "+" );
    while( find != copy.npos ) {
        copy.at( find ) = ' ';
        find = copy.find( "+" );
    }
    int num = sscanf( copy.c_str(), "%d %d %d %d", w, h, x, y );
    if ( num != 4 ) {
        fprintf( stderr, "Error parsing geometry from %s\n", arg.c_str() );
        return 1;
    }
    return 0;
}

// We use this to detect if we should enable masking or not.
// This is really important because if a user tries to screenshot a window that's
// slightly off-screen he probably wants the whole window, but if a user
// takes a full screenshot, then he would most certainly want it masked.
bool checkMask( std::string type, int x, int y, int w, int h, Window id ) {
    int sw = WidthOfScreen( xengine->m_screen );
    int sh = HeightOfScreen( xengine->m_screen );
    if ( type == "auto" ) {
        // If we specified an actual window we certainly don't want to mask anything.
        if ( id != None && id != xengine->m_root ) {
            return false;
        }
        // If our screenshot has > 80% of the screen covered, we probably want it masked by off-screen pixels.
        if ( abs( (float)sw - (float)w ) / (float)sw < 0.2 &&
             abs( (float)sh - (float)h ) / (float)sh < 0.2 &&
             (float)x / (float)sw < 0.2 &&
             (float)y / (float)sh < 0.2 ) {
            return true;
        }
        // Otherwise we're probably taking a picture of a specific thing on the screen.
        return false;
    } else if ( type == "on" ) {
        return true;
    }
    return false;
}

int main( int argc, char** argv ) {
    // First parse any options and the filename we need.
    gengetopt_args_info options;
    int err = cmdline_parser( argc, argv, &options );
    if ( err ) {
        return err;
    }
    // Then set up the x interface.
    err = xengine->init( options.xdisplay_arg );
    if ( err ) {
        fprintf( stderr, "Failed to grab X display!\n" );
        return err;
    }
    // Then the imlib2 interface
    err = imengine->init();
    if ( err ) {
        fprintf( stderr, "Failed to initialize imlib2!\n" );
        return err;
    }
    float delay = atof( options.delay_arg );
    if ( !options.windowid_given ) {
        options.windowid_arg = None;
    }
    std::string file = "";
    // If we don't have a file, default to writing to the home directory.
    if ( options.inputs_num <= 0 ) {
        char currentdir[512];
        // FIXME: getcwd is fundamentally broken, switch it with a C++ equivalent that won't ever have
        // buffer sizing issues.
        getcwd( currentdir, 512 );
        file = currentdir;
        std::string result;
        err = exec( "date +%F-%T", &result );
        if ( err ) {
            file += "/screenshot.png";
        } else {
            file += "/" + result.substr( 0, result.length() - 1 ) + ".png";
        }
        printf( "No file specified, using %s\n", file.c_str() );
    } else if ( options.inputs_num == 1 ) {
        file = options.inputs[ 0 ];
    } else {
        fprintf( stderr, "Unexpected number of output files! There should only be one.\n" );
        cmdline_parser_free( &options );
        return 1;
    }
    // Check if we were asked to prompt for selection:
    if ( options.select_flag ) {
        // Execute slop with any options given.
        std::string result;
        std::stringstream slopcommand;
        slopcommand << "slop ";
        slopcommand << options.nokeyboard_flag ? "--nokeyboard" : "";
        slopcommand << " -b " << options.bordersize_arg;
        slopcommand << " -p " << options.padding_arg;
        slopcommand << " -t " << options.tolerance_arg;
        slopcommand << " -g " << options.gracetime_arg;
        slopcommand << " -c " << options.color_arg;
        slopcommand << options.nodecorations_flag ? "-n" : "";
        slopcommand << " --min=" << options.min_arg;
        slopcommand << " --max=" << options.max_arg;
        slopcommand << options.highlight_flag ? "-l" : "";

        err = exec( slopcommand.str(), &result );
        if ( err ) {
            fprintf( stderr, "slop failed to run, canceling screenshot. Is slop installed?\n" );
            cmdline_parser_free( &options );
            return 1;
        }
        // from here we'll just be parsing the output of slop.
        // Replace all ='s with spaces in the result, this is so sscanf works properly.
        int find = result.find( "=" );
        while( find != result.npos ) {
            result.at( find ) = ' ';
            find = result.find( "=" );
        }
        int x, y, w, h;
        int num = sscanf( result.c_str(), "X %d\n Y %d\n W %d\n H %d\n%*s",
                          &x,
                          &y,
                          &w,
                          &h );
        if ( num == 4 && ( w > 0 && h > 0 ) ) {
            bool mask = checkMask( options.mask_arg, x, y, w, h, options.windowid_arg );
            // Wait the designated amount of time before taking the screenshot.
            // 1000000 microseconds = 1 second
            usleep( (unsigned int)(delay * 1000000.f) );
            err = imengine->screenshot( file,
                                        x, y,
                                        w, h,
                                        options.hidecursor_flag,
                                        options.windowid_arg, mask );
            cmdline_parser_free( &options );
            if ( err ) {
                return err;
            }
            return 0;
        }
        fprintf( stderr, "Either the user canceled the query for selection, or slop failed to run properly. Canceling screenshot.\n" );
        cmdline_parser_free( &options );
        return 1;
    }
    if ( options.x_given && options.y_given && options.w_given && options.h_given ) {
        options.geometry_given = true;
        char temp[128];
        sprintf( temp, "%ix%i+%i+%i\0", options.w_arg, options.h_arg, options.x_arg, options.y_arg );
        options.geometry_arg = temp;
    } else if ( ( options.x_given || options.y_given || options.w_given || options.h_given ) && !options.geometry_given ) {
        fprintf( stderr, "Partial geometry arguments were set, but it isn't enough data to take a screenshot!" );
        fprintf( stderr, "Either give the geometry arugment, or give ALL of the following arguments: x, y, w, h," );
        fprintf( stderr, "    or don't give any of them." );
        cmdline_parser_free( &options );
        return 1;
    }
    // Just take a full screen shot if we didn't get any geometry.
    if ( !options.geometry_given ) {
        // Wait the designated amount of time before taking the screenshot.
        // 1000000 microseconds = 1 second
        usleep( (unsigned int)(delay * 1000000.f) );
        err = imengine->screenshot( file, options.hidecursor_flag, options.windowid_arg, strcmp( options.mask_arg, "off" ) ? true : false );
        cmdline_parser_free( &options );
        if ( err ) {
            return err;
        }
        return 0;
    }
    // Otherwise take a screen shot of the supplied region.
    int x, y, w, h;
    parseGeometry( options.geometry_arg, &x, &y, &w, &h );
    bool mask = checkMask( options.mask_arg, x, y, w, h, options.windowid_arg );
    usleep( (unsigned int)(delay * 1000000.f) );
    err = imengine->screenshot( file,
                                x, y,
                                w, h,
                                options.hidecursor_flag,
                                options.windowid_arg, mask );
    cmdline_parser_free( &options );
    if ( err ) {
        return err;
    }
    return 0;
}
