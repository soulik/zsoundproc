#ifndef _RBDRAW_H_
#define _RBDRAW_H_


extern "C"
{
#include <X11/X.h>
#include <Xm/Xm.h>
#include <Xm/DrawingA.h>
}

#include <string>


class rbdraw
{
public:

   rbdraw(
      Widget parent,
      Widget left,
      Widget bottom);

   ~rbdraw();

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

   rbdraw();

Widget   _rb_draw;
static GC       _gc;
Pixel    _black_pixel;
Pixel    _white_pixel;
Pixel    _red_pixel;
static int      _array_size;
static double   _db_array[32];

};

#endif
