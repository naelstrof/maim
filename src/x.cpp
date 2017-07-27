#include "x.hpp"

static char _x_err = 0;
static int
TmpXError(Display * d, XErrorEvent * ev) {
    _x_err = 1;
    return 0;
}

glm::ivec4 getWindowGeometryWithoutBorder( X11* x11, Window win ) {
    XWindowAttributes attr;
    XGetWindowAttributes( x11->display, win, &attr );
    unsigned int width = attr.width;           
    unsigned int height = attr.height;         
    unsigned int border = attr.border_width;   
    int x, y;
    Window junk;
    XTranslateCoordinates( x11->display, win, attr.root, 0, 0, &x, &y, &junk );
    return glm::vec4( x, y, width, height );
} 

glm::ivec4 getWindowGeometry( X11* x11, Window win ) {
    XWindowAttributes attr;         
    XGetWindowAttributes( x11->display, win, &attr );
    unsigned int width = attr.width;           
    unsigned int height = attr.height;         
    unsigned int border = attr.border_width;   
    int x, y;
    Window junk;
    XTranslateCoordinates( x11->display, win, attr.root, -attr.border_width, -attr.border_width, &x, &y, &junk );
    width += border*2;
    height += border*2;
    return glm::vec4( x, y, width, height );
}

std::vector<XRRCrtcInfo*> X11::getCRTCS() {
    std::vector<XRRCrtcInfo*> monitors;                                            
    if ( !res ) {            
        return monitors;       
    }
    for ( int i=0;i<res->ncrtc;i++ ) {                                           
        monitors.push_back( XRRGetCrtcInfo( display, res, res->crtcs[ i ] ) );   
    }
    return monitors;           
}   
    
void X11::freeCRTCS( std::vector<XRRCrtcInfo*> monitors ) {              
    for ( unsigned int i=0;i<monitors.size();i++ ) {
        XRRFreeCrtcInfo( monitors[ i ] );
    }
}

X11::X11( std::string displayName ) {
    // Initialize display
    display = XOpenDisplay( displayName.c_str() );
    if ( !display ) {
        throw new std::runtime_error(std::string("Error: Failed to open X display: ") + displayName );
    }
    screen = ScreenOfDisplay( display, DefaultScreen( display ) );
    visual = DefaultVisual( display, XScreenNumberOfScreen( screen ) );
    root = DefaultRootWindow( display );
    int major = 0;
    int minor = 0;
    Bool pixmaps = true;
    haveXShm = XShmQueryVersion( display, &major, &minor, &pixmaps );
    haveXShm = (haveXShm && pixmaps );
    major = 0;
    minor = 0;
    haveXRR = XRRQueryVersion( display, &major, &minor );
    major = 0;
    minor = 2;
    XCompositeQueryVersion( display, &major, &minor );
    if ( major > 0 || minor >= 2 ) {
        haveXComposite = true;
    } else {
        haveXComposite = false;
    }
    if ( haveXRR ) {
        res = XRRGetScreenResourcesCurrent( display, root );
    }
}

X11::~X11() {
    XCloseDisplay( display );
}

XImage* X11::getImage( Window draw, int x, int y, int w, int h, glm::ivec2& imageloc ) {
    glm::ivec4 sourceGeo = getWindowGeometry( this, draw );
    // We need to clamp the selection to fit within the
    // provided window.

    x = glm::clamp( x, sourceGeo.x, sourceGeo.x+sourceGeo.z );
    y = glm::clamp( y, sourceGeo.y, sourceGeo.y+sourceGeo.w );
    w = glm::clamp( w, 1, sourceGeo.x+sourceGeo.z-x );
    h = glm::clamp( h, 1, sourceGeo.y+sourceGeo.w-y );

    imageloc = glm::ivec2( x, y );

    // Translate the newly clamped selection to local coordinates.
    int localx, localy;
    Window junk;
    XTranslateCoordinates( this->display, this->root, draw, x, y, &localx, &localy, &junk);

    if ( haveXComposite ) {
        // We redirect all the pixmaps offscreen, so that they won't be corrupted if obscured.
        for ( int i = 0; i < ScreenCount( display ); i++ ) {
            XCompositeRedirectSubwindows( display, RootWindow( display, i ), CompositeRedirectAutomatic );
        }
        // We don't have to worry about undoing the redirect, since as soon as maim closes X knows to undo it.
    }

    // We use XRender to grab the drawable, since it'll save it in a format we like.
    XWindowAttributes attr;
    XGetWindowAttributes( display, draw, &attr );
    XRenderPictFormat *format = XRenderFindVisualFormat( display, attr.visual );
    bool hasAlpha = ( format->type == PictTypeDirect && format->direct.alphaMask );
    XRenderPictureAttributes pa;
    pa.subwindow_mode = IncludeInferiors;
    Picture picture = XRenderCreatePicture( display, draw, format, CPSubwindowMode, &pa );
    XserverRegion region = findRegion( draw );
    // Also we use XRender because of this neato function here.
    XFixesSetPictureClipRegion( display, picture, 0, 0, region );
    XFixesDestroyRegion( display, region );

    Pixmap pixmap = XCreatePixmap(display, root, w, h, 32);
    XRenderPictureAttributes pa2;

    XRenderPictFormat *format2 = XRenderFindStandardFormat(display, PictStandardARGB32);
    Picture pixmapPicture = XRenderCreatePicture( display, pixmap, format2, 0, &pa2 );
    XRenderColor c;
    c.red = 0x0000;
    c.green = 0x0000;
    c.blue = 0x0000;
    c.alpha = 0x0000;
    XRenderFillRectangle (display, PictOpSrc, pixmapPicture, &c, 0, 0, w, h);
    XRenderComposite(display, hasAlpha ? PictOpOver : PictOpSrc, picture, 0,
                     pixmapPicture, localx, localy, 0, 0, 0, 0,
                     w, h);
    XImage* temp = XGetImage( display, pixmap, 0, 0, w, h, AllPlanes, ZPixmap );
    temp->red_mask = format2->direct.redMask << format2->direct.red;
    temp->green_mask = format2->direct.greenMask << format2->direct.green;
    temp->blue_mask = format2->direct.blueMask << format2->direct.blue;
    temp->depth = format2->depth;
    return temp;
}

