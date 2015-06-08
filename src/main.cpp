/* main.cpp
 *
 * Copyright (C) 2014: Dalton Nell, Maim Contributors (https://github.com/naelstrof/maim/graphs/contributors).
 *
 * This file is part of Maim.
 *
 * Maim is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Maim is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Maim.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/param.h>
#include <pwd.h>
#include <string>
#include <sstream>
#include <time.h>

#include "x.hpp"
#include "im.hpp"
#include "cmdline.h"

// Executes a command and gets its output. Used for executing slop for selection.
int exec( std::string cmd, std::string* ret ) {
    FILE* pipe = popen( cmd.c_str(), "r" );
    if ( !pipe ) {
        return EXIT_FAILURE;
    }
    // Doesn't matter what size the buffer is, since it's grabbed in chunks.
    char buffer[255];
    std::string result = "";
    while( !feof( pipe ) ) {
        if( fgets( buffer, 255, pipe ) != NULL ) {
            result += buffer;
        }
    }
    *ret = result;
    if ( pclose( pipe ) == -1 ) {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

// Parse geometry from a string, it's pretty simple really.
int parseGeometry( std::string arg, int* x, int* y, int* w, int* h ) {
    std::string copy = arg;
    // Replace all x's and +'s with spaces. This is so that sscanf works properly, it just doesn't
    // like using anything but spaces for delimiters.
    int find = copy.find( "x" );
    while( find != (int)copy.npos ) {
        copy.at( find ) = ' ';
        find = copy.find( "x" );
    }
    find = copy.find( "+" );
    while( find != (int)copy.npos ) {
        copy.at( find ) = ' ';
        find = copy.find( "+" );
    }
    int num = sscanf( copy.c_str(), "%d %d %d %d", w, h, x, y );
    if ( num != 4 ) {
        fprintf( stderr, "Error parsing geometry from %s\n", arg.c_str() );
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

// We use this to detect if we should enable masking or not.
// This is really important because if a user tries to screenshot a window that's
// slightly off-screen he probably wants the whole window, but if a user
// takes a full screenshot, then he would most certainly want it masked, but
// only if they have pixels that are offscreen.
bool checkMask( std::string type, int x, int y, int w, int h, Window id ) {
    int sw = WidthOfScreen( xengine->m_screen );
    int sh = HeightOfScreen( xengine->m_screen );
    if ( type == "auto" ) {
        // First we check if there's even any offscreen pixels
        int monitorArea = 0;
        std::vector<XRRCrtcInfo*> monitors = xengine->getCRTCS();
        for ( unsigned int i = 0;i<monitors.size();i++ ) {
            XRRCrtcInfo* cmonitor = monitors[ i ];
            monitorArea += cmonitor->height * cmonitor->width;
        }
        xengine->freeCRTCS( monitors );
        // If our monitors cover the entire screen, masking won't do anything anyway.
        if ( monitorArea >= sw * sh ) {
            return false;
        }
        // If we specified an actual window we certainly don't want to mask anything.
        if ( id != None && id != xengine->m_root ) {
            return false;
        }
        // If our screenshot has > 80% of the screen covered, we probably want it masked by off-screen pixels.
        if ( abs( (int)( (float)sw - (float)w ) ) / (float)sw < 0.2 &&
             abs( (int)( (float)sh - (float)h ) ) / (float)sh < 0.2 &&
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

int slop( gengetopt_args_info options, int* x, int* y, int* w, int* h, Window* window ) {
    std::stringstream slopcommand;
    slopcommand << "slop";
    if ( options.nokeyboard_flag ) {
        slopcommand << " --nokeyboard ";
    }
    slopcommand << " -b " << options.bordersize_arg;
    slopcommand << " -p " << options.padding_arg;
    slopcommand << " -t " << options.tolerance_arg;
    slopcommand << " -g " << options.gracetime_arg;
    slopcommand << " -c " << options.color_arg;
    if ( options.nodecorations_flag ) {
        slopcommand << " -n";
    }
    slopcommand << " --min=" << options.min_arg;
    slopcommand << " --max=" << options.max_arg;
    if ( options.xdisplay_given ) {
        slopcommand << " --xdisplay=" << options.xdisplay_arg;
    }
    if ( options.highlight_flag ) {
        slopcommand << " -l";
    }
    slopcommand << "\n";
    std::string result;
    int err = exec( slopcommand.str(), &result );
    if ( err != EXIT_SUCCESS ) {
        return EXIT_FAILURE;
    }
    // From here we'll just be parsing the output of slop.
    // Replace all ='s with spaces in the result, this is so sscanf works properly.
    int find = result.find( "=" );
    while( find != (int)result.npos ) {
        result.at( find ) = ' ';
        find = result.find( "=" );
    }
    Window test = None;
    int num = sscanf( result.c_str(), "X %i\n Y %i\n W %i\n H %i\nG %*s\nID %lu", x, y, w, h, &test );
    if ( num != 5 || ( *w == 0 && *h == 0 ) ) {
        return EXIT_FAILURE;
    }
    // If we actually have a window selection, set the window and offset the coordinates to be
    // localized to that particular window.
    if ( test != None ) {
        *window = test;
        // If we get a window, make sure that slop's selection's origin is around it.
        // Slop's selection's origin defaults to the root window, so we just use XTranslateCoordinates.
        Window junk;
        XTranslateCoordinates( xengine->m_display, xengine->m_root, test, *x, *y, x, y, &junk );
    }
    return EXIT_SUCCESS;
}

int app( int argc, char** argv ) {
    // First parse any options and the filename we need.
    gengetopt_args_info options;
    int err = cmdline_parser( argc, argv, &options );
    if ( err != EXIT_SUCCESS ) {
        return EXIT_FAILURE;
    }
    // Then set up the x interface.
    if ( options.xdisplay_given ) {
        err = xengine->init( options.xdisplay_arg );
    } else {
        // If we weren't specifically given a xdisplay, we try
        // to parse it from environment variables
        char* display = getenv( "DISPLAY" );
        if ( display ) {
            err = xengine->init( display );
        } else {
            fprintf( stderr, "Warning: Failed to parse environment variable: DISPLAY. Using \":0\" instead.\n" );
            err = xengine->init( ":0" );
        }
    }
    if ( err != EXIT_SUCCESS ) {
        fprintf( stderr, "Failed to grab X display!\n" );
        return EXIT_FAILURE;
    }
    // Then the imlib2 interface
    err = imengine->init();
    if ( err != EXIT_SUCCESS ) {
        fprintf( stderr, "Failed to initialize imlib2!\n" );
        return EXIT_FAILURE;
    }
    // Grab all of our variables from the options.
    bool gotGeometry = false;
    bool gotSelectFlag = options.select_flag;
    int x, y, w, h;
    float delay;
    err = sscanf( options.delay_arg, "%f", &delay );
    if ( err != 1 ) {
        fprintf( stderr, "Failed to parse %s as a float for delay!\n", options.delay_arg );
        return EXIT_FAILURE;
    }
    struct timespec delayTime;
    delayTime.tv_sec = delay;
    delayTime.tv_nsec = 0;
    // Get our geometry if we have any.
    if ( options.x_given && options.y_given && options.w_given && options.h_given && !options.geometry_given ) {
        x = options.x_arg;
        y = options.y_arg;
        w = options.w_arg;
        h = options.h_arg;
        gotGeometry = true;
    } else if ( ( options.x_given || options.y_given || options.w_given || options.h_given ) && !options.geometry_given ) {
        fprintf( stderr, "Partial geometry arguments were set, but it isn't enough data to take a screenshot!\n" );
        fprintf( stderr, "Please give the geometry argument or give ALL of the following arguments: x, y, w, h.\n" );
        cmdline_parser_free( &options );
        return EXIT_FAILURE;
    } else if ( options.geometry_given ) {
        err = parseGeometry( options.geometry_arg, &x, &y, &w, &h );
        if ( err != EXIT_SUCCESS ) {
            fprintf( stderr, "Failed to parse geometry %s, should be in format WxH+X+Y!\n", options.geometry_arg );
            cmdline_parser_free( &options );
            return EXIT_FAILURE;
        }
        gotGeometry = true;
    }
    // Get our window if we have one, default to the root window.
    Window window = xengine->m_root;
    if ( options.windowid_given ) {
        window = (Window)options.windowid_arg;
        // Since we have a window we need to turn root coords into our local window coords.
        // but only if the user wants us to.
        if ( !options.localize_flag && window != None ) {
            Window junk;
            XTranslateCoordinates( xengine->m_display, xengine->m_root, window, x, y, &x, &y, &junk );
        }
    }
    // Get our file name
    std::string file = "";
    // If we don't have a file, default to writing to the home directory.
    if ( options.inputs_num == 0 ) {
        // Try as hard as we can to get the current directory.
        int trycount = 0;
        int length = MAXPATHLEN;
        char* currentdir = new char[ length ];
        char* error = getcwd( currentdir, length );
        while ( error == NULL ) {
            delete[] currentdir;
            length *= 2;
            currentdir = new char[ length ];
            error = getcwd( currentdir, length );
            trycount++;
            // Ok someone's trying to be whacky with the current directory if we're going 8 times over
            // the max path length.
            if ( trycount > 3 ) {
                fprintf( stderr, "Failed to grab the current directory!" );
                cmdline_parser_free( &options );
                return EXIT_FAILURE;
            }
        }
        file = currentdir;
        // Get unix timestamp
        std::stringstream result;
        result << (int)time( NULL );
        file += "/" + result.str() + ".png";
        printf( "No file specified, using %s\n", file.c_str() );
        delete [] currentdir;
    } else if ( options.inputs_num == 1 ) {
        file = options.inputs[ 0 ];
    } else {
        fprintf( stderr, "Unexpected number of output files! There should only be one.\n" );
        cmdline_parser_free( &options );
        return EXIT_FAILURE;
    }

    // Finally we have all our information, now to use it.
    if ( gotSelectFlag ) {
        err = slop( options, &x, &y, &w, &h, &window );
        if ( err != EXIT_SUCCESS ) {
            fprintf( stderr, "Selection was cancelled or slop failed to run. Make sure it's installed!\n" );
            cmdline_parser_free( &options );
            return EXIT_FAILURE;
        }
        err = nanosleep( &delayTime, NULL );
        if ( err != EXIT_SUCCESS ) {
            fprintf( stderr, "Warning: Failed to delay the screenshot. Continuing anyway..." );
        }
        err = imengine->screenshot( window, x, y, w, h );
        if ( err != EXIT_SUCCESS ) {
            fprintf( stderr, "Failed to take screenshot.\n" );
            return EXIT_FAILURE;
        }
        if ( !options.hidecursor_flag ) {
            imengine->blendCursor( window, x, y );
        }
        if ( checkMask( options.mask_arg, x, y, w, h, window ) ) {
            imengine->mask( x, y, w, h );
        }
        err = imengine->save( file );
        cmdline_parser_free( &options );
        if ( err != EXIT_SUCCESS ) {
            fprintf( stderr, "Failed to take screenshot.\n" );
            return EXIT_FAILURE;
        }
        return EXIT_SUCCESS;
    }
    if ( gotGeometry ) {
        err = nanosleep( &delayTime, NULL );
        if ( err != EXIT_SUCCESS ) {
            fprintf( stderr, "Warning: Failed to delay the screenshot. Continuing anyway..." );
        }
        err = imengine->screenshot( window, x, y, w, h );
        if ( err != EXIT_SUCCESS ) {
            fprintf( stderr, "Failed to take screenshot.\n" );
            return EXIT_FAILURE;
        }
        if ( !options.hidecursor_flag ) {
            imengine->blendCursor( window, x, y );
        }
        if ( checkMask( options.mask_arg, x, y, w, h, window ) ) {
            imengine->mask( x, y, w, h );
        }
        err = imengine->save( file );
        cmdline_parser_free( &options );
        if ( err != EXIT_SUCCESS ) {
            fprintf( stderr, "Failed to take screenshot.\n" );
            return EXIT_FAILURE;
        }
        return EXIT_SUCCESS;
    }
    // If we didn't get any special options, just screenshot the specified window
    // (Which defaults to the whole screen).
    err = nanosleep( &delayTime, NULL );
    if ( err != EXIT_SUCCESS ) {
        fprintf( stderr, "Warning: Failed to delay the screenshot. Continuing anyway..." );
    }
    err = imengine->screenshot( window );
    if ( err != EXIT_SUCCESS ) {
        fprintf( stderr, "Failed to take screenshot.\n" );
        return EXIT_FAILURE;
    }
    if ( !options.hidecursor_flag ) {
        imengine->blendCursor( window );
    }
    if ( checkMask( options.mask_arg, 0, 0, WidthOfScreen( xengine->m_screen ), HeightOfScreen( xengine->m_screen ), window ) ) {
        imengine->mask();
    }
    err = imengine->save( file );
    cmdline_parser_free( &options );
    if ( err != EXIT_SUCCESS ) {
        fprintf( stderr, "Failed to take screenshot.\n" );
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int main( int argc, char** argv ) {
    try {
        return app( argc, argv );
    } catch( std::bad_alloc const& exception ) {
        fprintf( stderr, "Couldn't allocate enough memory! No space left in RAM." );
        return EXIT_FAILURE;
    } catch( std::exception const& exception ) {
        fprintf( stderr, "Unhandled Exception Thrown: %s\n", exception.what() );
        return EXIT_FAILURE;
    } catch( ... ) {
        fprintf( stderr, "Unknown Exception Thrown!\n" );
        return EXIT_FAILURE;
    }
}
