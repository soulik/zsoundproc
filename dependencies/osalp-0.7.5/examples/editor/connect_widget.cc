// This file contains no license whatsoever. It is provided in the public domain as an example
// of how to use the audio library
//
// Bruce Forsberg
// forsberg@tns.net
// 
//

#include <iostream.h>
#include <stdio.h>
#include "connect_widget.h"

#include "aflibAudioEdit.h"

connect_widget::connect_widget(
   Widget  parent,
   Widget  left,
   Widget  right,
   Widget  top,
   Widget  bottom,
   int     height,
   int     width)
{
   _height   = height;
   _width    = width;
   _init_gc  = FALSE;
   _back_buf = 0;
   _o_start  = 0;
   _o_stop   = 0;
   _i_start  = 0;
   _i_stop   = 0;
   _npts     = 0;
   _pts      = NULL;
   // We don't need double buffering for this widget
   _disable_back = TRUE;

   build_widget(parent, left, right, top, bottom);
}

connect_widget::~connect_widget()
{
   delete [] _pts;
}


void
connect_widget::perform_expose()
{
   // This function will perform an expose of the widget

   expose(_w, (caddr_t)this, NULL);
}


Widget
connect_widget::getWidget() const
{
   return (_w);
}


void
connect_widget::set_parameters(
   long long       o_start,
   long long       o_stop,
   long long       i_start,
   long long       i_stop,
   aflibAudioEdit *edit,
   int             id)
{
   int new_id, ii;
   long long ii_start, ii_stop, oo_start, oo_stop;
   long long in_start, in_stop, ou_start, ou_stop;
   double    factor;
   int       num_pts = 0;


   _o_start = o_start;
   _o_stop  = o_stop;
   _i_start = i_start;
   _i_stop  = i_stop;

   if (edit)
   {
      _npts = edit->getNumberOfSegments();

      delete [] _pts;
      _pts = new XPoint[4 * _npts];

      for (ii = 0; ii < _npts; ii++)
      {
         edit->getSegment(ii + 1, new_id,
            ii_start, ii_stop, oo_start, oo_stop, factor);

         if (new_id == id)
         {
            ou_start = _width * oo_start / o_stop;
            ou_stop  = _width * oo_stop / o_stop;
            in_start = _width * ii_start / i_stop;
            in_stop  = _width * ii_stop / i_stop;

            _pts[num_pts * 4 + 0].x = (short)ou_start;   
            _pts[num_pts * 4 + 0].y = 0;   
            _pts[num_pts * 4 + 1].x = (short)in_start;   
            _pts[num_pts * 4 + 1].y = _height;   
            _pts[num_pts * 4 + 2].x = (short)in_stop;   
            _pts[num_pts * 4 + 2].y = _height;   
            _pts[num_pts * 4 + 3].x = (short)ou_stop;   
            _pts[num_pts * 4 + 3].y = 0;
            num_pts++;
         }
      }
   }

   _npts = num_pts;
   perform_expose();
}


void
connect_widget::expose(
   Widget  w,
   caddr_t client_data,
   XmDrawingAreaCallbackStruct  *draw_struct)
{
   Display *dis = XtDisplay(w);
   connect_widget *obj = (connect_widget *)client_data;
   Window  win;
   int     i, ii;


   // We must initialize GC after toplevel is realized. Thus we will do it after
   // first exposure.
   if (obj->_init_gc == FALSE)
   {
      obj->create_gc();
   }

   // IS back buffer active. If not then use front buffer (ie widget)
   if (obj->_back_buf == 0)
      win = XtWindow(w);
   else
      win = obj->_back_buf;

   XSetForeground(dis, obj->_gc, obj->_black_pixel);
   XFillRectangle(dis, win, obj->_gc, 0, 0, obj->_width, obj->_height);

   for (i = 0; i < obj->_npts; i++)
   {
      ii = i % WIDGET_NUM_COLORS;
      XSetForeground(dis, obj->_gc, obj->_color_pixel[ii]);
      XFillPolygon(dis, win, obj->_gc, &(obj->_pts[i * 4]), 4, Complex, CoordModeOrigin);
   }

   // IF back buffering is enabled then swap
   if (obj->_back_buf != 0)
      XdbeSwapBuffers(dis, &(obj->_back_info), 1);

   XSetForeground(dis, obj->_gc, obj->_white_pixel);
}


