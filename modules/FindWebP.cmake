# - Find WebP
# Find the WebP libraries
#
#  This module defines the following variables:
#     WEBP_FOUND        - 1 if WEBP_INCLUDE_DIR & WEBP_LIBRARY are found, 0 otherwise
#     WEBP_INCLUDE_DIR  - where to find webp/encode.h, etc.
#     WEBP_LIBRARY      - the libwebp library

find_path( WEBP_INCLUDE_DIR
           NAMES webp/encode.h
           PATH_SUFFIXES /usr/include /include
           DOC "The libwebp include directory" )

find_library( WEBP_LIBRARY
              NAMES libwebp.so
              PATHS /usr/lib /lib
              DOC "The libwebp library" )

if( WEBP_INCLUDE_DIR AND WEBP_LIBRARY )
    set( WEBP_FOUND 1 )
else()
    set( WEBP_FOUND 0 )
endif()

mark_as_advanced( WEBP_INCLUDE_DIR WEBP_LIBRARY )
