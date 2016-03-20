#ifndef _LTDRAW_H_
#define _LTDRAW_H_


extern "C"
{
#include <X11/X.h>
#include <Xm/Xm.h>
#include <Xm/DrawingA.h>
}

#include <string>


class ltdraw
{
public:

   ltdraw(
      Widget parent,
      Widget bottom);

   ~ltdraw();

   void
   after_realize();

   void
   setChannels(int chan);

   void
   setBits(int bits);

   void
   setFile(string& file);

   void
   setFormat(const string& format);

   Widget
   getWidget() const;

   static void
   expose(
      Widget  w,
      caddr_t client_data,
      XmDrawingAreaCallbackStruct  *draw_struct);


private:

   ltdraw();

Widget   _lt_draw;
static string   _bits_string;
static string   _chan_string;
static string   _file_string;
static string   _format_string;
static GC       _gc;
static Pixel    _black_pixel;
static Pixel    _white_pixel;
static Pixel    _yellow_pixel;
static Font     _font2;

};

#endif
