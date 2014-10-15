#include <cstdio>
#include <cstdlib>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>

#include "x.hpp"
#include "im.hpp"
#include "options.hpp"

// Executes a command and gets its output. Used for executing slop for selection.
int exec( std::string cmd, std::string* ret ) {
    FILE* pipe = popen( cmd.c_str(), "r" );
    if ( !pipe ) {
        return 1;
    }
    char buffer[128];
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

// We use this to detect if we should enable masking or not.
// This is really important because if a user tries to screenshot a window that's
// slightly off-screen he probably wants the whole window, but if a user
// takes a full screenshot, then he would most certainly want it masked.
bool checkMask( std::string type, int x, int y, int w, int h, Window id ) {
    int sw = WidthOfScreen( xengine->m_screen );
    int sh = HeightOfScreen( xengine->m_screen );
    if ( type == "AUTO" ) {
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
    } else if ( type == "ON" ) {
        return true;
    }
    return false;
}

int main( int argc, char** argv ) {
    // First parse any options and the filename we need.
    int err = options->parseOptions( argc, argv );
    if ( err ) {
        return err;
    }
    // Then set up the x interface.
    err = xengine->init( options->m_xdisplay.c_str() );
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
    xengine->getCRTCS();
    // If we don't have a file, default to writing to the home directory.
    if ( !options->m_gotFile ) {
        struct passwd *pw = getpwuid( getuid() );
        std::string homedir = pw->pw_dir;
        std::string result;
        err = exec( "date +%F-%T", &result );
        if ( err ) {
            homedir += "/screenshot.png";
        } else {
            homedir += "/" + result.substr( 0, result.length() - 1 ) + ".png";
        }
        options->m_file = homedir;
        printf( "No file specified, using %s\n", homedir.c_str() );
    }
    // Check if we were asked to prompt for selection:
    if ( options->m_select ) {
        // Execute slop with any options given.
        std::string result;
        std::string cmd = "slop ";
        cmd += options->m_slopoptions;
        err = exec( cmd, &result );
        if ( err ) {
            fprintf( stderr, "slop failed to run, canceling screenshot. Is slop installed?\n" );
            return 1;
        }
        // from here we'll just be parsing the output of slop.
        // Replace all ='s with spaces in the result, this is so sscanf works properly.
        int find = result.find( "=" );
        while( find != result.npos ) {
            result.at( find ) = ' ';
            find = result.find( "=" );
        }
        int num = sscanf( result.c_str(), "X %d\n Y %d\n W %d\n H %d\n%*s",
                          &options->m_x,
                          &options->m_y,
                          &options->m_w,
                          &options->m_h );
        if ( num == 4 && ( options->m_w > 0 && options->m_h > 0 ) ) {
            bool mask = checkMask( options->m_mask, options->m_x, options->m_y, options->m_w, options->m_h, options->m_window );
            // Wait the designated amount of time before taking the screenshot.
            // 1000000 microseconds = 1 second
            usleep( (unsigned int)(options->m_delay * 1000000.f) );
            err = imengine->screenshot( options->m_file,
                                        options->m_x, options->m_y,
                                        options->m_w, options->m_h,
                                        options->m_hidecursor,
                                        options->m_window, mask );
            if ( err ) {
                return err;
            }
            return 0;
        }
        fprintf( stderr, "Either the user canceled the query for selection, or slop failed to run properly. Canceling screenshot.\n" );
        return 1;
    }
    // Just take a full screen shot if we didn't get any geometry.
    if ( !options->m_gotGeometry ) {
        // Wait the designated amount of time before taking the screenshot.
        // 1000000 microseconds = 1 second
        usleep( (unsigned int)(options->m_delay * 1000000.f) );
        err = imengine->screenshot( options->m_file, options->m_hidecursor, options->m_window, options->m_mask == "OFF" ? false : true );
        if ( err ) {
            return err;
        }
        return 0;
    }
    // Otherwise take a screen shot of the supplied region.
    bool mask = checkMask( options->m_mask, options->m_x, options->m_y, options->m_w, options->m_h, options->m_window );
    usleep( (unsigned int)(options->m_delay * 1000000.f) );
    err = imengine->screenshot( options->m_file,
                                options->m_x, options->m_y,
                                options->m_w, options->m_h,
                                options->m_hidecursor,
                                options->m_window, mask );
    if ( err ) {
        return err;
    }
    return 0;
}
