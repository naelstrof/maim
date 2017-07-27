# - Find XComposite
# Find the XComposite libraries
#
#  This module defines the following variables:
#     XCOMPOSITE_FOUND        - 1 if XCOMPOSITE_INCLUDE_DIR & XCOMPOSITE_LIBRARY are found, 0 otherwise
#     XCOMPOSITE_INCLUDE_DIR  - where to find Xlib.h, etc.
#     XCOMPOSITE_LIBRARY      - the X11 library
#

find_path( XCOMPOSITE_INCLUDE_DIR
           NAMES X11/extensions/Xcomposite.h
           PATH_SUFFIXES X11/extensions
           DOC "The XComposite include directory" )

find_library( XCOMPOSITE_LIBRARY
              NAMES Xcomposite
              PATHS /usr/lib /lib
              DOC "The XComposite library" )

if( XCOMPOSITE_INCLUDE_DIR AND XCOMPOSITE_LIBRARY )
    set( XCOMPOSITE_FOUND 1 )
else()
    set( XCOMPOSITE_FOUND 0 )
endif()

mark_as_advanced( XCOMPOSITE_INCLUDE_DIR XCOMPOSITE_LIBRARY )
