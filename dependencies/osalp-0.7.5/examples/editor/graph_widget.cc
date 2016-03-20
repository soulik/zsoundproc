// This file contains no license whatsoever. It is provided in the public domain as an example
// of how to use the audio library
//
// Bruce Forsberg
// forsberg@tns.net
// 
//

#include <iostream.h>
#include <stdio.h>
#include "graph_widget.h"
#include "plot.h"

#define MARKER_DELTA  5
#define TOP_MARGIN    10
#define BOTTOM_MARGIN 20
#define ZERO_TIME_STRING "00:00:00.00"


graph_widget::graph_widget(
   Widget  parent,
   Widget  left,
   Widget  right,
   Widget  top,
   Widget  bottom,
   int     height,
   int     width,
   int     channels)
{
   _height   = height;
   _width    = width;
   _channels = channels;
   _init_gc  = FALSE;
   _back_buf = 0;
   _marker_selected = MARKER_NONE;
   _marker_samples_per_second = 0;
   _marker_delta = 1;
   strcpy(_start_time, ZERO_TIME_STRING);
   strcpy(_stop_time, ZERO_TIME_STRING);
   strcpy(_begin_time, ZERO_TIME_STRING);
   strcpy(_end_time, ZERO_TIME_STRING);
   strcpy(_current_time, ZERO_TIME_STRING);
   _name_string[0] = 0;
   _start_sample = 0;
   _stop_sample = 0;
   _begin_sample = 0;
   _end_sample = 0;
   _current_sample = 0;
   _begin_marker = 0;
   _end_marker = width - 1;
   _current_marker = 0;
   _data_array = NULL;
   _divisor = 1;
   _shift_data = 0;
   _shift_start_x = 0;
   _shift_start_y = 0;
   _shift_stop_x = 0;
   _shift_stop_y = 0;
   _total_samples = 0;
   _slider_x = -1;

   allocateMemory();

   build_widget(parent, left, right, top, bottom);
}

graph_widget::~graph_widget()
{
   delete [] _data_array;
}


void
graph_widget::allocateMemory()
{
   delete [] _data_array;

   // Allocate memory for data storage
   _data_array = new XSegment[_width * _channels];
}


void
graph_widget::load_new_data(int *data)
{
   // This function will load new data into the graph. This data must be
   // width * channels in length. All of the first channel data should be
   // first followed by the second channel data.

   int  i, chan, index;
   int  h, off;


   h = _height / _channels;

   for (chan = 0; chan < _channels; chan++)
   {
      off = h * chan + TOP_MARGIN;
      for (i = 0; i < _width; i++)
      {
         index = i + _width * chan;
         _data_array[index].y1 = data[index] / _channels + off;
         _data_array[index].x1 = i;
         if (data[index] > _height/2)
            _data_array[index].y2 = h/2 - (data[index]/_channels - h/2) + off;
         else
            _data_array[index].y2 = h/2 + (h/2 - data[index]/_channels) + off;
         _data_array[index].x2 = i;
      }
   }
}

void
graph_widget::perform_expose()
{
   // This function will perform an expose of the widget

   expose(_w, (caddr_t)this, NULL);
}


void
graph_widget::setSamplesPerSecond(long samples)
{
   _marker_samples_per_second = samples;
}


long
graph_widget::getSamplesPerSecond() const
{
   return (_marker_samples_per_second);
}


void
graph_widget::setMarkerResolution(int res)
{
   _marker_delta = res;
}


int
graph_widget::getMarkerResolution() const
{
   return (_marker_delta);
}


void
graph_widget::setTotalSamples(long long samples)
{
   _total_samples = samples;
}


long long
graph_widget::getTotalSamples() const
{
   return (_total_samples);
}

void
graph_widget::setStartSamples(long long samples)
{
   _start_sample = samples;

   convertToTimeString(_start_sample, _start_time);

   // These are based on _start_sample so we update again.
   setBeginSamples(_begin_sample);
   setEndSamples(_begin_sample);
}


long long
graph_widget::getStartSamples() const
{
   return (_start_sample);
}


void
graph_widget::setStopSamples(long long samples)
{
   _stop_sample = samples;

   convertToTimeString(_stop_sample, _stop_time);
}


