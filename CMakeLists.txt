cmake_minimum_required( VERSION 2.8 )

project( "maim" )
set( maim_VERSION_MAJOR 2 )
set( maim_VERSION_MINOR 3 )
set( maim_VERSION_PATCH 35 )

set( BIN_TARGET     "${PROJECT_NAME}" )
set( CMAKE_INSTALL_PREFIX "/usr/bin" )

# Linux compiler initialization.
if ( "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" OR
     "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR
     "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel" )
    set( CMAKE_CXX_FLAGS
         "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -pedantic-errors -Wno-unused-parameter" )
         # -Wall:   Enable all warnings.
         # -Wextra: Enable some more warnings.
         # -Werror: Have errors on warnings.
         # -Wno-unused-parameter: Don't error on unused parameters, required since we have function hooks
         #                        that have unused parameters.
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
configure_file( "src/cmdline.in"
                "src/cmdline.h" )

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
         DESTINATION  "${CMAKE_INSTALL_PREFIX}" )
