#include "image.hpp"

ARGBImage::~ARGBImage() {
    delete[] data;
}

ARGBImage::ARGBImage( XImage* image, glm::ivec2 iloc, glm::ivec4 selectionrect, int channels, X11* x11 ) {
    this->imagex = iloc.x;
    this->imagey = iloc.y;
    this->channels = channels;
    glm::ivec2 spos = glm::ivec2( selectionrect.x, selectionrect.y );
    offset = spos-iloc;
    long long int alpha_mask = ~(image->red_mask|image->green_mask|image->blue_mask);
    long long int roffset = get_shift(image->red_mask);
    long long int goffset = get_shift(image->green_mask);
    long long int boffset = get_shift(image->blue_mask);
    long long int aoffset = get_shift(alpha_mask);
    width = selectionrect.z;
    height = selectionrect.w;
    data = new unsigned char[width*height*channels];
    // Clear necessary stuff
    // Top rect
    for ( int y = 0; y < glm::min(-offset.y,(int)height);y++ ) {
        for ( int x = 0; x < width;x++ ) {
            for ( int c = 0; c < channels;c++ ) {
                data[(y*width+x)*channels+c] = 0;
            }
        }
    }
    // Left rect
    for ( int y = 0; y < height;y++ ) {
        for ( int x = 0; x < glm::min(-offset.x,(int)width);x++ ) {
            for ( int c = 0; c < channels;c++ ) {
                data[(y*width+x)*channels+c] = 0;
            }
        }
    }
    // Bot rect
    for ( int y=-offset.y+image->height; y<height; y++ ) {
        for ( int x = 0; x < width;x++ ) {
            for ( int c = 0; c < channels;c++ ) {
                data[(y*width+x)*channels+c] = 0;
            }
        }
    }
    // Right rect
    for ( int y = 0; y < height;y++ ) {
        for ( int x = -offset.x+image->width; x<width; x++ ) {
            for ( int c = 0; c < channels;c++ ) {
                data[(y*width+x)*channels+c] = 0;
            }
        }
    }

    // Find the intersection of the rectangles.
    int maxx = glm::max( offset.x, 0 );
    int maxy = glm::max( offset.y, 0 );
    int minw = glm::min( (int)(offset.x+width), image->width);
    int minh = glm::min( (int)(offset.y+height), image->height );

    // Loop only through the intersecting parts, copying everything.
    // Also check if we have any useful alpha data.
    switch( channels ) {
        case 4:
            if ( aoffset >= image->depth ) {
                for(int y = maxy; y < minh; y++) {
                    for(int x = maxx; x < minw; x++) {
                        // This is where we just have RGB but require an RGBA image.
                        computeRGBAPixel( data, image, x, y, roffset, goffset, boffset, width, offset );
                    }
                }
            } else {
                for(int y = maxy; y < minh; y++) {
                    for(int x = maxx; x < minw; x++) {
                        computeRGBAPixel( data, image, x, y, roffset, goffset, boffset, aoffset, width, offset );
                    }
                }
            }
            break;
        case 3:
            for(int y = maxy; y < minh; y++) {
                for(int x = maxx; x < minw; x++) {
                    computeRGBPixel( data, image, x, y, roffset, goffset, boffset, width, offset );
                }
            }
            break;
        default:
            throw new std::invalid_argument("Invalid number of channels provided to image.");
    }
}

void png_write_ostream(png_structp png_ptr, png_bytep data, png_size_t length)
{
    std::ostream *stream = (std::ostream*)png_get_io_ptr(png_ptr); //Get pointer to ostream
    stream->write((char*)data,length); //Write requested amount of data
}

void png_flush_ostream(png_structp png_ptr)
{
    std::ostream *stream = (std::ostream*)png_get_io_ptr(png_ptr); //Get pointer to ostream
    stream->flush();
}

void user_error_fn(png_structp png_ptr, png_const_charp error_msg)
{
    throw new std::runtime_error(error_msg);
}

void user_warning_fn(png_structp png_ptr, png_const_charp warning_msg)
{
    std::cerr << warning_msg << "\n";
}

