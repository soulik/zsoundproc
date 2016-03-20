#ifndef _LBDRAW_H_
#define _LBDRAW_H_


extern "C"
{
#include <X11/X.h>
#include <Xm/Xm.h>
#include <Xm/DrawingA.h>
}

#include <string>


class lbdraw
{
public:

   lbdraw(
      Widget parent,
      Widget bottom);

   ~lbdraw();

   void
   after_realize();

   void
   setTime(
      int  seconds);

   void
   setRecordMode(
      bool on);

   Widget
   getWidget() const;

   static void
   expose(
      Widget  w,
      caddr_t client_data,
      XmDrawingAreaCallbackStruct  *draw_struct);


private:

   lbdraw();

Widget   _lb_draw;
static GC       _gc;
static string   _time_string;
static bool     _record_mode;
static Pixel    _black_pixel;
static Pixel    _white_pixel;
static Pixel    _red_pixel;
static Pixel    _yellow_pixel;
static Font     _font1;
static Font     _font2;

};

#endif
