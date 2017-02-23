# - Find XFixes
# Find the XFixes libraries
#
#  This module defines the following variables:
#     XFIXES_FOUND        - 1 if XFIXES_INCLUDE_DIR & XFIXES_LIBRARY are found, 0 otherwise
#     XFIXES_INCLUDE_DIR  - where to find Xlib.h, etc.
#     XFIXES_LIBRARY      - the X11 library
#

find_path( XFIXES_INCLUDE_DIR
           NAMES X11/extensions/Xfixes.h
           PATH_SUFFIXES X11/extensions
           DOC "The XFixes include directory" )

find_library( XFIXES_LIBRARY
              NAMES Xfixes
              PATHS /usr/lib /lib
              DOC "The XFixes library" )

if( XFIXES_INCLUDE_DIR AND XFIXES_LIBRARY )
    set( XFIXES_FOUND 1 )
else()
    set( XFIXES_FOUND 0 )
endif()

mark_as_advanced( XFIXES_INCLUDE_DIR XFIXES_LIBRARY )
