cmake_minimum_required( VERSION 3.1.3 )

project( "maim" )

set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build (Debug or Release)")

if ( NOT CMAKE_INSTALL_PREFIX )
    set(CMAKE_INSTALL_PREFIX "/usr")
endif()

set( CMAKE_INSTALL_MANDIR "${CMAKE_INSTALL_PREFIX}/share/man" CACHE PATH "Directory where man pages reside. (/usr/share/man, /usr/local/share/man, etc.)" )

set( CMAKE_COMPRESS_MAN TRUE CACHE BOOL "Whether or not to compress the man pages for install." )

if ( CMAKE_COMPRESS_MAN )
    set( MANTARGET "maim.1.gz" )
else()
    set( MANTARGET "maim.1" )
endif()

add_definitions(-DMAIM_VERSION="v5.4.64")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/")

# Sources
set( source
     src/x.cpp
     src/image.cpp
     src/main.cpp )

set( BIN_TARGET     "${PROJECT_NAME}" )

# Executable
add_executable( "${BIN_TARGET}" ${source} )

# Obtain library paths and make sure they exist.
set( CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" "${CMAKE_SOURCE_DIR}/modules" )
find_package( PNG        REQUIRED )
find_package( JPEG       REQUIRED )
find_package( XRandr     REQUIRED )
find_package( XRender    REQUIRED )
find_package( XFixes     REQUIRED )
find_package( XComposite REQUIRED )
find_package( X11        REQUIRED )
find_package( SLOP       REQUIRED )
find_package( Threads    REQUIRED )

find_package( Meta       REQUIRED )
find_package( Cairo      REQUIRED )
find_package( Clutter    REQUIRED )
find_package( GLIB       REQUIRED )
find_package( Cogl       REQUIRED )
find_package( Pango      REQUIRED )
find_package( ATK        REQUIRED )
find_package( JsonGlib   REQUIRED )

set_property(TARGET ${BIN_TARGET} PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET ${BIN_TARGET} PROPERTY CXX_STANDARD 11)

# Includes
include_directories( ${XRANDR_INCLUDE_DIR}
                     ${X11_INCLUDE_DIR}
                     ${SLOP_INCLUDE_DIR}
                     ${XFIXES_INCLUDE_DIR}
                     ${XRENDER_INCLUDE_DIR}
                     ${XCOMPOSITE_INCLUDE_DIR}
                     ${JPEG_INCLUDE_DIR}
                     ${XRANDR_INCLUDE_DIR}
                     ${CAIRO_INCLUDE_DIRS}
                     ${GLIB_INCLUDE_DIRS}
                     ${CLUTTER_INCLUDE_DIR}
                     ${PANGO_INCLUDE_DIR}
                     ${ATK_INCLUDE_DIRS}
                     ${COGL_INCLUDE_DIRS}
                     ${JSONGLIB_INCLUDE_DIR}
                     ${Meta_INCLUDE_DIR}
                     ${PNG_INCLUDE_DIRS} )

# Libraries
target_link_libraries( ${BIN_TARGET}
                       ${CMAKE_THREAD_LIBS_INIT}
                       ${X11_LIBRARIES}
                       ${PNG_LIBRARIES}
                       ${XFIXES_LIBRARY}
                       ${XRENDER_LIBRARY}
                       ${XCOMPOSITE_LIBRARY}
                       ${XRANDR_LIBRARY}
                       ${CAIRO_LIBRARIES}
                       ${GLIB_LIBRARIES}
                       ${PANGO_LIBRARIES}
                       ${CLUTTER_LIBRARY}
                       ${ATK_LIBRARIES}
                       ${JPEG_LIBRARIES}
                       ${JSONGLIB_LIBRARY}
                       ${SLOP_LIBRARIES} )

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")

install( TARGETS ${BIN_TARGET} DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" )
install( FILES "${CMAKE_SOURCE_DIR}/${MANTARGET}" DESTINATION  "${CMAKE_INSTALL_MANDIR}/man1" COMPONENT doc )
