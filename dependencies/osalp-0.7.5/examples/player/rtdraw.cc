// This file contains no license whatsoever. It is provided in the public domain as an example
// of how to use the audio library
//
// Bruce Forsberg
// forsberg@tns.net
// 
//


#include <stdio.h>
#include <stdlib.h>

#include "rtdraw.h"

#define DRAW_WIDTH  150
#define DRAW_HEIGHT  60

GC rtdraw::_gc;
Pixel  rtdraw::_black_pixel;
Pixel  rtdraw::_white_pixel;
Pixel  rtdraw::_red_pixel;
int    rtdraw::_array_size = 0;
double rtdraw::_db_array[32];


rtdraw::rtdraw(
   Widget parent,
   Widget left,
   Widget bottom)
{
   Arg  args[20];
   int  n;
   XColor  red;
   Display *dpy;

   _array_size = 0;

   dpy = XtDisplay(parent);

   _black_pixel = BlackPixelOfScreen(XtScreen(parent));
   _white_pixel = WhitePixelOfScreen(XtScreen(parent));

   red.red = 65535;
   red.green = 0;
   red.blue = 0;
   XAllocColor(dpy, DefaultColormap(dpy, DefaultScreen(dpy)), &red);
   _red_pixel = red.pixel;

   n = 0;
   XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
   XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET); n++;
   XtSetArg(args[n], XmNleftWidget, left); n++;
   XtSetArg(args[n], XmNbottomAttachment, XmATTACH_WIDGET); n++;
   XtSetArg(args[n], XmNbottomWidget, bottom); n++;
   XtSetArg(args[n], XmNwidth, DRAW_WIDTH); n++;
   XtSetArg(args[n], XmNheight, DRAW_HEIGHT); n++;
   XtSetArg(args[n], XmNbackground, _black_pixel); n++;
   _rt_draw = XmCreateDrawingArea(parent, "Draw RT", args, n);
   XtAddCallback(_rt_draw, XmNexposeCallback, (void (*)(_WidgetRec*, void*, void*))rtdraw::expose, NULL);
   XtManageChild(_rt_draw);
}

rtdraw::~rtdraw()
{
}

void
rtdraw::after_realize()
{
   XGCValues gcv;
   gcv.foreground = _white_pixel;
   gcv.background = _black_pixel;

   _gc = XCreateGC (XtDisplay(_rt_draw), XtWindow(_rt_draw),
      GCForeground | GCBackground, &gcv);
}

Widget
rtdraw::getWidget() const
{
   return (_rt_draw);
}

void
rtdraw::plot_spectrum(
   int array_size,
   double *db_array)
{
   _array_size = array_size;

   memcpy(_db_array, db_array, sizeof(double) * array_size);

   rtdraw::expose(_rt_draw, NULL, NULL);
}

void
rtdraw::expose(
   Widget  w,
   caddr_t client_data,
   XmDrawingAreaCallbackStruct  *draw_struct)
{
   Display *dis = XtDisplay(w);
   Window win = XtWindow(w);
   XTextItem text_item[1];
   int  i;
   int width;

   XSetForeground(dis, _gc, _black_pixel);
   XFillRectangle(dis, win, _gc, 0, 0, DRAW_WIDTH, DRAW_HEIGHT);
   XSetForeground(dis, _gc, _white_pixel);

   // Write "Left Channel"
   text_item[0].font = None;
   text_item[0].delta = 0;
   text_item[0].nchars = strlen("Right Channel");
   text_item[0].chars = "Right Channel";
   XSetForeground(dis, _gc, _white_pixel);
   XDrawText(dis, win, _gc, 20, 10, text_item, 1);

   if (_array_size != 0)
   {
      width = DRAW_WIDTH / _array_size;
      XSetForeground(dis, _gc, _red_pixel);
      for (i = 0; i < _array_size-1; i++)
      {
         int db_int = (int)(_db_array[i]);

         db_int += 60;
         db_int = 60 - db_int;

         if (db_int > 60)
            db_int = 60;

         XFillRectangle(dis, win, _gc, i * width,
            db_int, width-1, DRAW_HEIGHT);

      }
   }
}

