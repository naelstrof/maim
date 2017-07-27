# - Try to find JsonGlib-1.0
# Once done, this will define
#
#  JSONGLIB_FOUND - system has Glib
#  JSONGLIB_INCLUDE_DIRS - the Glib include directories
#  JSONGLIB_LIBRARIES - link these to use Glib

# Main include dir
find_path(JSONGLIB_INCLUDE_DIR
  NAMES json-glib/json-glib.h
  PATH_SUFFIXES json-glib-1.0
)

# Finally the library itself
find_library(JSONGLIB_LIBRARY
  NAMES json-glib-1.0
)

if(JSONGLIB_FOUND)
  set(JSONGLIB_INCLUDE_DIRS ${JsonGlib_INCLUDE_DIR})
  set(JSONGLIB_LIBRARIES ${JsonGlib_LIBRARY})
endif(JSONGLIB_FOUND)


