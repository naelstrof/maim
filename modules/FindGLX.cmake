# Try to find GLX. Once done, this will define:
#
#   GLX_FOUND - variable which returns the result of the search
#   GLX_INCLUDE_DIRS - list of include directories
#   GLX_LIBRARIES - options for the linker

#=============================================================================
# Copyright 2012 Benjamin Eikel
#
# Distributed under the OSI-approved BSD License (the "License");
# see accompanying file Copyright.txt for details.
#
# This software is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the License for more information.
#=============================================================================
# (To distribute this file outside of CMake, substitute the full
#  License text for the above reference.)

find_package(PkgConfig)
pkg_check_modules(PC_GLX QUIET glx)

find_path(GLX_INCLUDE_DIR
    GL/glx.h
    HINTS ${PC_GLX_INCLUDEDIR} ${PC_GLX_INCLUDE_DIRS}
)
find_library(GLX_LIBRARY
    GL
    HINTS ${PC_GLX_LIBDIR} ${PC_GLX_LIBRARY_DIRS}
)

set(GLX_INCLUDE_DIRS ${GLX_INCLUDE_DIR})
set(GLX_LIBRARIES ${GLX_LIBRARY})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GLX DEFAULT_MSG
    GLX_INCLUDE_DIR
    GLX_LIBRARY
)

mark_as_advanced(
    GLX_INCLUDE_DIR
    GLX_LIBRARY
)
