FIND_PATH(COGL_INCLUDE_DIRS cogl/cogl.h
#    $ENV{CAIRODIR}/include
#    $ENV{CAIRODIR}
#    ~/Library/Frameworks
#    /Library/Frameworks
    /usr/local/include
    /usr/include
#    /sw/include # Fink
#    /opt/local/include # DarwinPorts
#    /opt/csw/include # Blastwave
#    /opt/include
    PATH_SUFFIXES cogl
)