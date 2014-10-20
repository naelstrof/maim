#
# This module finds if IMLIB2 is available and determines where the
# include files and libraries are.
# On Unix/Linux it relies on the output of imlib2-config.
# This code sets the following variables:
#
#
#
# IMLIB2_FOUND              = system has IMLIB2 lib
#
# IMLIB2_LIBRARIES          = full path to the libraries
#    on Unix/Linux with additional linker flags from "imlib2-config --libs"
#
# CMAKE_IMLIB2_CXX_FLAGS    = Unix compiler flags for IMLIB2, essentially "`imlib2-config --cxxflags`"
#
# IMLIB2_INCLUDE_DIR        = where to find headers
#
# IMLIB2_LINK_DIRECTORIES   = link directories, useful for rpath on Unix
#
#
# author Jan Woetzel and Jan-Friso Evers
# www.mip.informatik.uni-kiel.de/~jw

IF(WIN32)
    MESSAGE("FindIMLIB2.cmake: IMLIB2 not (yet) supported on WIN32")
    SET(IMLIB2_FOUND OFF	)
ELSE(WIN32)
    IF(UNIX)
        SET(IMLIB2_CONFIG_PREFER_PATH "$ENV{IMLIB2_HOME}/bin" CACHE STRING "preferred path to imlib2")
        FIND_PROGRAM(IMLIB2_CONFIG imlib2-config
                     ${IMLIB2_CONFIG_PREFER_PATH}
                     /usr/bin/
                     /opt/gnome/bin/)

        IF (IMLIB2_CONFIG)
            # OK, found imlib2-config.
            # set CXXFLAGS to be fed into CXX_FLAGS by the user:
            SET(IMLIB2_CXX_FLAGS "`${IMLIB2_CONFIG} --cflags`")

            # set INCLUDE_DIRS to prefix+include
            EXEC_PROGRAM(${IMLIB2_CONFIG}
            ARGS --prefix
            OUTPUT_VARIABLE IMLIB2_PREFIX)
            SET(IMLIB2_INCLUDE_DIR ${IMLIB2_PREFIX}/include CACHE STRING INTERNAL)

            # extract link dirs for rpath
            EXEC_PROGRAM(${IMLIB2_CONFIG}
                         ARGS --libs
                         OUTPUT_VARIABLE IMLIB2_CONFIG_LIBS)

            # set link libraries and link flags
            #SET(IMLIB2_LIBRARIES "`${IMLIB2_CONFIG} --libs`")
            SET(IMLIB2_LIBRARIES ${IMLIB2_CONFIG_LIBS})

            # split off the link dirs (for rpath)
            # use regular expression to match wildcard equivalent "-L*<endchar>"
            # with <endchar> is a space or a semicolon
            STRING(REGEX MATCHALL "[-][L]([^ ;])+"
                   IMLIB2_LINK_DIRECTORIES_WITH_PREFIX
                   "${IMLIB2_CONFIG_LIBS}")
            #MESSAGE("DBG  IMLIB2_LINK_DIRECTORIES_WITH_PREFIX=${IMLIB2_LINK_DIRECTORIES_WITH_PREFIX}")

            # remove prefix -L because we need the pure directory for LINK_DIRECTORIES
            # replace -L by ; because the separator seems to be lost otherwise (bug or feature?)
            IF (IMLIB2_LINK_DIRECTORIES_WITH_PREFIX)
                STRING(REGEX REPLACE "[-][L]" ";" IMLIB2_LINK_DIRECTORIES ${IMLIB2_LINK_DIRECTORIES_WITH_PREFIX} )
                #MESSAGE("DBG  IMLIB2_LINK_DIRECTORIES=${IMLIB2_LINK_DIRECTORIES}")
            ENDIF (IMLIB2_LINK_DIRECTORIES_WITH_PREFIX)

            # replace space separated string by semicolon separated vector to make 
            # it work with LINK_DIRECTORIES
            SEPARATE_ARGUMENTS(IMLIB2_LINK_DIRECTORIES)

            MARK_AS_ADVANCED(IMLIB2_CXX_FLAGS
                             IMLIB2_INCLUDE_DIR
                             IMLIB2_LIBRARIES
                             IMLIB2_LINK_DIRECTORIES
                             IMLIB2_CONFIG_PREFER_PATH
                             IMLIB2_CONFIG)

        ELSE(IMLIB2_CONFIG)
            MESSAGE( "FindIMLIB2.cmake: imlib2-config not found. Please set it manually. IMLIB2_CONFIG=${IMLIB2_CONFIG}")
        ENDIF(IMLIB2_CONFIG)
    ENDIF(UNIX)
ENDIF(WIN32)

IF(IMLIB2_LIBRARIES)
    IF(IMLIB2_INCLUDE_DIR OR IMLIB2_CXX_FLAGS)
        SET(IMLIB2_FOUND 1)
    ENDIF(IMLIB2_INCLUDE_DIR OR IMLIB2_CXX_FLAGS)
ENDIF(IMLIB2_LIBRARIES)
