# - Find XFixes
# Find the XFixes libraries
#
#  This module defines the following variables:
#     SHADOWFACTORY_FOUND        - 1 if SHADOWFACTORY_INCLUDE_DIR & SHADOWFACTORY_LIBRARY are found, 0 otherwise
#     SHADOWFACTORY_INCLUDE_DIR  - where to find Xlib.h, etc.
#     SHADOWFACTORY_LIBRARY      - the X11 library
#

find_path( SHADOWFACTORY_INCLUDE_DIR
           NAMES meta/meta-shadow-factory.h
           HINTS /usr/include/mutter
           DOC "The mutter meta include directory" )

find_library( SHADOWFACTORY_LIBRARY
              NAMES mutter-0
              PATHS /usr/lib /lib
              DOC "The mutter library" )

if( SHADOWFACTORY_INCLUDE_DIR AND SHADOWFACTORY_LIBRARY )
    set( SHADOWFACTORY_FOUND 1 )
else()
    set( SHADOWFACTORY_FOUND 0 )
endif()

mark_as_advanced( SHADOWFACTORY_INCLUDE_DIR SHADOWFACTORY_LIBRARY )