bool X11::hasClipping( Window d ) {
    int bShaped, xbs, ybs, cShaped, xcs, ycs;
    unsigned int wbs, hbs, wcs, hcs;
    XShapeQueryExtents ( display, d, &bShaped, &xbs, &ybs, &wbs, &hbs, &cShaped, &xcs, &ycs, &wcs, &hcs );
    return bShaped || cShaped;
}

XserverRegion X11::findRegion( Window d ) {
    XserverRegion rootRegion = XFixesCreateRegionFromWindow( display, d, WindowRegionBounding );
    glm::vec4 rootgeo = getWindowGeometryWithoutBorder( this, d );
    XFixesTranslateRegion( display, rootRegion, rootgeo.x, rootgeo.y ); // Regions are in respect to the root window by default.
    unionClippingRegions( rootRegion, d );
    unionBorderRegions( rootRegion, d );
    return rootRegion;
}

void X11::unionBorderRegions( XserverRegion rootRegion, Window d ) {
    glm::vec4 bordergeo = getWindowGeometry( this, d );
    glm::vec4 geo = getWindowGeometryWithoutBorder( this, d );
    XRectangle* rects = new XRectangle[1];
    rects[0].x = bordergeo.x;
    rects[0].y = bordergeo.y;
    rects[0].width = bordergeo.z;
    rects[0].height = bordergeo.w;
    XserverRegion borderRegionRect = XFixesCreateRegion( display, rects, 1 );
    rects[0].x = geo.x;
    rects[0].y = geo.y;
    rects[0].width = geo.z;
    rects[0].height = geo.w;
    XserverRegion regionRect = XFixesCreateRegion( display, rects, 1 );
    XFixesSubtractRegion( display, regionRect, borderRegionRect, regionRect );
    XFixesUnionRegion( display, rootRegion, rootRegion, regionRect );
    XFixesDestroyRegion( display, regionRect );
    XFixesDestroyRegion( display, borderRegionRect );
}

void X11::unionClippingRegions( XserverRegion rootRegion, Window child ) {
    Window root, parent;
    Window* children;
    unsigned int num_children;
    XQueryTree( display, child, &root, &parent, &children, &num_children);
    for ( unsigned int i=0;i<num_children;i++ ) {
        if ( hasClipping( children[i] ) ) {
            Window clippingWindow = children[i];
            glm::vec4 geo = getWindowGeometryWithoutBorder( this, clippingWindow );
            XRectangle* rects = new XRectangle[1];
            rects[0].x = geo.x;
            rects[0].y = geo.y;
            rects[0].width = geo.z;
            rects[0].height = geo.w;
            // We have to keep the parent's region from interfering the clipping region.
            XserverRegion clippingWindowRect = XFixesCreateRegion( display, rects, 1 );
            // So we cut a neat hole the size of the child window where the clipping happens.
            XFixesSubtractRegion( display, rootRegion, rootRegion, clippingWindowRect );
            XFixesDestroyRegion( display, clippingWindowRect );
            XserverRegion childRegion = XFixesCreateRegionFromWindow( display, clippingWindow, WindowRegionBounding );
            XFixesTranslateRegion( display, childRegion, geo.x, geo.y ); // Regions are in respect to the root window by default.
            XFixesUnionRegion( display, rootRegion, rootRegion, childRegion );
            XFixesDestroyRegion( display, childRegion );
            // We don't desend deeper down the tree looking for more clipping regions, I just don't want to support
            // applications that do that...
        } else {
            unionClippingRegions( rootRegion, children[i] );
        }
    }
}
