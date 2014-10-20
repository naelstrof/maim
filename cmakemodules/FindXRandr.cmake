# - Find XRandr
# Find the XRandr libraries
#
#  This module defines the following variables:
#     XRANDR_FOUND        - 1 if XRANDR_INCLUDE_DIR & XRANDR_LIBRARY are found, 0 otherwise
#     XRANDR_INCLUDE_DIR  - where to find Xlib.h, etc.
#     XRANDR_LIBRARY      - the X11 library
#

find_path( XRANDR_INCLUDE_DIR
           NAMES X11/extensions/Xrandr.h
           PATH_SUFFIXES X11/extensions
           DOC "The XRandr include directory" )

find_library( XRANDR_LIBRARY
              NAMES Xrandr
              PATHS /usr/lib /lib
              DOC "The XRandr library" )

if( XRANDR_INCLUDE_DIR AND XRANDR_LIBRARY )
    set( XRANDR_FOUND 1 )
else()
    set( XRANDR_FOUND 0 )
endif()

mark_as_advanced( XRANDR_INCLUDE_DIR XRANDR_LIBRARY )
