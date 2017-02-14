# - Find CImg
# Find CImg header.
#
#  CIMG_INCLUDE_DIRS - where to find CImg uncludes.
#  CIMG_FOUND        - True if CImg found.

# Look for the header file.
FIND_PATH(CIMG_INCLUDE_DIR NAMES CImg/CImg.h CImg.h)

# handle the QUIETLY and REQUIRED arguments and set TBB_FOUND to TRUE if
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(CIMG DEFAULT_MSG CIMG_INCLUDE_DIR)

# Copy the results to the output variables.
IF(CIMG_FOUND)
  SET(CIMG_INCLUDE_DIRS ${CIMG_INCLUDE_DIR})
ELSE(CIMG_FOUND)
  SET(CIMG_INCLUDE_DIRS)
ENDIF(CIMG_FOUND)

MARK_AS_ADVANCED(CIMG_INCLUDE_DIR)
