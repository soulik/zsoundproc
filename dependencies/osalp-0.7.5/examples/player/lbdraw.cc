// This file contains no license whatsoever. It is provided in the public domain as an example
// of how to use the audio library
//
// Bruce Forsberg
// forsberg@tns.net
// 
//


#include <stdio.h>
#include <stdlib.h>

#include "lbdraw.h"

#define DRAW_WIDTH  150
#define DRAW_HEIGHT  60
#define RECORD_FONT "-*-fixed-*-*-*-*-12-*-*-*-*-*-*-*"
#define TIME_FONT "-*-fixed-*-*-*-*-20-*-*-*-*-*-*-*"

GC lbdraw::_gc;
Pixel  lbdraw::_black_pixel;
Pixel  lbdraw::_white_pixel;
Pixel  lbdraw::_red_pixel;
Pixel  lbdraw::_yellow_pixel;
string lbdraw::_time_string("00:00:00");
bool   lbdraw::_record_mode;
Font   lbdraw::_font1;
Font   lbdraw::_font2;


lbdraw::lbdraw(
   Widget parent,
   Widget bottom)
{
   Arg  args[20];
   int  n;
   XColor  red, yellow;
   Display *dpy;

   dpy = XtDisplay(parent);

   _black_pixel = BlackPixelOfScreen(XtScreen(parent));
   _white_pixel = WhitePixelOfScreen(XtScreen(parent));

   red.red = 65535;
   red.green = 0;
   red.blue = 0;
   XAllocColor(dpy, DefaultColormap(dpy, DefaultScreen(dpy)), &red);
   _red_pixel = red.pixel;

   yellow.red = 65535;
   yellow.green = 65535;
   yellow.blue = 0;
   XAllocColor(dpy, DefaultColormap(dpy, DefaultScreen(dpy)), &yellow);
   _yellow_pixel = yellow.pixel;

   n = 0;
   XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
   XtSetArg(args[n], XmNbottomAttachment, XmATTACH_WIDGET); n++;
   XtSetArg(args[n], XmNbottomWidget, bottom); n++;
   XtSetArg(args[n], XmNwidth, DRAW_WIDTH); n++;
   XtSetArg(args[n], XmNheight, DRAW_HEIGHT); n++;
   XtSetArg(args[n], XmNbackground, _black_pixel); n++;
   _lb_draw = XmCreateDrawingArea(parent, "Draw LB", args, n);
   XtAddCallback(_lb_draw, XmNexposeCallback, (void (*)(_WidgetRec*, void*, void*))lbdraw::expose, NULL);
   XtManageChild(_lb_draw);
}

lbdraw::~lbdraw()
{
}

void
lbdraw::after_realize()
{
   XGCValues gcv;
   gcv.foreground = _white_pixel;
   gcv.background = _black_pixel;

   _gc = XCreateGC (XtDisplay(_lb_draw), XtWindow(_lb_draw),
      GCForeground | GCBackground, &gcv);

   _font1 = XLoadFont(XtDisplay(_lb_draw), TIME_FONT);
   _font2 = XLoadFont(XtDisplay(_lb_draw), RECORD_FONT);
}

void
lbdraw::setTime(
   int  seconds)
{
   char  buf[100];
   static int pre_hours = 0;
   static int pre_minutes = 0;
   static int pre_seconds = 0;
   int hours, minutes;

   hours = (int)(seconds / 3600);
   seconds -= (hours * 3600);
   minutes = (int)(seconds / 60);
   seconds -= (minutes * 60);

   if ((hours == pre_hours) && (minutes == pre_minutes) &&
       (seconds == pre_seconds))
   {
   }
   else
   {
      sprintf(buf, "%2.2d:%2.2d:%2.2d", hours, minutes, seconds);
      _time_string = buf;

      expose( _lb_draw, NULL, NULL);

      pre_hours = hours;
      pre_minutes = minutes;
      pre_seconds = seconds;
   }
}

void
lbdraw::setRecordMode(
   bool  on)
{
   _record_mode = on;

   expose(_lb_draw, NULL, NULL);
}

Widget
lbdraw::getWidget() const
{
   return (_lb_draw);
}

void
lbdraw::expose(
   Widget  w,
   caddr_t client_data,
   XmDrawingAreaCallbackStruct  *draw_struct)
{
   Display *dis = XtDisplay(w);
   Window win = XtWindow(w);
   XTextItem text_item[1];

   XSetForeground(dis, _gc, _black_pixel);
   XFillRectangle(dis, win, _gc, 0, 0, DRAW_WIDTH, DRAW_HEIGHT);
   XSetForeground(dis, _gc, _white_pixel);

   // Draw rectangle and fill with red if recording
   XDrawRectangle(dis, win, _gc, 1, 1, 7, 7);  
   if (_record_mode == TRUE)
   {
      XSetForeground(dis, _gc, _red_pixel);
      XFillRectangle(dis, win, _gc, 2, 2, 6, 6);
      XSetForeground(dis, _gc, _white_pixel);
   }
   // Write "Record text"
   text_item[0].font = _font2;
   text_item[0].delta = 0;
   text_item[0].nchars = strlen("Record");
   text_item[0].chars = "Record";
   XSetForeground(dis, _gc, _white_pixel);
   XDrawText(dis, win, _gc, 15, 10, text_item, 1);

   // Draw current time
   text_item[0].font = _font1;
   text_item[0].delta = 0;
   text_item[0].nchars = strlen(_time_string.c_str());
   text_item[0].chars = (char *)_time_string.c_str();
   XSetForeground(dis, _gc, _yellow_pixel);
   XDrawText(dis, win, _gc, 10, DRAW_HEIGHT - 10, text_item, 1);
}

