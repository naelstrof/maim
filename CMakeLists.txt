cmake_minimum_required( VERSION 3.1.3 )

project(maim VERSION 5.6.3 LANGUAGES CXX)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "/usr" CACHE PATH "..." FORCE)
endif()

include(GNUInstallDirs)

add_definitions(-DMAIM_VERSION="v${PROJECT_VERSION}")

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

set_property(TARGET ${BIN_TARGET} PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET ${BIN_TARGET} PROPERTY CXX_STANDARD 11)
set_property(TARGET ${BIN_TARGET} PROPERTY CXX_EXTENSIONS OFF)

# Includes
include_directories( ${XRANDR_INCLUDE_DIR}
                     ${X11_INCLUDE_DIR}
                     ${SLOP_INCLUDE_DIR}
                     ${XFIXES_INCLUDE_DIR}
                     ${XCOMPOSITE_INCLUDE_DIR}
                     ${JPEG_INCLUDE_DIR}
                     ${XRANDR_INCLUDE_DIR}
                     ${XRENDER_INCLUDE_DIR}
                     ${PNG_INCLUDE_DIRS} )

# Libraries
target_link_libraries( ${BIN_TARGET}
                       ${CMAKE_THREAD_LIBS_INIT}
                       ${X11_LIBRARIES}
                       ${PNG_LIBRARIES}
                       ${XFIXES_LIBRARY}
                       ${XCOMPOSITE_LIBRARY}
                       ${XRANDR_LIBRARY}
                       ${JPEG_LIBRARIES}
                       ${XRENDER_LIBRARY}
                       ${SLOP_LIBRARIES} )

if( ${CMAKE_VERSION} VERSION_LESS 3.7 )
  message( WARNING "CMake version is below 3.7, CMake version >= 3.7 is required for unicode support." )
else()
  find_package(ICU COMPONENTS uc)
  set( MAIM_UNICODE TRUE CACHE BOOL "To enable or disable unicode support." )
  if ( MAIM_UNICODE AND ICU_FOUND )
      # ICU is required for old nvidia drivers to work for whatever reason.
      add_definitions(-DCXXOPTS_USE_UNICODE)
      include_directories( ${ICU_INCLUDE_DIR} )
      target_link_libraries( ${BIN_TARGET} ${ICU_UC_LIBRARIES} )
  endif()
endif()

install( TARGETS ${BIN_TARGET} DESTINATION "${CMAKE_INSTALL_BINDIR}" )
install( FILES "${CMAKE_SOURCE_DIR}/maim.1" DESTINATION  "${CMAKE_INSTALL_MANDIR}/man1" COMPONENT doc )
