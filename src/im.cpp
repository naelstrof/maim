/* im.cpp: Handles starting and managing imlib2.
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
#include "im.hpp"

maim::IMEngine* imengine = new maim::IMEngine();

maim::IMEngine::IMEngine() {
}

maim::IMEngine::~IMEngine() {
}

/**
* @brief Initializes our imlib context
*
* @return 0 on success, 1 on failure.
*/
int maim::IMEngine::init() {
    if ( !xengine->m_good ) {
        return EXIT_FAILURE;
    }
    imlib_set_cache_size( 2048 * 1024 );
    imlib_context_set_display( xengine->m_display );
    imlib_context_set_visual( xengine->m_visual );
    imlib_context_set_colormap( xengine->m_colormap );
    imlib_context_set_blend( 1 );
    return EXIT_SUCCESS;
}

/**
* @brief Takes a screenshot of the given window, and leaves the allocated image in the imlib context.
*
* @param id A window ID, for example the root id would take a full screenshot.
*
* @return 0 on success, 1 on failure.
*/
int maim::IMEngine::screenshot( Window id ) {
    if ( id == None ) {
        fprintf( stderr, "Error: Can't screenshot a window with id None!\n" );
        return EXIT_FAILURE;
    }
    // Get the window's dimensions
    Window root;
    int x, y;
    unsigned int w, h, b, d;
    int status = XGetGeometry( xengine->m_display, id, &root, &x, &y, &w, &h, &b, &d );
    if ( status == 0 ) {
        fprintf( stderr, "Error: Failed to grab window geometry of window id: %lu\n", id );
        return EXIT_FAILURE;
    }
    // Create an uninitialized image buffer of the same width and height as the window
    Imlib_Image buffer = imlib_create_image( w, h );
    imlib_context_set_image( buffer );
    // Make sure that imlib knows that it's possible for the image to have alpha
    // prevents blending issues in the future.
    imlib_image_set_has_alpha( 1 );
    imlib_context_set_drawable( id );
    int destinationx = x < 0 ? -x : 0;
    int destinationy = y < 0 ? -y : 0;
    imlib_copy_drawable_to_image( 0, destinationx, destinationy, w, h, 0, 0, 0 );
    // Screenshot image is now in the imlib context!
    return EXIT_SUCCESS;
}

/**
* @brief Similar to maim::IMEngine::screenshot( Window id ), but also crops the image.
*
* @param id The window to take a screenshot of.
* @param x Starting X position of the crop.
* @param y Starting Y position of the crop.
* @param w Width of the final cropped image.
* @param h Height of the final cropped image.
*
* @return 0 on success, 1 on failure.
*/
int maim::IMEngine::screenshot( Window id, int x, int y, unsigned int w, unsigned int h ) {
    if ( id == None ) {
        fprintf( stderr, "Error: Can't screenshot a window with id None!\n" );
        return EXIT_FAILURE;
    }
    // Create an uninitialized image buffer of the same width and height as our selection.
    Imlib_Image buffer = imlib_create_image( w, h );
    imlib_context_set_image( buffer );
    // Make sure that imlib knows that it's possible for the image to have alpha
    // prevents blending issues in the future.
    imlib_image_set_has_alpha( 1 );
    imlib_context_set_drawable( id );
    // This make sure negative x or y values actually affect the location
    // of the drawable. Since asking for it to copy from a negative
    // x or y position doesn't seem to do anything.
    // Might be a bug, but if it's fixed it'll break my program as it is now. :v
    int destinationx = x < 0 ? -x : 0;
    int destinationy = y < 0 ? -y : 0;
    imlib_copy_drawable_to_image( 0, x, y, w, h, destinationx, destinationy, 0 );
    // Screenshot image is now in the imlib context!
    return EXIT_SUCCESS;
}

