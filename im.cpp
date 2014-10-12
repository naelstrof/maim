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

int maim::IMEngine::screenshot( std::string file, int x, int y, int w, int h ) {
    Imlib_Image buffer = imlib_create_image( w, h );
    imlib_context_set_image( buffer );
    imlib_copy_drawable_to_image( 0, x, y, w, h, 0, 0, 1 );
    Imlib_Load_Error err;
    int find = file.find_last_of( '.' );
    imlib_save_image_with_error_return( file.c_str(), &err );
    if ( err != IMLIB_LOAD_ERROR_NONE ) {
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
                fprintf( stderr, "path component is non-existant\n" );
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
