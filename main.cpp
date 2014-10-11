#include <cstdio>
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

int main( int argc, char** argv ) {
    // First parse any options and the file we need.
    int err = options->parseOptions( argc, argv );
    if ( err ) {
        return err;
    }
    // Then we set up the x interface
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
    // If we don't have a file, default to writing to the home directory.
    if ( !options->m_gotFile ) {
        struct passwd *pw = getpwuid( getuid() );
        std::string homedir = pw->pw_dir;
        homedir += "/screenshot.png";
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
            err = imengine->screenshot( options->m_file,
                                        options->m_x, options->m_y,
                                        options->m_w, options->m_h );
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
        err = imengine->screenshot( options->m_file );
        if ( err ) {
            return err;
        }
        return 0;
    }
    // Otherwise take a screen shot of the supplied region.
    err = imengine->screenshot( options->m_file,
                                options->m_x, options->m_y,
                                options->m_w, options->m_h );
    if ( err ) {
        return err;
    }
    return 0;
}
