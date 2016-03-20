// This file contains no license whatsoever. It is provided in the public domain as an example
// of how to use the audio library
//
// Bruce Forsberg
// forsberg@tns.net
// 
//


#include <stdio.h>
#include <stdlib.h>

#include "ltdraw.h"

#define DRAW_WIDTH  150
#define DRAW_HEIGHT  60
#define CHAN_FONT "-*-fixed-*-*-*-*-12-*-*-*-*-*-*-*"

string ltdraw::_bits_string = "";
string ltdraw::_chan_string = "";
string ltdraw::_file_string = "";
string ltdraw::_format_string = "";
GC     ltdraw::_gc;
Pixel  ltdraw::_black_pixel;
Pixel  ltdraw::_white_pixel;
Pixel  ltdraw::_yellow_pixel;
Font   ltdraw::_font2;


ltdraw::ltdraw(
   Widget parent,
   Widget bottom)
{
   Arg  args[20];
   int  n;
   XColor  yellow;
   Display *dpy;


   dpy = XtDisplay(parent);

   _black_pixel = BlackPixelOfScreen(XtScreen(parent));
   _white_pixel = WhitePixelOfScreen(XtScreen(parent));

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
   _lt_draw = XmCreateDrawingArea(parent, "Draw LB", args, n);
   XtAddCallback(_lt_draw, XmNexposeCallback, (void (*)(_WidgetRec*, void*, void*))ltdraw::expose, NULL);
   XtManageChild(_lt_draw);
}

ltdraw::~ltdraw()
{
}

void
ltdraw::after_realize()
{
   XGCValues gcv;
   gcv.foreground = _white_pixel;
   gcv.background = _black_pixel;

   _gc = XCreateGC (XtDisplay(_lt_draw), XtWindow(_lt_draw),
      GCForeground | GCBackground, &gcv);

   _font2 = XLoadFont(XtDisplay(_lt_draw), CHAN_FONT);
}

void
ltdraw::setChannels(int chan)
{
   char buf[100];

   sprintf(buf, "Channels: %d", chan);
   _chan_string = buf;

   expose(_lt_draw, NULL, NULL);
}

void
ltdraw::setBits(int bits)
{
   char buf[100];

   sprintf(buf, "Bits: %d", bits);
   _bits_string = buf;

   expose(_lt_draw, NULL, NULL);
}

void
ltdraw::setFile(string& file)
{
   _file_string = file;

   expose(_lt_draw, NULL, NULL);
}

void
ltdraw::setFormat(const string& format)
{
   _format_string = "Format: ";
   _format_string.append(format);

   expose(_lt_draw, NULL, NULL);
}

Widget
ltdraw::getWidget() const
{
   return (_lt_draw);
}

void
ltdraw::expose(
   Widget  w,
   caddr_t client_data,
   XmDrawingAreaCallbackStruct  *draw_struct)
{
   Display *dis = XtDisplay(w);
   Window win = XtWindow(w);
   XTextItem text_item[1];
   string new_str;

   XSetForeground(dis, _gc, _black_pixel);
   XFillRectangle(dis, win, _gc, 0, 0, DRAW_WIDTH, DRAW_HEIGHT);
   XSetForeground(dis, _gc, _white_pixel);

   new_str = _chan_string;
   new_str.append(" ");
   new_str.append(_bits_string);
   // Write "Channels & Bits text"
   text_item[0].font = _font2;
   text_item[0].delta = 0;
   text_item[0].nchars = new_str.length();
   text_item[0].chars = (char *)new_str.c_str();
   XSetForeground(dis, _gc, _yellow_pixel);
   XDrawText(dis, win, _gc, 5, 10, text_item, 1);

   // Write "Format text"
   text_item[0].font = _font2;
   text_item[0].delta = 0;
   text_item[0].nchars = _format_string.length();
   text_item[0].chars = (char *)_format_string.c_str();
   XSetForeground(dis, _gc, _yellow_pixel);
   XDrawText(dis, win, _gc, 5, 20, text_item, 1);

   // Write "File text"
   text_item[0].font = _font2;
   text_item[0].delta = 0;
   text_item[0].nchars = _file_string.length();
   text_item[0].chars = (char *)_file_string.c_str();
   XSetForeground(dis, _gc, _yellow_pixel);
   XDrawText(dis, win, _gc, 5, 30, text_item, 1);

}

