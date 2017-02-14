#include "cimg.hpp"

cimg_library::CImg<unsigned char> cimg_from_x11( XImage* image )
{
    cimg_library::CImg<unsigned char> res(image->width,image->height,1,4);

    unsigned char  *pR, *pG, *pB, *pA;
    pR = res.data(0,0,0,0);
    pG = res.data(0,0,0,1);
    pB = res.data(0,0,0,2);
    pA = res.data(0,0,0,3);
    long long int alpha_mask = ~(image->red_mask|image->green_mask|image->blue_mask);
    long long int roffset = get_shift(image->red_mask);
    long long int goffset = get_shift(image->green_mask);
    long long int boffset = get_shift(image->blue_mask);
    long long int aoffset = get_shift(alpha_mask);

    const void* ptrs = image->data;

	switch( image->bits_per_pixel ) {
		case 32:
            for ( uint off = (image->width*image->height); off>0; --off ) {
                unsigned int real = *static_cast<const unsigned int*>(ptrs);
                *(pR++) = (real & image->red_mask) >> roffset;
                *(pG++) = (real & image->green_mask) >> goffset;
                *(pB++) = (real & image->blue_mask) >> boffset;
                *(pA++) = (real & alpha_mask) >> aoffset;
                ptrs = static_cast<const unsigned int*>(ptrs) + 1;
            }
            break;
        case 16:
            for ( uint off = (image->width*image->height); off>0; --off ) {
                unsigned short real = *static_cast<const unsigned short*>(ptrs);
                *(pR++) = (real & image->red_mask) >> roffset;
                *(pG++) = (real & image->green_mask) >> goffset;
                *(pB++) = (real & image->blue_mask) >> boffset;
                *(pA++) = (real & alpha_mask) >> aoffset;
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
                *(pR++) = (real & image->red_mask) >> roffset;
                *(pG++) = (real & image->green_mask) >> goffset;
                *(pB++) = (real & image->blue_mask) >> boffset;
                *(pA++) = (real & alpha_mask) >> aoffset;
                ptrs = static_cast<const unsigned char*>(ptrs) + 6;
            }
        default:
            throw new std::runtime_error("Encountered an screen format maim doesn't understand!\n");
    }

    return res;
}

