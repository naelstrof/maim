# Copyright (c) 2009, Whispersoft s.r.l.
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are
# met:
#
# * Redistributions of source code must retain the above copyright
# notice, this list of conditions and the following disclaimer.
# * Redistributions in binary form must reproduce the above
# copyright notice, this list of conditions and the following disclaimer
# in the documentation and/or other materials provided with the
# distribution.
# * Neither the name of Whispersoft s.r.l. nor the names of its
# contributors may be used to endorse or promote products derived from
# this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
# OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
# Finds Pango library
#
#  PANGO_INCLUDE_DIR - where to find pango.h, etc.
#  PANGO_LIBRARIES   - List of libraries when using Pango.
#  PANGO_FOUND       - True if Pango found.
#

if (PANGO_INCLUDE_DIR)
  # Already in cache, be silent
  set(PANGO_FIND_QUIETLY TRUE)
endif (PANGO_INCLUDE_DIR)

find_path(PANGO_INCLUDE_DIR pango/pango.h
  /opt/local/include
  /usr/local/include
  /usr/include
  /opt/local/include/pango-1.0
  /usr/local/include/pango-1.0
  /usr/include/pango-1.0
)

set(PANGO_NAMES pango-1.0)
find_library(PANGO_LIBRARY
  NAMES ${PANGO_NAMES}
  PATHS /usr/lib /usr/local/lib /opt/local/lib
)

if (PANGO_INCLUDE_DIR AND PANGO_LIBRARY)
   set(PANGO_FOUND TRUE)
   set( PANGO_LIBRARIES ${PANGO_LIBRARY} )
else (PANGO_INCLUDE_DIR AND PANGO_LIBRARY)
   set(PANGO_FOUND FALSE)
   set(PANGO_LIBRARIES)
endif (PANGO_INCLUDE_DIR AND PANGO_LIBRARY)

if (PANGO_FOUND)
   if (NOT PANGO_FIND_QUIETLY)
      message(STATUS "Found PANGO: ${PANGO_LIBRARY}")
   endif (NOT PANGO_FIND_QUIETLY)
else (PANGO_FOUND)
   if (PANGO_FIND_REQUIRED)
      message(STATUS "Looked for PANGO libraries named ${PANGO_NAMES}.")
      message(STATUS "Include file detected: [${PANGO_INCLUDE_DIR}].")
      message(STATUS "Lib file detected: [${PANGO_LIBRARY}].")
      message(FATAL_ERROR "=========> Could NOT find PANGO library")
   endif (PANGO_FIND_REQUIRED)
endif (PANGO_FOUND)

mark_as_advanced(
  PANGO_LIBRARY
  PANGO_INCLUDE_DIR
  )