long long
graph_widget::getStopSamples() const
{
   return (_stop_sample);
}


void
graph_widget::setBeginSamples(long long samples)
{
   _begin_sample = samples;
   _begin_marker = _begin_sample / _marker_delta;

   convertToTimeString(_begin_sample + _start_sample, _begin_time);
}

      
long long
graph_widget::getBeginSamples() const
{
   return (_begin_sample);
}


void
graph_widget::setEndSamples(long long samples)
{
   _end_sample = samples;
   _end_marker = _end_sample / _marker_delta;

   convertToTimeString(_end_sample + _start_sample, _end_time);
}


long long
graph_widget::getEndSamples() const
{
   return (_end_sample);
}


void
graph_widget::setCurrentSamples(long long samples)
{
   _current_sample = samples;
   _current_marker = _current_sample / _marker_delta;

   convertToTimeString(_current_sample, _current_time);
}


long long
graph_widget::getCurrentSamples() const
{
   return (_end_sample);
}


void
graph_widget::setChannels(int chan)
{
   _channels = chan;

   allocateMemory();
}

int
graph_widget::getChannels() const
{
   return (_channels);
}

void
graph_widget::setDivisor(int div)
{
   if (div == 0)
   {
      div = 1;
      _shift_data *= 2;
   }
   _divisor = div;
}

int
graph_widget::getDivisor() const
{
   return (_divisor);
}

void
graph_widget::setShiftData(int shift_data)
{
   _shift_data = shift_data;
   if (_shift_data < 0)
      _shift_data = 0;
}

int
graph_widget::getShiftData() const
{
  return (_shift_data);
}


Widget
graph_widget::getWidget() const
{
   return (_w);
}


void
graph_widget::setName(const char * str)
{
   // This function limits the string length to 256 characters

   strncpy(_name_string, str, GRAPH_WIDGET_NAME_STRING);
}


void
graph_widget::button_press_callback(
   Widget w,
   caddr_t client_data,
   XEvent    *event,
   Boolean   *flag)
{
   int x = event->xbutton.x;
   int y = event->xbutton.y;
   int d = MARKER_DELTA;
   graph_widget * obj = (graph_widget *)client_data;


   // IF pointer is at begin marker
   if ((obj->_begin_marker > (x-d)) && (obj->_begin_marker < (x+d)))
   {
      obj->_marker_selected = MARKER_BEGIN;
      obj->_begin_marker = x;
      obj->_begin_sample = obj->_begin_marker * obj->_marker_delta;
   }
   // IF pointer is at end marker
   if ((obj->_end_marker > (x-d)) && (obj->_end_marker < (x+d)))
   {
      obj->_marker_selected = MARKER_END;
      obj->_end_marker = x;
      obj->_end_sample = obj->_end_marker * obj->_marker_delta;
   }

   // IF pointer is in slider
   if ((obj->_shift_start_x < x) && (obj->_shift_stop_x > x) &&
       (obj->_shift_start_y < y) && (obj->_shift_stop_y > y))
   {
      obj->_marker_selected = MARKER_SLIDER;
      // Get start x position as a reference
      obj->_slider_x = x;
   }

   expose(w, client_data, NULL);
}

void
graph_widget::button_motion_callback(
   Widget w,
   caddr_t client_data,
   XEvent    *event,
   Boolean   *flag)
{
   int x = event->xbutton.x;
   int hour, minutes;
   double seconds;
   graph_widget * obj = (graph_widget *)client_data;


   if (obj->_marker_selected == MARKER_BEGIN)
   {
      obj->_begin_marker = x;
      obj->_begin_sample = obj->_begin_marker * obj->_marker_delta;
      seconds = (double)(x * obj->_marker_delta + obj->_start_sample)/ obj->_marker_samples_per_second;
      hour = (int)(seconds / 3600);
      seconds -= (hour * 3600);
      minutes = (int)(seconds / 60);
      seconds -= (minutes * 60);
      sprintf(obj->_begin_time, "%2.2d:%2.2d:%05.2f", hour, minutes, seconds);
   }
   else if (obj->_marker_selected == MARKER_END)
   {
      obj->_end_marker = x;
      obj->_end_sample = obj->_end_marker * obj->_marker_delta;
      seconds = (double)(x * obj->_marker_delta + obj->_start_sample)/ obj->_marker_samples_per_second;
      hour = (int)(seconds / 3600);
      seconds -= (hour * 3600);
      minutes = (int)(seconds / 60);
      seconds -= (minutes * 60);
      sprintf(obj->_end_time, "%2.2d:%2.2d:%05.2f", hour, minutes, seconds);
   }
   else if (obj->_marker_selected == MARKER_SLIDER)
   {
      plot_shift_right( (x - obj->_slider_x) * obj->getDivisor() );
      obj->_slider_x = x;
   }

   // Draw changes except for SLIDER which has already drawn
   if (obj->_marker_selected != MARKER_SLIDER)
      expose(w, client_data, NULL);
}

