cmake_minimum_required( VERSION 2.8 )

project( "maim" )

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/")

# Sources
set( source
     src/x.cpp
     src/cimg.cpp
     src/options.cpp
     src/main.cpp )

set( BIN_TARGET     "${PROJECT_NAME}" )

# Executable
add_executable( "${BIN_TARGET}" ${source} )

# Obtain library paths and make sure they exist.
set( CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" "${CMAKE_SOURCE_DIR}/modules" )
find_package( CImg      REQUIRED )
find_package( XRandr    REQUIRED )
find_package( X11       REQUIRED )
find_package( SLOP      REQUIRED )

# Includes
include_directories( ${XRANDR_INCLUDE_DIR}
                     ${X11_INCLUDE_DIR}
                     ${SLOP_INCLUDE_DIR}
                     ${CImg_INCLUDE_DIRS} )

# Libraries
target_link_libraries( ${BIN_TARGET}
                       ${X11_LIBRARIES}
                       ${SLOP_LIBRARIES} )

find_package(Threads REQUIRED)
if(THREADS_HAVE_PTHREAD_ARG)
  target_compile_options(PUBLIC ${BIN_TARGET} "-pthread")
endif()
if(CMAKE_THREAD_LIBS_INIT)
  target_link_libraries(${BIN_TARGET} "${CMAKE_THREAD_LIBS_INIT}")
endif()
