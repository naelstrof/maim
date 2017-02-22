cmake_minimum_required( VERSION 2.8 )

project( "maim" )

set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build (Debug or Release)")

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
find_package( X11       REQUIRED )
find_package( SLOP      REQUIRED )

# Includes
include_directories( ${XRANDR_INCLUDE_DIR}
                     ${X11_INCLUDE_DIR}
                     ${SLOP_INCLUDE_DIR}
                     ${JPEG_INCLUDE_DIR}
                     ${PNG_INCLUDE_DIRS} )

# Libraries
target_link_libraries( ${BIN_TARGET}
                       ${X11_LIBRARIES}
                       ${PNG_LIBRARIES}
                       ${JPEG_LIBRARIES}
                       ${SLOP_LIBRARIES} )

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