void
graph_widget::button_release_callback(
   Widget  w,
   caddr_t client_data,
   XEvent    *event,
   Boolean   *flag)
{
   graph_widget * obj = (graph_widget *)client_data;
   obj->_marker_selected = MARKER_NONE;

   expose(w, client_data, NULL);
}


void
graph_widget::expose(
   Widget  w,
   caddr_t client_data,
   XmDrawingAreaCallbackStruct  *draw_struct)
{
   Display *dis = XtDisplay(w);
   graph_widget *obj = (graph_widget *)client_data;
   Window  win;
   XTextItem text_item[1];
   int  i;
   int  t_m;
   int  wid1, wid2;


   t_m = TOP_MARGIN + BOTTOM_MARGIN;

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
   XFillRectangle(dis, win, obj->_gc, 0, 0, obj->_width, obj->_height + t_m);
   XSetForeground(dis, obj->_gc, obj->_white_pixel);

   for (i = 0; i < obj->_channels; i++)
   {
      int h = obj->_height / obj->_channels;
      XDrawLine(dis, win, obj->_gc,
         0, h * i + h/2 + TOP_MARGIN, obj->_width, h * i + h/2 + TOP_MARGIN);

      XDrawSegments(dis, win, obj->_gc, &(obj->_data_array[i * obj->_width]), obj->_width);
   }


   text_item[0].font = None;
   text_item[0].delta = 0;
   text_item[0].nchars = strlen(obj->_name_string);
   text_item[0].chars = obj->_name_string;
   XDrawText(dis, win, obj->_gc, 0, TOP_MARGIN, text_item, 1);

   text_item[0].nchars = strlen(obj->_start_time);
   text_item[0].chars = obj->_start_time;
   XDrawText(dis, win, obj->_gc, 0, obj->_height + t_m, text_item, 1);

   text_item[0].nchars = strlen(obj->_stop_time);
   text_item[0].chars = obj->_stop_time;
   XDrawText(dis, win, obj->_gc, obj->_width - 70, obj->_height + t_m, text_item, 1);

   XSetForeground(dis, obj->_gc, obj->_green_pixel);
   text_item[0].nchars = strlen(obj->_begin_time);
   text_item[0].chars = obj->_begin_time;
   XDrawText(dis, win, obj->_gc, obj->_width / 2 - 80,  obj->_height + t_m, text_item, 1);
   /* Draw marker */
   XDrawLine(dis, win, obj->_gc, obj->_begin_marker, TOP_MARGIN, obj->_begin_marker, obj->_height + TOP_MARGIN);

   XSetForeground(dis, obj->_gc, obj->_red_pixel);
   text_item[0].nchars = strlen(obj->_end_time);
   text_item[0].chars = obj->_end_time;
   XDrawText(dis, win, obj->_gc, obj->_width / 2 + 10,  obj->_height + t_m, text_item, 1);
   /* Draw marker */
   XDrawLine(dis, win, obj->_gc, obj->_end_marker, TOP_MARGIN, obj->_end_marker, obj->_height + TOP_MARGIN);

   XSetForeground(dis, obj->_gc, obj->_blue_pixel);
   /* Draw marker */
   XDrawLine(dis, win, obj->_gc, obj->_current_marker, TOP_MARGIN, obj->_current_marker, obj->_height + TOP_MARGIN);

   // Find start and stop of blue bar
   if (obj->_total_samples == 0)
   {
      wid1 = 0;
      wid2 =  obj->_width;
   }
   else
   {
      wid1 = (int)(((double)obj->_start_sample / (double)obj->_total_samples) * obj->_width);
      wid2 = (int)(((double)obj->_stop_sample / (double)obj->_total_samples) * obj->_width);
   }

   XFillRectangle(dis, win, obj->_gc, wid1, obj->_height + TOP_MARGIN, wid2-wid1, BOTTOM_MARGIN / 2);

   obj->_shift_start_x = wid1;
   obj->_shift_stop_x  = wid2;
   obj->_shift_start_y = obj->_height + TOP_MARGIN;
   obj->_shift_stop_y  = obj->_shift_start_y + BOTTOM_MARGIN / 2;

   // IF back buffering is enabled then swap
   if (obj->_back_buf != 0)
      XdbeSwapBuffers(dis, &(obj->_back_info), 1);

   XSetForeground(dis, obj->_gc, obj->_white_pixel);
}