void ARGBImage::writePNG( std::ostream& streamout, int quality ) {
    if ( quality > 10 || quality < 1 ) {
        throw new std::invalid_argument("Quality argument must be between 1 and 10");
    }
    png_structp png = NULL;
    png_infop info = NULL;
    png_bytep *rows = new png_bytep[height];

    png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if(!png) throw new std::runtime_error( "Failed to write png image" );
    info = png_create_info_struct(png);
    if(!info) throw new std::runtime_error( "Failed to write png image" );
    png_set_error_fn(png, png_get_error_ptr(png), user_error_fn, user_warning_fn);
    png_set_write_fn(png, &streamout, png_write_ostream, png_flush_ostream);
    png_set_compression_level(png, quality-1);
    if ( channels == 4 ) {
        png_set_IHDR(png, info, width, height,
           8, PNG_COLOR_TYPE_RGB_ALPHA, PNG_INTERLACE_NONE,
           PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
    } else {
        png_set_IHDR(png, info, width, height,
           8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
           PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
    }
    for( int i=0;i<height;i++ ) {
        rows[i] = (png_bytep)(data+(i*width*4));
    }
    png_write_info(png, info);
    png_write_image(png, rows);
    png_write_end(png, info);
    png_destroy_write_struct(&png, &info);
}

void init_buffer(jpeg_compress_struct* cinfo) {}
 
/* what to do when the buffer is full; this should almost never
 * happen since we allocated our buffer to be big to start with
 */
boolean empty_buffer(jpeg_compress_struct* cinfo) {
    return TRUE;
}
 
/* finalize the buffer and do any cleanup stuff */
void term_buffer(jpeg_compress_struct* cinfo) {}

void ARGBImage::writeJPEG( std::ostream& streamout, int quality ) {
    if ( channels != 3 ) {
        throw new std::runtime_error( "JPEG tried to save image with more than 3 channels." );
    }
    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr       jerr;
    struct jpeg_destination_mgr dmgr;
 
    /* create our in-memory output buffer to hold the jpeg */
    JOCTET * out_buffer   = new JOCTET[width * height *3];
 
    /* here is the magic */
    dmgr.init_destination    = init_buffer;
    dmgr.empty_output_buffer = empty_buffer;
    dmgr.term_destination    = term_buffer;
    dmgr.next_output_byte    = out_buffer;
    dmgr.free_in_buffer      = width * height *3;
 
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo);
 
    /* make sure we tell it about our manager */
    cinfo.dest = &dmgr;
 
    cinfo.image_width      = width;
    cinfo.image_height     = height;
    cinfo.input_components = 3;
    cinfo.in_color_space   = JCS_RGB;
 
    jpeg_set_defaults(&cinfo);
    // Convert quality from scale 1-10 to 0-100
    jpeg_set_quality (&cinfo, (int)((float)quality-1.f)*(100.f/9.f), TRUE);
    jpeg_start_compress(&cinfo, TRUE);
 
    JSAMPROW row_pointer;
    unsigned char* buffer    = (unsigned char*)data;
 
    /* main code to write jpeg data */
    while (cinfo.next_scanline < cinfo.image_height) {         
        row_pointer = (JSAMPROW) &buffer[cinfo.next_scanline * 3*width];
        jpeg_write_scanlines(&cinfo, &row_pointer, 1);
    }
    jpeg_finish_compress(&cinfo);
 
    streamout.write( (const char*)out_buffer, cinfo.dest->next_output_byte - out_buffer );
    delete[] out_buffer;
}

bool ARGBImage::intersect( XRRCrtcInfo* a, glm::vec4 b ) {
    if (a->x < b.x + b.z &&
        a->x + a->width > b.x &&
        a->y < b.y + b.w &&
        a->height + a->y > b.y) {
        return true;
    }
    return false;
}

bool ARGBImage::containsCompletely( XRRCrtcInfo* a, glm::vec4 b ) {
    if ( b.x >= a->x && b.y >= a->y && b.x+b.z <= a->x+a->width && b.y+b.w <= a->y+a->height ) {
        return true;
    }
    return false;
}

void ARGBImage::mask(X11* x11) {
    if ( !x11->haveXRR ) {
        return;
    }
    std::vector<XRRCrtcInfo*> physicalMonitors = x11->getCRTCS();
    // Make sure a masking needs to happen, it's not a perfect detection,
    // but will detect most situations where a masking actually needs to happen.
    for ( int i=0;i<physicalMonitors.size();i++ ) {
        XRRCrtcInfo* m = physicalMonitors[i];
        if ( intersect(m, glm::vec4(imagex, imagey, width, height ) ) ) {
            if ( containsCompletely(m, glm::vec4( imagex, imagey, width, height ) ) ) {
                x11->freeCRTCS(physicalMonitors);
                return;
            }
        }
    }
    unsigned char* copy = new unsigned char[width*height*channels];
    // Zero out our copy
    for ( int y = 0; y < height;y++ ) {
        for ( int x = 0; x < width;x++ ) {
            for ( int c = 0; c < channels;c++ ) {
                copy[(y*width+x)*channels+c] = 0;
            }
        }
    }
    for ( int i=0;i<physicalMonitors.size();i++ ) {
        // Make sure we're intersecting
        XRRCrtcInfo* m = physicalMonitors[i];
        if ( !intersect(m, glm::vec4(imagex, imagey, width, height ) ) ) {
            continue;
        }
        // Copy over data within the intersecting areas.
        for ( int y = glm::max(0,m->y-imagey); y<glm::min(height,m->y+m->height-imagey);y++ ) {
            for ( int x = glm::max(0,m->x-imagex); x < glm::min(width,m->x+m->width-imagex);x++ ) {
                for ( int c = 0; c < channels;c++ ) {
                    copy[(y*width+x)*channels+c] = data[(y*width+x)*channels+c];
                }
            }
        }
    }
    x11->freeCRTCS(physicalMonitors);
    delete[] data;
    data = copy;
}

void ARGBImage::blendCursor( X11* x11 ) {
    if ( !x11->haveXFixes ) {
        return;
    }
    XFixesCursorImage* xcursor = XFixesGetCursorImage( x11->display );
    if ( !xcursor ) {
        return;
    }
    // 64bit -> 32bit conversion
    unsigned char pixels[xcursor->width * xcursor->height * 4];
    for ( unsigned int i=0;i<xcursor->width*xcursor->height;i++ ) {
        ((unsigned int*)pixels)[ i ] = (unsigned int)xcursor->pixels[ i ];
    }
    xcursor->y -= xcursor->yhot + offset.x;
    xcursor->x -= xcursor->xhot + offset.y;
    for ( int y = glm::max(0,xcursor->y-imagey); y<glm::min((int)height,xcursor->y+xcursor->height-imagey);y++ ) {
        for ( int x = glm::max(0,xcursor->x-imagex); x < glm::min((int)width,xcursor->x+xcursor->width-imagex);x++ ) {
            int cx = x-(xcursor->x-imagex);
            int cy = y-(xcursor->y-imagey);
            float alpha = (float)pixels[(cy*xcursor->width+cx)*4+3]/255.f;
            data[(y*width+x)*channels] = data[(y*width+x)*channels]*(1-alpha) + pixels[(cy*xcursor->width+cx)*4+2]*alpha;
            data[(y*width+x)*channels+1] = data[(y*width+x)*channels+1]*(1-alpha) + pixels[(cy*xcursor->width+cx)*4+1]*alpha;
            data[(y*width+x)*channels+2] = data[(y*width+x)*channels+2]*(1-alpha) + pixels[(cy*xcursor->width+cx)*4]*alpha;
            // If the original image has alpha, we need to override it.
            if ( channels == 4 ) {
                data[(y*width+x)*channels+3] = glm::min(data[(y*width+x)*channels+3]+pixels[(cy*xcursor->width+cx)*4+3],255);
            }
        }
    }
}
