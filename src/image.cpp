#include "image.hpp"

RGBAImage::~RGBAImage() {
}

RGBAImage::RGBAImage( XImage* image ) {
    long long int alpha_mask = ~(image->red_mask|image->green_mask|image->blue_mask);
    long long int roffset = get_shift(image->red_mask);
    long long int goffset = get_shift(image->green_mask);
    long long int boffset = get_shift(image->blue_mask);
    long long int aoffset = get_shift(alpha_mask);

    const void* ptrs = image->data;
    data = new char[image->width*image->height*4];
    char* crawler = data;

    width = image->width;
    height = image->height;
	switch( image->bits_per_pixel ) {
		case 32:
            for ( uint off = (image->width*image->height); off>0; --off ) {
                unsigned int real = *static_cast<const unsigned int*>(ptrs);
                *(crawler++) = (real & image->red_mask) >> roffset;
                *(crawler++) = (real & image->green_mask) >> goffset;
                *(crawler++) = (real & image->blue_mask) >> boffset;
                *(crawler++) = (real & alpha_mask) >> aoffset;
                ptrs = static_cast<const unsigned int*>(ptrs) + 1;
            }
            break;
        case 16:
            for ( uint off = (image->width*image->height); off>0; --off ) {
                unsigned short real = *static_cast<const unsigned short*>(ptrs);
                *(crawler++) = (real & image->red_mask) >> roffset;
                *(crawler++) = (real & image->green_mask) >> goffset;
                *(crawler++) = (real & image->blue_mask) >> boffset;
                *(crawler++) = (real & alpha_mask) >> aoffset;
                ptrs = static_cast<const unsigned short*>(ptrs) + 1;
            }
            break;
        // Oh god this sucks, who thought 24 bits was a good idea?
        case 24:
            for ( uint off = (image->width*image->height); off>0; --off ) {
                unsigned int real = *static_cast<const unsigned char*>(ptrs);
                real = real << 4;
                real = real | *static_cast<const unsigned char*>(ptrs)+1;
                real = real << 4;
                real = real | *static_cast<const unsigned char*>(ptrs)+2;
                real = real << 4;
                real = real | *static_cast<const unsigned char*>(ptrs)+3;
                real = real << 4;
                real = real | *static_cast<const unsigned char*>(ptrs)+4;
                real = real << 4;
                real = real | *static_cast<const unsigned char*>(ptrs)+5;
                *(crawler++) = (real & image->red_mask) >> roffset;
                *(crawler++) = (real & image->green_mask) >> goffset;
                *(crawler++) = (real & image->blue_mask) >> boffset;
                *(crawler++) = (real & alpha_mask) >> aoffset;
                ptrs = static_cast<const unsigned char*>(ptrs) + 6;
            }
        default:
            throw new std::runtime_error("Encountered an screen format maim doesn't understand!\n");
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

void RGBAImage::writePNG( std::ostream& streamout ) {
    png_structp png = NULL;
	png_infop info = NULL;
	png_bytep *rows = new png_bytep[height];

	png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if(!png) throw new std::runtime_error( "Failed to write png image" );
	info = png_create_info_struct(png);
	if(!info) throw new std::runtime_error( "Failed to write png image" );
    png_set_error_fn(png, png_get_error_ptr(png), user_error_fn, user_warning_fn);
    png_set_write_fn(png, &streamout, png_write_ostream, png_flush_ostream);
	png_set_compression_level(png, 9);
	png_set_IHDR(png, info, width, height,
       8, PNG_COLOR_TYPE_RGB_ALPHA, PNG_INTERLACE_NONE,
       PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
    for( int i=0;i<height;i++ ) {
        rows[i] = (png_bytep)(data+(i*width*4));
    }
    png_write_info(png, info);
    png_write_image(png, rows);
    png_write_end(png, info);
    png_destroy_write_struct(&png, &info);
}
