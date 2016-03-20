// This file contains no license whatsoever. It is provided in the public domain as an example
// of how to use the audio library
//
// Bruce Forsberg
// forsberg@tns.net
// 
//


#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include <X11/X.h>

extern "C"
{
#include <Xm/Xm.h>
#include <Xm/Form.h>
#include <Xm/PushB.h>
#include <Xm/Text.h>
#include <Xm/DrawingA.h>
#include <Xm/Scale.h>
#include <Xm/RowColumn.h>
}

#include "pixmaps.h"

#include "aflibConfig.h"
#include "aflibAudioFile.h"
#include "aflibData.h"
#include "aflibSampleData.h"
#include "aflibEnvFile.h"
#include <string>

#include "LiteClue.h"

#include "ltdraw.h"
#include "lbdraw.h"
#include "rbdraw.h"
#include "rtdraw.h"
#include "audio.h"
#include "popup.h"


extern void
expose_callback(
   Widget  w,
   caddr_t client_data,
   XmDrawingAreaCallbackStruct  *draw_struct);

extern void
scale_callback(
   Widget  w,
   caddr_t client_data,
   XmScaleCallbackStruct *scale_struct);

extern void
pitch_callback(
   Widget  w,
   caddr_t client_data,
   XmScaleCallbackStruct *scale_struct);


extern void
gui_postit(
   Widget pb,
   void *popup,
   XEvent  *event,
   Boolean              *flag);


static XrmOptionDescRec opTable[] =
{
   {"-time", ".time", XrmoptionSepArg, NULL},
};

static String FallBack[] =
{
   "*borderWidth: 0",
   "*geometry: +100+100",
   "*background: grey",
   NULL
};

static ltdraw  * _ltdraw = NULL;
static lbdraw  * _lbdraw = NULL;
static rbdraw  * _rbdraw = NULL;
static rtdraw  * _rtdraw = NULL;
static Widget PositionScale, PitchButton;
static Pixmap pitch_pixmap, nopitch_pixmap;
static bool pitch_state = FALSE;
static XtAppContext  app;


void
stop_call(
   Widget w,
   XtPointer client_data,
   XtPointer call_data)
{
   audio_stop();
}

void
play_call(
   Widget w,
   XtPointer client_data,
   XtPointer call_data)
{
   audio_play(NULL);
}

void
pause_call(
   Widget w,
   XtPointer client_data,
   XtPointer call_data)
{
   audio_pause();
}

void
pitch_call(
   Widget w,
   XtPointer client_data,
   XtPointer call_data)
{
   Arg  args[20];
   int  n;

   n = 0;
   if (pitch_state == TRUE)
   {
      XtSetArg(args[n], XmNlabelPixmap, nopitch_pixmap); n++;
      pitch_state = FALSE;
   }
   else
   {
      XtSetArg(args[n], XmNlabelPixmap, pitch_pixmap); n++;
      pitch_state = TRUE;
   }
   XtSetValues(PitchButton, args, n);

   audio_pitch();
}

void
gui_spectrum(
   int array,
   double *db_array)
{
   _rbdraw->plot_spectrum(array, db_array);
   _rtdraw->plot_spectrum(array, &db_array[32]);
}

