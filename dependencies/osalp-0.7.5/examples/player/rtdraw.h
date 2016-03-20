#ifndef _RTDRAW_H_
#define _RTDRAW_H_


extern "C"
{
#include <X11/X.h>
#include <Xm/Xm.h>
#include <Xm/DrawingA.h>
}

#include <string>


class rtdraw
{
public:

   rtdraw(
      Widget parent,
      Widget left,
      Widget bottom);

   ~rtdraw();

   void
   after_realize();

   Widget
   getWidget() const;

   void
   plot_spectrum(
      int array_size,
      double *db_array);

   static void
   expose(
      Widget  w,
      caddr_t client_data,
      XmDrawingAreaCallbackStruct  *draw_struct);


private:

   rtdraw();

Widget   _rt_draw;
static GC       _gc;
static Pixel    _black_pixel;
static Pixel    _white_pixel;
static Pixel    _red_pixel;
static int      _array_size;
static double   _db_array[32];

};

#endif
