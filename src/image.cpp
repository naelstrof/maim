#include "image.hpp"

ARGBImage::~ARGBImage() {
    delete[] data;
}

ARGBImage::ARGBImage( XImage* image, glm::ivec2 iloc, glm::ivec4 selectionrect, int channels ) {
    this->channels = channels;
    glm::ivec2 spos = glm::ivec2( selectionrect.x, selectionrect.y );
    glm::ivec2 offset = spos-iloc;
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
    for ( unsigned int y = 0; y < glm::min(-offset.y,(int)height);y++ ) {
        for ( unsigned int x = 0; x < width;x++ ) {
            for ( unsigned int c = 0; c < channels;c++ ) {
                data[(y*width+x)*channels+c] = 0;
            }
        }
    }
    // Left rect
    for ( unsigned int y = 0; y < height;y++ ) {
        for ( unsigned int x = 0; x < glm::min(-offset.x,(int)width);x++ ) {
            for ( unsigned int c = 0; c < channels;c++ ) {
                data[(y*width+x)*channels+c] = 0;
            }
        }
    }
    // Bot rect
    for ( unsigned int y=-offset.y+image->height; y<height; y++ ) {
        for ( unsigned int x = 0; x < width;x++ ) {
            for ( unsigned int c = 0; c < channels;c++ ) {
                data[(y*width+x)*channels+c] = 0;
            }
        }
    }
    // Right rect
    for ( unsigned int y = 0; y < height;y++ ) {
        for ( unsigned int x = -offset.x+image->width; x<width; x++ ) {
            for ( unsigned int c = 0; c < channels;c++ ) {
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
	jpeg_set_quality (&cinfo, (int)((float)quality-1.f)*(100.f/9.f), true);
	jpeg_start_compress(&cinfo, true);
 
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
