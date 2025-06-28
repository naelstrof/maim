# - Find SLOP
# Find the SLOP libraries
#
#  This module defines the following variables:
#     SLOP_FOUND        - 1 if SLOP_INCLUDE_DIR & SLOP_LIBRARY are found, 0 otherwise
#     SLOP_INCLUDE_DIR  - where to find Xlib.h, etc.
#     SLOP_LIBRARY      - the X11 library
#


find_path( SLOP_INCLUDE_DIR
           NAMES slop.hpp
           PATH_SUFFIXES /usr/include /include
           DOC "The SLOP include directory" )

find_library( SLOP_LIBRARY
              NAMES slopy slopy.so slop slop.so
              PATHS /usr/lib /lib
              DOC "The SLOP library" )

if( SLOP_INCLUDE_DIR AND SLOP_LIBRARY )
    set( SLOP_FOUND 1 )
else()
    set( SLOP_FOUND 0 )
endif()

mark_as_advanced( SLOP_INCLUDE_DIR SLOP_LIBRARY )
