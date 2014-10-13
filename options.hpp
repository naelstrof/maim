#ifndef MAIM_OPTIONS_H_

#include <string>
#include <cstring>
#include <cstdio>
#include <wordexp.h>

#include "x.hpp"

namespace maim {

class Options {
public:
                Options();
    int         parseOptions( int argc, char** argv );
    void        printHelp();

    bool        m_gotGeometry;
    bool        m_gotFile;
    bool        m_select;
    bool        m_hidecursor;
    float       m_delay;
    int         m_x;
    int         m_y;
    int         m_w;
    int         m_h;
    std::string m_version;
    std::string m_xdisplay;
    std::string m_file;
    std::string m_slopoptions;
    Window      m_window;
private:
    int         parseInt( std::string arg, int* returnInt );
    int         parseFloat( std::string arg, float* returnFloat );
    int         parseString( std::string arg, std::string* returnString );
    int         parseColor( std::string arg, float* r, float* g, float* b );
    int         parseGeometry( std::string arg, int* x, int* y, int* w, int* h );
    bool        matches( std::string arg, std::string shorthand, std::string longhand );
    bool        matches( std::string arg, std::string shorthand );
};

}

extern maim::Options* options;

#endif // MAIM_OPTIONS_H_
