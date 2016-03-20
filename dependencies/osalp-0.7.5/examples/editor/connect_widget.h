#ifndef _CONNECT_WIDGET_H
#define _CONNECT_WIDGET_H


extern "C"
{
#include <X11/X.h>
#include <Xm/Xm.h>
#include <X11/extensions/Xext.h>
#include <X11/extensions/Xdbe.h>
#include <Xm/DrawingA.h>
}

#define  WIDGET_NUM_COLORS   6


class aflibAudioEdit;

class connect_widget {

public:

   connect_widget(
      Widget parent,
      Widget left,
      Widget right,
      Widget top,
      Widget bottom,
      int    height,
      int    width);

   ~connect_widget();

   Widget
   getWidget() const;

   void
   set_parameters(
      long long       o_start,
      long long       o_stop,
      long long       i_start,
      long long       i_stop,
      aflibAudioEdit *edit,
      int             id);

   void
   perform_expose();

   static void
   connect_widget::expose(
      Widget  w,
      caddr_t client_data,
      XmDrawingAreaCallbackStruct  *draw_struct);

protected:

private:

   void
   build_widget(
      Widget parent,
      Widget left,
      Widget right,
      Widget top,
      Widget bottom);

   void
   create_gc();


int        _width;
int        _height;
long long  _o_start;
long long  _o_stop;
long long  _i_start;
long long  _i_stop;

// Drawing structures
Widget  _w;
GC      _gc;
bool    _init_gc;
XPoint  *_pts;
int     _npts;

// Color support for drawing
Pixel   _black_pixel;
Pixel   _white_pixel;
Pixel   _color_pixel[WIDGET_NUM_COLORS];

// back buffering support
Drawable      _back_buf;
XdbeSwapInfo  _back_info;
bool          _disable_back;

};


#endif
