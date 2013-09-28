#include "im.hpp"

maim::IMEngine* imengine = new maim::IMEngine();

maim::IMEngine::IMEngine() {
}

maim::IMEngine::~IMEngine() {
}

int maim::IMEngine::init() {
    if ( !xengine->m_good ) {
        return 1;
    }
    imlib_set_cache_size( 2048 * 1024 );
    imlib_context_set_display( xengine->m_display );
    imlib_context_set_visual( xengine->m_visual );
    imlib_context_set_colormap( xengine->m_colormap );
    imlib_context_set_drawable( xengine->m_root );
    return 0;
}

std::string maim::IMEngine::guessFormat( std::string file ) {
    // Try to guess image format.
    std::string format;
    int find = file.find_last_of( '.' );
    if ( find == std::string::npos ) {
        return "png";
    }
    format = file.substr( find );
    if ( format.length() > 3 && format != "jpeg" ) {
        return "png";
    }
    return format;
}

int maim::IMEngine::screenshot( std::string file, int x, int y, int w, int h ) {
    Imlib_Image buffer = imlib_create_image( w, h );
    imlib_context_set_image( buffer );
    imlib_copy_drawable_to_image( 0, x, y, w, h, 0, 0, 1 );
    imlib_image_set_format( guessFormat( file ).c_str() );
    Imlib_Load_Error err;
    imlib_save_image_with_error_return( file.c_str(), &err );
    if ( err != IMLIB_LOAD_ERROR_NONE ) {
        fprintf( stderr, "Failed to save image %s: ", file.c_str() );
        switch( err ) {
            default: {
                fprintf( stderr, "unknown error %d\n", (int)err );
                break;
            }
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
    }

    return 0;
}

int maim::IMEngine::screenshot( std::string file ) {
    // Get root window's dimensions
    Window root;
    int x, y;
    unsigned int w, h, b, d;
    XGetGeometry( xengine->m_display, xengine->m_root, &root,
                  &x, &y, &w, &h, &b, &d );
    // Then screenshot it.
    return screenshot( file, x, y, w, h );
}
