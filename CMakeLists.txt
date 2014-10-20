cmake_minimum_required( VERSION 2.8 )

set( PROJECT_NAME   "maim" )
set( BIN_TARGET     "${PROJECT_NAME}" )
set( CMAKE_INSTALL_PREFIX "/usr/bin" )

project( ${PROJECT_NAME} )

# Linux compiler initialization.
if ( CMAKE_COMPILER_IS_GNUCXX )
    set( CMAKE_CXX_FLAGS
         "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -pedantic-errors" )
         # -Wall:   Enable all warnings.
         # -Wextra: Enable some more warnings.
         # -Werror: Have errors on warnings.
    add_definitions( -D_CMAKE_LINUX_ )
else()
    message( FATAL_ERROR "Your operating system isn't supported yet! CMake will now exit." )
endif()

# Add a check target for our makefile.
find_program( CPPCHECK_EXECUTABLE cppcheck
              DOC "A tool for static C/C++ code analysis." )
if (CPPCHECK_EXECUTABLE)
    add_custom_target( "check"
                       COMMAND "${CPPCHECK_EXECUTABLE}" "--enable=all" "*"
                       WORKING_DIRECTORY src VERBATIM )
endif()

# Sources
set( source
     src/cmdline.c
     src/im.cpp
     src/x.cpp
     src/main.cpp )

# Obtain library paths and make sure they exist.
set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmakemodules" )
find_package( Imlib2    REQUIRED )
find_package( X11       REQUIRED )
find_package( XRandr    REQUIRED )
find_package( XFixes    REQUIRED )

set( CMAKE_CXX_FLAGS
     "${CMAKE_CXX_FLAGS} ${CMAKE_IMLIB2_CXX_FLAGS}" )

# Includes
include_directories( ${IMLIB2_INCLUDE_DIR}
                     ${XRANDR_INCLUDE_DIR}
                     ${X11_INCLUDE_DIR}
                     ${XFIXES_INCLUDE_DIR} )

# Executable
add_executable( ${BIN_TARGET} ${source} )

# Libraries
target_link_libraries( ${BIN_TARGET}
                       ${IMLIB2_LIBRARIES}
                       ${X11_LIBRARIES}
                       ${XRANDR_LIBRARY}
                       ${XFIXES_LIBRARY} )

install( TARGETS ${BIN_TARGET}
         DESTINATION  ${CMAKE_INSTALL_PREFIX} )