/**
* @brief Blends the system cursor onto the current image in the imlib context.
*
* @param id The window used to take the screenshot, this is used to grab the relative cursor position.
* @param x X offset of the window to the image
* @param y Y offset of the window to the image
*
* @return 0 on success, 1 on failure.
*/
int maim::IMEngine::blendCursor( Window id, int x, int y ) {
    if ( id == None ) {
        fprintf( stderr, "Error: Can't blend the cursor without a valid window (None given)!\n" );
        return EXIT_FAILURE;
    }
    if ( imlib_context_get_image() == NULL ) {
        fprintf( stderr, "Error: Can't blend the cursor to NULL image!\n" );
        return EXIT_FAILURE;
    }
    // Grab the cursor image with XFixes
    XFixesCursorImage* xcursor = XFixesGetCursorImage( xengine->m_display );
    // If we failed don't do anything.
    if ( !xcursor ) {
        fprintf( stderr, "Warning: Failed to grab cursor image, it won't appear in screenshots!\n" );
        return EXIT_FAILURE;
    }
    // For whatever reason, XFixes returns 32 bit ARGB colors with 64 bit longs?
    // I'm guessing this is because some old AMD cpu's longs are actually 32 bits.
    // Regardless this is how I convert it to the correct bit length.
    uint32_t* pixels = new uint32_t[ xcursor->width * xcursor->height ];
    for ( int i=0;i<xcursor->width*xcursor->height;i++ ) {
        pixels[ i ] = (uint32_t)xcursor->pixels[ i ];
    }
    Imlib_Image cursor = imlib_create_image_using_data( xcursor->width, xcursor->height, pixels );
    // First save the image that we'll be applying the cursor to.
    Imlib_Image buffer = imlib_context_get_image();
    // Make sure imlib knows that it has alpha
    imlib_context_set_image( cursor );
    imlib_image_set_has_alpha( 1 );
    // Then we quickly set the old image back.
    imlib_context_set_image( buffer );
    // We grab the window's position with this, so we can find where the cursor would be located on our image.
    Window root, junk;
    int tx, ty;
    unsigned int tw, th, tb, td;
    int status = XGetGeometry( xengine->m_display, id, &root, &tx, &ty, &tw, &th, &tb, &td );
    if ( status == 0 ) {
        fprintf( stderr, "Error: Failed to grab window geometry of window id: %lu\n", id );
        return EXIT_FAILURE;
    }
    // Make sure the window's position is in root coordinates
    XTranslateCoordinates( xengine->m_display, id, root, -tb, -tb, &tx, &ty, &junk );
    // Finally blend the cursor to the screenshot, we don't have to worry about the cursor not being visible as it would be a non-existant image if it was.
    imlib_blend_image_onto_image( cursor, 0, 0, 0, xcursor->width, xcursor->height, xcursor->x-tx-xcursor->xhot-x, xcursor->y-ty-xcursor->yhot-y, xcursor->width, xcursor->height );
    // Free the cursor image and delete its data.
    imlib_context_set_image( cursor );
    imlib_free_image();
    imlib_context_set_image( buffer );
    free( xcursor );
    delete[] pixels;
    return EXIT_SUCCESS;
}

/**
* @brief This one is a doozy, it tries to mask off-screen pixels so they don't show up as garbage in screenshots.
*        It's highly situational in it's usage, and often yields zero noticable results as most people not only
*        don't have a multi-monitor setup, but the few people that do don't have them unevenly set up.
*
* @param x X offset of the image in buffer in relation to the physical monitors.
* @param y Y offset of the image in buffer in relation to the physical monitors.
* @param w Width of the image in buffer.
* @param h Height of the image in buffer.
*
* @return 0 on success, 1 on failure.
*/
int maim::IMEngine::mask( int x, int y, unsigned int w, unsigned int h ) {
    // If xengine couldn't find any physical screens. We don't do anything.
    if ( !xengine->m_res ) {
        return EXIT_FAILURE;
    }
    if ( imlib_context_get_image() == NULL ) {
        fprintf( stderr, "Error: Can't mask a NULL image!\n" );
        return EXIT_FAILURE;
    }
    // If no width or height arguments were given, we grab them ourselves.
    if ( w == 0 && h == 0 && x == 0 && y == 0 ) {
        w = imlib_image_get_width();
        h = imlib_image_get_height();
    } else if ( w == 0 || h == 0 ) {
        fprintf( stderr, "Error: Tried to mask an image with 0 width or height!\n" );
        return EXIT_FAILURE;
    }
    // So first we generate an image of the same exact size filled with the color 0,0,0,0
    Imlib_Image mask = imlib_create_image( w, h );
    // Save our original image.
    Imlib_Image buffer = imlib_context_get_image();
    imlib_context_set_image( mask );
    imlib_image_set_has_alpha( 1 );
    imlib_context_set_color( 0, 0, 0, 0 );
    imlib_image_fill_rectangle( 0, 0, w, h );
    // Grab our monitor information, (basically get pixel rectangles that are actually displaying).
    std::vector<XRRCrtcInfo*> monitors = xengine->getCRTCS();
    imlib_context_set_color( 0, 0, 0, 255 );
    for ( unsigned int i=0;i<monitors.size();i++ ) {
        XRRCrtcInfo* cmonitor = monitors[ i ];
        // Then quickly block in our visible pixels on our mask
        imlib_image_fill_rectangle( cmonitor->x - x, cmonitor->y - y, cmonitor->width, cmonitor->height );
    }
    xengine->freeCRTCS( monitors );
    imlib_context_set_color( 255, 255, 255, 255 );
    imlib_context_set_image( buffer );
    // Then finally apply our mask to the original image, which should remove any garbage pixels that are off-screen.
    imlib_image_copy_alpha_to_image( mask, 0, 0 );
    imlib_context_set_image( mask );
    imlib_free_image();
    imlib_context_set_image( buffer );
    // Then finally apply our mask to the original image, which should remove any garbage pixels that are off-screen.
    // But unfortunately that doesn't actually delete the pixels, so we have to do one more pass.
    // This whole thing just creates another blank image, and blends the masked image onto it.
    // This is because all we did was copy the alpha channel, so formats like jpg wouldn't even care that we did all this
    // work unless I add this extra pass.
    Imlib_Image finalimage = imlib_create_image( w, h );
    imlib_context_set_image( finalimage );
    imlib_image_set_has_alpha( 1 );
    imlib_context_set_color( 0, 0, 0, 0 );
    imlib_image_fill_rectangle( 0, 0, w, h );
    imlib_context_set_color( 255, 255, 255, 255 );
    imlib_blend_image_onto_image( buffer, 1, 0, 0, w, h, 0, 0, w, h );
    imlib_context_set_image( buffer );
    imlib_free_image();
    imlib_context_set_image( finalimage );
    // Our final image is in the imlib context!
    return EXIT_SUCCESS;
}