void
graph_widget::create_gc()
{
   /* Need to do this after toplevel is realized */
   XGCValues gcv;
   gcv.foreground = _white_pixel;
   gcv.background = _black_pixel;
   int  minor_ver, major_ver;


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

   // Was back buffer activated
   if (_back_buf == 0)
      _gc = XCreateGC (XtDisplay(_w), XtWindow(_w), GCForeground | GCBackground, &gcv);
   else
      _gc = XCreateGC (XtDisplay(_w), _back_buf, GCForeground | GCBackground, &gcv);

   // We are initialized
   _init_gc = TRUE;
}


void
graph_widget::build_widget(
   Widget parent,
   Widget left,
   Widget right,
   Widget top,
   Widget bottom)
{
   Arg  args[20];
   int  n;
   XColor  red, green, blue;
   Display *dpy;

   dpy = XtDisplay(parent);

   _black_pixel = BlackPixelOfScreen(XtScreen(parent));
   _white_pixel = WhitePixelOfScreen(XtScreen(parent));

   red.red = 65535;
   red.green = 0;
   red.blue = 0;
   XAllocColor(dpy, DefaultColormap(dpy, DefaultScreen(dpy)), &red);
   _red_pixel = red.pixel;

   green.red = 0;
   green.green = 65535;
   green.blue = 0;
   XAllocColor(dpy, DefaultColormap(dpy, DefaultScreen(dpy)), &green);
   _green_pixel = green.pixel;

   blue.red = 0;
   blue.green = 0;
   blue.blue = 65535;
   XAllocColor(dpy, DefaultColormap(dpy, DefaultScreen(dpy)), &blue);
   _blue_pixel = blue.pixel;

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
   XtSetArg(args[n], XmNheight, _height + TOP_MARGIN + BOTTOM_MARGIN); n++;
   XtSetArg(args[n], XmNbackground, _black_pixel); n++;
   _w = XmCreateDrawingArea(parent, "Draw", args, n);
   XtAddCallback(_w, XmNexposeCallback, (void (*)(_WidgetRec*, void*, void*))graph_widget::expose, this);
   XtAddEventHandler(_w, ButtonPressMask, FALSE, (void (*)(_WidgetRec*, void*, XEvent*, Boolean*))button_press_callback, this);
   XtAddEventHandler(_w, ButtonMotionMask, FALSE, (void (*)(_WidgetRec*, void*, XEvent*, Boolean*))button_motion_callback, this);
   XtAddEventHandler(_w, ButtonReleaseMask, FALSE, (void (*)(_WidgetRec*, void*, XEvent*, Boolean*))button_release_callback, this);
   XtManageChild(_w);

}


void
graph_widget::convertToTimeString(
      long long samples,
      char *    str)
{

   int hour, minutes;
   double seconds;


   seconds = (double)samples / _marker_samples_per_second;
   hour = (int)(seconds / 3600);
   seconds -= (hour * 3600);
   minutes = (int)(seconds / 60);
   seconds -= (minutes * 60);
   sprintf(str, "%2.2d:%2.2d:%05.2f", hour, minutes, seconds);
}