void
connect_widget::create_gc()
{
   /* Need to do this after toplevel is realized */
   XGCValues gcv;
   gcv.foreground = _white_pixel;
   gcv.background = _black_pixel;
   int  minor_ver, major_ver;

   // should back buffering be enabled
   if (_disable_back != TRUE)
   {
   // Use double buffering for smooth display
   if (_back_buf == 0)
   {
      // Does server support double buffering
      if (0 != XdbeQueryExtension(XtDisplay(_w), &major_ver, &minor_ver))
      {
         XdbeBackBufferAttributes *attr;

         _back_buf = XdbeAllocateBackBufferName(XtDisplay(_w), XtWindow(_w), XdbeUndefined);
         attr = XdbeGetBackBufferAttributes(XtDisplay(_w), _back_buf);
         _back_info.swap_window = attr->window;
         _back_info.swap_action = XdbeUndefined;
      }
      else
      {
         cout << "Server does not support Double Buffering! Xdbe extension not active" << endl;
      }
   }
   }

   // Was back buffer activated
   if (_back_buf == 0)
      _gc = XCreateGC (XtDisplay(_w), XtWindow(_w), GCForeground | GCBackground, &gcv);
   else
      _gc = XCreateGC (XtDisplay(_w), _back_buf, GCForeground | GCBackground, &gcv);

   // We are initialized
   _init_gc = TRUE;
}


void
connect_widget::build_widget(
   Widget parent,
   Widget left,
   Widget right,
   Widget top,
   Widget bottom)
{
   Arg  args[20];
   int  n;
   XColor  color;
   Display *dpy;

   dpy = XtDisplay(parent);

   _black_pixel = BlackPixelOfScreen(XtScreen(parent));
   _white_pixel = WhitePixelOfScreen(XtScreen(parent));

   color.red = 65535;
   color.green = 0;
   color.blue = 0;
   XAllocColor(dpy, DefaultColormap(dpy, DefaultScreen(dpy)), &color);
   _color_pixel[0] = color.pixel;

   color.red = 0;
   color.green = 65535;
   color.blue = 0;
   XAllocColor(dpy, DefaultColormap(dpy, DefaultScreen(dpy)), &color);
   _color_pixel[1] = color.pixel;

   color.red = 0;
   color.green = 0;
   color.blue = 65535;
   XAllocColor(dpy, DefaultColormap(dpy, DefaultScreen(dpy)), &color);
   _color_pixel[2] = color.pixel;

   color.red = 65535;
   color.green = 65535;
   color.blue = 0;
   XAllocColor(dpy, DefaultColormap(dpy, DefaultScreen(dpy)), &color);
   _color_pixel[3] = color.pixel;

   color.red = 65535;
   color.green = 0;
   color.blue = 65535;
   XAllocColor(dpy, DefaultColormap(dpy, DefaultScreen(dpy)), &color);
   _color_pixel[4] = color.pixel;

   color.red = 0;
   color.green = 65535;
   color.blue = 65535;
   XAllocColor(dpy, DefaultColormap(dpy, DefaultScreen(dpy)), &color);
   _color_pixel[5] = color.pixel;

   n = 0;
   if (left == NULL)
   {
      XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
   }
   else
   {
      XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET); n++;
      XtSetArg(args[n], XmNleftWidget, left); n++;
   }
   if (right == NULL)
   {
      XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
   }
   else
   {
      XtSetArg(args[n], XmNrightAttachment, XmATTACH_WIDGET); n++;
      XtSetArg(args[n], XmNrightWidget, right); n++;
   }
   if (top == NULL)
   {
      XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
   }
   else
   {
      XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
      XtSetArg(args[n], XmNtopWidget, top); n++;
   }
   XtSetArg(args[n], XmNwidth, _width + 1); n++;
   XtSetArg(args[n], XmNheight, _height); n++;
   XtSetArg(args[n], XmNbackground, _black_pixel); n++;
   _w = XmCreateDrawingArea(parent, "Draw", args, n);
   XtAddCallback(_w, XmNexposeCallback, connect_widget::expose, this);
   XtManageChild(_w);

}


