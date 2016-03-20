#ifndef _GRAPH_WIDGET_H
#define _GRAPH_WIDGET_H


extern "C"
{
#include <X11/X.h>
#include <Xm/Xm.h>
#include <X11/extensions/Xext.h>
#include <X11/extensions/Xdbe.h>
#include <Xm/DrawingA.h>
}

#define GRAPH_STRING_SIZE 100
#define GRAPH_WIDGET_NAME_STRING 256

class graph_widget {

public:

   graph_widget(
      Widget parent,
      Widget left,
      Widget right,
      Widget top,
      Widget bottom,
      int    height,
      int    width,
      int    channels);

   ~graph_widget();

   void
   allocateMemory();

   void
   load_new_data(int *data);

   void
   perform_expose();

   void
   setSamplesPerSecond(long samples);

   long
   getSamplesPerSecond() const;

   void
   setMarkerResolution(int res);

   int
   getMarkerResolution() const;

   void
   setTotalSamples(long long samples);

   long long
   getTotalSamples() const;

   void
   setStartSamples(long long samples);

   long long
   getStartSamples() const;

   void
   setStopSamples(long long samples);

   long long
   getStopSamples() const;

   void
   setBeginSamples(long long samples);

   long long
   getBeginSamples() const;

   void
   setEndSamples(long long samples);

   long long
   getEndSamples() const;

   void
   setCurrentSamples(long long samples);

   long long
   getCurrentSamples() const;

   void
   setChannels(int chan);

   int
   getChannels() const;

   void
   setDivisor(int div);

   int
   getDivisor() const;

   void
   setShiftData(int shift_data);

   int
   getShiftData() const;

   Widget
   getWidget() const;

   void
   setName(const char * str);

   static void
   button_press_callback(
      Widget w,
      caddr_t client_data,
      XEvent    *event,
      Boolean   *flag);

   static void
   button_motion_callback(
      Widget w,
      caddr_t client_data,
      XEvent    *event,
      Boolean   *flag);

   static void
   button_release_callback(
      Widget  w,
      caddr_t client_data,
      XEvent    *event,
      Boolean   *flag);

   static void
   graph_widget::expose(
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

   void
   convertToTimeString(
      long long samples,
      char *    str);

enum marker_type
{
   MARKER_NONE,
   MARKER_BEGIN,
   MARKER_END,
   MARKER_SLIDER
};

// Drawing structures
Widget  _w;
GC      _gc;
bool    _init_gc;
XSegment  * _data_array;

// Configuration information
int     _height;
int     _width;
int     _channels;
char    _name_string[256];

// Color support for drawing
Pixel   _black_pixel;
Pixel   _white_pixel;
Pixel   _red_pixel;
Pixel   _green_pixel;
Pixel   _blue_pixel;

// back buffering support
Drawable      _back_buf;
XdbeSwapInfo  _back_info;

// Start Time support
char      _start_time[GRAPH_STRING_SIZE];
long long _start_sample;

// Stop Time support
char      _stop_time[GRAPH_STRING_SIZE];
long long _stop_sample;

// Begin Time support
char      _begin_time[GRAPH_STRING_SIZE];
int       _begin_marker;
long long _begin_sample;

// End Time support
char      _end_time[GRAPH_STRING_SIZE];
int       _end_marker;
long long _end_sample;

// Current Time support
char      _current_time[GRAPH_STRING_SIZE];
int       _current_marker;
long long _current_sample;

// Marker support
graph_widget::marker_type  _marker_selected;
long      _marker_samples_per_second;
int       _marker_delta;
long long _total_samples;

// Display reduction data
int  _divisor;
int  _shift_data;
int  _shift_start_x;
int  _shift_start_y;
int  _shift_stop_x;
int  _shift_stop_y;
int  _slider_x;


};


#endif
