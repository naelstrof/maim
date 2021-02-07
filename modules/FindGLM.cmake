# - Find GLM
# Find the GLM libraries
#
#  This module defines the following variables:
#     GLM_FOUND       - 1 if GLM_INCLUDE_DIR is found, 0 otherwise
#     GLM_INCLUDE_DIR - where to find glm/glm.hpp
#

find_path( GLM_INCLUDE_DIR
           NAMES glm/glm.hpp
           PATH_SUFFIXES /usr/include /include
           DOC "The GLM include directory" )

if( GLM_INCLUDE_DIR )
    set( GLM_FOUND 1 )
else()
    set( GLM_FOUND 0 )
endif()

mark_as_advanced( GLM_INCLUDE_DIR )
