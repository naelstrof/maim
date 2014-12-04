cmake_minimum_required( VERSION 2.8 )

project( "maim" )
set( maim_VERSION_MAJOR 3 )
set( maim_VERSION_MINOR 3 )
set( maim_VERSION_PATCH 39 )

set( BIN_TARGET     "${PROJECT_NAME}" )
if( NOT CMAKE_INSTALL_PREFIX )
   set( CMAKE_INSTALL_PREFIX "/usr" )
endif()

if ( NOT CMAKE_INSTALL_MANDIR )
    set( CMAKE_INSTALL_MANDIR "/usr/share/man" )
endif()

if( NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE )
   set( CMAKE_BUILD_TYPE RelWithDebInfo )
endif()

# Linux compiler initialization.
if ( "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" OR
     "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR
     "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel" )
    set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-unused-parameter" )
    set( CMAKE_CXX_FLAGS_DEBUG "-Wextra -pedantic-errors -O0 -g" )
    set( CMAKE_CXX_FLAGS_RELEASE "-O2" )
    set( CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g" )
         # -Wall:   Enable all warnings.
         # -Wextra: Enable some more warnings.
         # -Werror: Have errors on warnings.
         # -pedantic-errors: Even more errors.
         # -Wno-unused-parameter: Don't error on unused parameters, required since we have function hooks
         #                        that have unused parameters.
         # -O#: Optimization level
else()
    message( FATAL_ERROR "Your operating system isn't supported yet! CMake will now exit." )
endif()

# Add a check target for our makefile.
find_program( CPPCHECK_EXECUTABLE cppcheck
              DOC "A tool for static C/C++ code analysis." )
if ( CPPCHECK_EXECUTABLE )
    add_custom_target( "check"
                       COMMAND "${CPPCHECK_EXECUTABLE}" "--enable=all" "*"
                       WORKING_DIRECTORY src VERBATIM )
endif()

# Add our manpage generator if possible. Not needed as we include the man pages by default.
find_program( RONN_EXECUTABLE ronn
              DOC "A tool for generating our manpages." )
find_program( GZIP_EXECUTABLE gzip
              DOC "A tool for generating our manpages." )
if ( RONN_EXECUTABLE AND GZIP_EXECUTABLE )
    add_custom_target( "man"
                       COMMAND "${RONN_EXECUTABLE}" "-r" "maim.1.ronn"
                       COMMAND "${GZIP_EXECUTABLE}" "maim.1"
                       WORKING_DIRECTORY man-src VERBATIM )
endif()

# Here we generate some of our code if we can. I package it pre-generated
# so nobody has to go find and install gengetopt if they don't want to.
find_program( GENGETOPT_EXECUTABLE gengetopt
              DOC "A tool to generate code to grab command line options." )
if ( GENGETOPT_EXECUTABLE )
    message( "-- Regenerating cmdline.in" )
    execute_process( COMMAND "${GENGETOPT_EXECUTABLE}" "--input=options.ggo"
                     WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/src" )
    file( RENAME "${CMAKE_SOURCE_DIR}/src/cmdline.h" "${CMAKE_SOURCE_DIR}/src/cmdline.in" )
else()
    message( "Warning: Command gengetopt not found! Won't regenerate command line code. (If you're just compiling this doesn't matter.)" )
endif()

# By default our src/options.ggo has our cmake versions variables for
# the 'version ""' line. We replace them here.
# The ${CMAKE_SOURCE_DIR} is there to fix problems with OpenBSD's out-of-source build black magic.
configure_file( "src/cmdline.in" "${CMAKE_SOURCE_DIR}/src/cmdline.h" )

# This allows for "make README.md" to be ran to update the README's help
# section automatically. We don't add it to ALL because running arbitrary
# scripts is unsafe and I don't know if systems will actually have it
# be executbable.
add_custom_target( README.md "./generateReadme.sh" DEPENDS "maim" )

# Sources
set( source
     src/cmdline.c
     src/im.cpp
     src/x.cpp
     src/main.cpp )

# Obtain library paths and make sure they exist.
set( CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" "${CMAKE_SOURCE_DIR}/cmakemodules" )
find_package( Imlib2    REQUIRED )
find_package( X11       REQUIRED )
find_package( XRandr    REQUIRED )
find_package( XFixes    REQUIRED )

set( CMAKE_CXX_FLAGS
     "${CMAKE_CXX_FLAGS} ${CMAKE_IMLIB2_CXX_FLAGS}" )

# Includes
include_directories( "${IMLIB2_INCLUDE_DIR}"
                     "${XRANDR_INCLUDE_DIR}"
                     "${X11_INCLUDE_DIR}"
                     "${XFIXES_INCLUDE_DIR}" )

# Executable
add_executable( "${BIN_TARGET}" ${source} )

# Libraries
target_link_libraries( "${BIN_TARGET}"
                       ${IMLIB2_LIBRARIES}
                       ${X11_LIBRARIES}
                       "${XRANDR_LIBRARY}"
                       "${XFIXES_LIBRARY}" )

install( TARGETS "${BIN_TARGET}"
         DESTINATION  "${CMAKE_INSTALL_PREFIX}/bin" )

install( FILES "${CMAKE_SOURCE_DIR}/man-src/maim.1.gz"
         DESTINATION  "${CMAKE_INSTALL_MANDIR}/man1"
         COMPONENT doc )
