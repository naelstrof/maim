cmake_minimum_required( VERSION 2.8 )

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

add_definitions(-DMAIM_VERSION="v4.4.47")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/")

# Sources
set( source
     src/x.cpp
     src/image.cpp
     src/options.cpp
     src/main.cpp )

set( BIN_TARGET     "${PROJECT_NAME}" )

# Executable
add_executable( "${BIN_TARGET}" ${source} )

# Obtain library paths and make sure they exist.
set( CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" "${CMAKE_SOURCE_DIR}/modules" )
find_package( PNG       REQUIRED )
find_package( JPEG      REQUIRED )
find_package( XRandr    REQUIRED )
find_package( XFixes    REQUIRED )
find_package( X11       REQUIRED )
find_package( SLOP      REQUIRED )

set_property(TARGET ${BIN_TARGET} PROPERTY CXX_STANDARD 11)
set_property(TARGET ${BIN_TARGET} PROPERTY CXX_STANDARD_REQUIRED ON)

# Includes
include_directories( ${XRANDR_INCLUDE_DIR}
                     ${X11_INCLUDE_DIR}
                     ${SLOP_INCLUDE_DIR}
                     ${XFIXES_INCLUDE_DIR}
                     ${JPEG_INCLUDE_DIR}
                     ${XRANDR_INCLUDE_DIR}
                     ${PNG_INCLUDE_DIRS} )

# Libraries
target_link_libraries( ${BIN_TARGET}
                       ${X11_LIBRARIES}
                       ${PNG_LIBRARIES}
                       ${XFIXES_LIBRARY}
                       ${XRANDR_LIBRARY}
                       ${JPEG_LIBRARIES}
                       ${SLOP_LIBRARIES} )

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")

install( TARGETS ${BIN_TARGET} DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" )
install( FILES "${CMAKE_SOURCE_DIR}/${MANTARGET}" DESTINATION  "${CMAKE_INSTALL_MANDIR}/man1" COMPONENT doc )