/**
* @brief Simply saves the image in buffer to the given file, then frees the image from memory. Imlib handles formating (.png, .gif, etc).
*
* @param file The file path to save.
*
* @return 0 on success, 1 on failure.
*/
int maim::IMEngine::save( std::string file ) {
    Imlib_Load_Error err;
    imlib_save_image_with_error_return( file.c_str(), &err );
    if ( err == IMLIB_LOAD_ERROR_NONE ) {
        imlib_free_image();
        return EXIT_SUCCESS;
    }
    fprintf( stderr, "Failed to save image %s: ", file.c_str() );
    switch( err ) {
        case IMLIB_LOAD_ERROR_UNKNOWN:
        default: {
            fprintf( stderr, "unknown error %d\n", (int)err );
            break;
        }
        case IMLIB_LOAD_ERROR_OUT_OF_FILE_DESCRIPTORS:
            fprintf( stderr, "out of file descriptors\n" );
            break;
        case IMLIB_LOAD_ERROR_OUT_OF_MEMORY:
            fprintf( stderr, "out of memory\n" );
            break;
        case IMLIB_LOAD_ERROR_TOO_MANY_SYMBOLIC_LINKS:
            fprintf( stderr, "path contains too many symbolic links\n" );
            break;
        case IMLIB_LOAD_ERROR_PATH_POINTS_OUTSIDE_ADDRESS_SPACE:
            fprintf( stderr, "path points outside address space\n" );
            break;
        case IMLIB_LOAD_ERROR_PATH_COMPONENT_NOT_DIRECTORY:
            fprintf( stderr, "path component is not a directory\n" );
            break;
        case IMLIB_LOAD_ERROR_PATH_COMPONENT_NON_EXISTANT:
            fprintf( stderr, "path component is non-existant (~ isn't expanded inside quotes!)\n" );
            break;
        case IMLIB_LOAD_ERROR_PATH_TOO_LONG:
            fprintf( stderr, "path is too long\n" );
            break;
        case IMLIB_LOAD_ERROR_NO_LOADER_FOR_FILE_FORMAT:
            fprintf( stderr, "no loader for file format (unsupported format)\n" );
            break;
        case IMLIB_LOAD_ERROR_OUT_OF_DISK_SPACE: {
            fprintf( stderr, "not enough disk space\n" );
            break;
        }
        case IMLIB_LOAD_ERROR_FILE_DOES_NOT_EXIST: {
            fprintf( stderr, "file does not exist\n" );
            break;
        }
        case IMLIB_LOAD_ERROR_FILE_IS_DIRECTORY: {
            fprintf( stderr, "file is a directory\n" );
            break;
        }
        case IMLIB_LOAD_ERROR_PERMISSION_DENIED_TO_WRITE:
        case IMLIB_LOAD_ERROR_PERMISSION_DENIED_TO_READ: {
            fprintf( stderr, "permission denied\n" );
            break;
        }
    }
    imlib_free_image();
    return EXIT_FAILURE;
}