void
gui_build(int argc, char * argv[])
{
   Widget toplevel, Form;
   Widget StopButton, PlayButton, PauseButton;
   Widget PitchScale;
   Widget popup;
   Widget liteClue;
   Pixmap stop_pixmap, pause_pixmap, play_pixmap;
   Pixel  fg, bg;
   Arg  args[20];
   int  n;


   toplevel = XtVaAppInitialize(&app, "Form1",
               opTable, XtNumber(opTable),
               &argc, argv,
               FallBack, NULL);

    n = 0;
    liteClue = XtCreatePopupShell(
        "popup_shell",
        xcgLiteClueWidgetClass, toplevel, args, n);

   Form = XmCreateForm(toplevel, "Form", NULL, 0);
   XtVaSetValues(Form,
                 XmNresizePolicy, XmRESIZE_ANY,
                 XmNallowShellResize, True,
                 NULL);

   popup = popup_build(Form, XmMENU_POPUP);

   XtVaGetValues(Form,
      XmNbackground, &bg,
      XmNforeground, &fg,
      NULL);

   stop_pixmap = XCreatePixmapFromBitmapData(XtDisplay(toplevel),
      RootWindowOfScreen(XtScreen(toplevel)),
      (char *)stop_pixmap_bits, stop_pixmap_width, stop_pixmap_height, fg, bg, DefaultDepthOfScreen(XtScreen(toplevel)));

   play_pixmap = XCreatePixmapFromBitmapData(XtDisplay(toplevel),
      RootWindowOfScreen(XtScreen(toplevel)),
      (char *)play_pixmap_bits, play_pixmap_width, play_pixmap_height, fg, bg, DefaultDepthOfScreen(XtScreen(toplevel)));

   pause_pixmap = XCreatePixmapFromBitmapData(XtDisplay(toplevel),
      RootWindowOfScreen(XtScreen(toplevel)),
      (char *)pause_pixmap_bits, pause_pixmap_width, pause_pixmap_height, fg, bg, DefaultDepthOfScreen(XtScreen(toplevel)));

   pitch_pixmap = XCreatePixmapFromBitmapData(XtDisplay(toplevel),
      RootWindowOfScreen(XtScreen(toplevel)),
      (char *)pitch_bits, pitch_width, pitch_height, fg, bg, DefaultDepthOfScreen(XtScreen(toplevel)));

   nopitch_pixmap = XCreatePixmapFromBitmapData(XtDisplay(toplevel),
      RootWindowOfScreen(XtScreen(toplevel)),
      (char *)nopitch_bits, nopitch_width, nopitch_height, fg, bg, DefaultDepthOfScreen(XtScreen(toplevel)));

   n = 0;
   XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
   XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
   XtSetArg(args[n], XmNlabelType, XmPIXMAP); n++;
   XtSetArg(args[n], XmNlabelPixmap, stop_pixmap); n++;
   StopButton = XmCreatePushButton(Form, "Stop Button", args, n);
   XtAddCallback(StopButton, XmNactivateCallback, stop_call, NULL);
   XtManageChild(StopButton);
   XcgLiteClueAddWidget(liteClue, StopButton,  "Stop Audio", 0, 0);

   n = 0;
   XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET); n++;
   XtSetArg(args[n], XmNleftWidget, StopButton); n++;
   XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
   XtSetArg(args[n], XmNlabelType, XmPIXMAP); n++;
   XtSetArg(args[n], XmNlabelPixmap, play_pixmap); n++;
   PlayButton = XmCreatePushButton(Form, "Play Button", args, n);
   XtAddCallback(PlayButton, XmNactivateCallback, play_call, NULL);
   XtManageChild(PlayButton);
   XcgLiteClueAddWidget(liteClue, PlayButton,  "Play Audio", 0, 0);

   n = 0;
   XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET); n++;
   XtSetArg(args[n], XmNleftWidget, PlayButton); n++;
   XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
   XtSetArg(args[n], XmNlabelType, XmPIXMAP); n++;
   XtSetArg(args[n], XmNlabelPixmap, pause_pixmap); n++;
   PauseButton = XmCreatePushButton(Form, "Pause Button", args, n);
   XtAddCallback(PauseButton, XmNactivateCallback, pause_call, NULL);
   XtManageChild(PauseButton);
   XcgLiteClueAddWidget(liteClue, PauseButton,  "Pause Audio", 0, 0);

   n = 0;
   XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET); n++;
   XtSetArg(args[n], XmNleftWidget, PauseButton); n++;
   XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
   XtSetArg(args[n], XmNlabelType, XmPIXMAP); n++;
   XtSetArg(args[n], XmNlabelPixmap, nopitch_pixmap); n++;
   PitchButton = XmCreatePushButton(Form, "Pitch Button", args, n);
   XtAddCallback(PitchButton, XmNactivateCallback, pitch_call, NULL);
   XtManageChild(PitchButton);
   XcgLiteClueAddWidget(liteClue, PitchButton, "Pitch Control On/Off", 0, 0);

   n = 0;
   XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET); n++;
   XtSetArg(args[n], XmNleftWidget, PitchButton); n++;
   XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
   XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
   XtSetArg(args[n], XmNmaximum, 200); n++;
   XtSetArg(args[n], XmNminimum, 50); n++;
   XtSetArg(args[n], XmNvalue, 100); n++;
   XtSetArg(args[n], XmNshowValue, FALSE); n++;
   XtSetArg(args[n], XmNprocessingDirection, XmMAX_ON_RIGHT); n++;
   XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
   PitchScale = XmCreateScale(Form, "Pitch Scale", args, n);
   XtAddCallback(PitchScale, XmNdragCallback, (void (*)(_WidgetRec*, void*, void*))pitch_callback, NULL);
   XtManageChild(PitchScale);                                                         
   XcgLiteClueAddWidget(liteClue, PitchScale, "Pitch Control", 0, 0);


   n = 0; 
   XtSetArg(args[n], XmNbottomAttachment, XmATTACH_WIDGET); n++;
   XtSetArg(args[n], XmNbottomWidget, StopButton); n++;
   XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
   XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
   XtSetArg(args[n], XmNmaximum, 100); n++;
   XtSetArg(args[n], XmNminimum, 0); n++;
   XtSetArg(args[n], XmNvalue, 0); n++;
   XtSetArg(args[n], XmNshowValue, FALSE); n++;
   XtSetArg(args[n], XmNprocessingDirection, XmMAX_ON_RIGHT); n++;
   XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
   XtSetArg(args[n], XmNscaleWidth, 300); n++;
   PositionScale = XmCreateScale(Form, "Position Scale", args, n);
   XtAddCallback(PositionScale, XmNdragCallback, (void (*)(_WidgetRec*, void*, void*))scale_callback, NULL);
   XtManageChild(PositionScale);
   XcgLiteClueAddWidget(liteClue, PositionScale, "Audio Position Control", 0, 0);

   _lbdraw = new lbdraw(Form, PositionScale);
   XtAddEventHandler(_lbdraw->getWidget(), ButtonPressMask, False, gui_postit, popup);

   _rbdraw = new rbdraw(Form, _lbdraw->getWidget(), PositionScale);
   XtAddEventHandler(_rbdraw->getWidget(), ButtonPressMask, False, gui_postit, popup);

   _ltdraw = new ltdraw(Form, _lbdraw->getWidget());
   XtAddEventHandler(_ltdraw->getWidget(), ButtonPressMask, False, gui_postit, popup);

   _rtdraw = new rtdraw(Form, _ltdraw->getWidget(), _rbdraw->getWidget());
   XtAddEventHandler(_rtdraw->getWidget(), ButtonPressMask, False, gui_postit, popup);

   XtManageChild(Form);

   XtRealizeWidget(toplevel);

   _lbdraw->after_realize();
   _rbdraw->after_realize();
   _ltdraw->after_realize();
   _rtdraw->after_realize();

   _lbdraw->setTime(0);
   _lbdraw->setRecordMode(FALSE);

   XtAppAddWorkProc(app, work_app, NULL);

   // Get device port or set if not found
   aflibEnvFile env(ENV_FILE);
   string key(PORT_KEY);
   string value;
   if (env.readValueFromFile(key, value))
   {
      audio_device(value.c_str());
   }
   else
   {
      popup_device_dialog(toplevel);
   }

}

void
gui_loop()
{
   XtAppMainLoop(app);
}

void
expose_callback(
   Widget  w,
   caddr_t client_data,
   XmDrawingAreaCallbackStruct  *draw_struct)
{
}

void
gui_postit(
   Widget pb,
   void *popup,
   XEvent  *event,
   Boolean              *flag)
{
   if (((XButtonPressedEvent *)event)->button != Button1)
      return;

   XmMenuPosition((Widget)popup, (XButtonPressedEvent *)event);
   XtManageChild((Widget)popup);
}

void
scale_callback(
   Widget  w,
   caddr_t client_data,
   XmScaleCallbackStruct *scale_struct)
{
   if (scale_struct)
   {
      audio_set_position(scale_struct->value);
   }
}

void
pitch_callback(
   Widget  w,
   caddr_t client_data,
   XmScaleCallbackStruct *scale_struct)
{
   audio_set_pitch( (double)scale_struct->value / 100.0);
}


void
gui_set_slider_position(
   long long cur_pos,
   long long total_pos,
   long long sam_per_sec)
{
   Arg  args[20];
   int  n;
   int value;
   static int previous_value = 0;

   if (total_pos == 0)
   {
      value = 0;
   }
   else
   {
      value = (int)((100 * cur_pos) / total_pos);
   }

   if (value != previous_value)
   {
      n = 0;
      XtSetArg(args[n], XmNvalue, value); n++;
      XtSetValues(PositionScale, args, n);
      previous_value = value;
   }

   if (sam_per_sec != 0)
   {
      _lbdraw->setTime((int)(cur_pos / sam_per_sec));
   }
}

void
gui_set_record_mode(int record)
{
   _lbdraw->setRecordMode(record);
}

void
gui_set_channels(int chan)
{
   _ltdraw->setChannels(chan);
}

void
gui_set_bits(int bits)
{
   _ltdraw->setBits(bits);
}

void
gui_set_file(string& file)
{
   _ltdraw->setFile(file);
}

void
gui_set_format(const string& format)
{
   _ltdraw->setFormat(format);
}



