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
#include <fcntl.h>
#include <iostream.h>
#include <string>

#include <X11/X.h>

extern "C"
{
#include <Xm/Xm.h>
#include <Xm/Form.h>
#include <Xm/PushB.h>
#include <Xm/ToggleB.h>
#include <Xm/RowColumn.h>
#include <Xm/Text.h>
#include <Xm/Label.h>
#include <Xm/DrawingA.h>
#include <Xm/CascadeB.h>
#include <Xm/FileSB.h>
#include <Xm/ScrolledW.h>
}

#include "plot.h"
#include "gui.h"

#define ADD_INPUT "Add Input"

static void
create_menu(Widget form);

static void
create_buttons(Widget form);

static Widget ScrollForm;
static Widget last_widget = NULL;


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



void
mode_call(
   Widget w,
   XtPointer client_data,
   XtPointer call_data)
{
   static bool mode = 1;
   XmString str;


   if (mode == 1)
   {
      str = XmStringCreateSimple("Input");
      mode = 0;
   }
   else
   {
      str = XmStringCreateSimple("Output");
      mode = 1;
   }

   XtVaSetValues(w,
                 XmNlabelString, str,
                 NULL);
   plot_set_mode(mode);

   XmStringFree(str);
}


void
add_callback(
   Widget w,
   XtPointer client_data,
   XtPointer call_data)
{
   plot_select_input((int)client_data);
}


void
insert_call(
   Widget w,
   XtPointer client_data,
   XtPointer call_data)
{
   plot_insert();
}


void
enlarge_call(
   Widget w,
   XtPointer client_data,
   XtPointer call_data)
{
   plot_enlarge();
}

void
reduce_call(
   Widget w,
   XtPointer client_data,
   XtPointer call_data)
{
   plot_reduce();
}

void
play_call(
   Widget w,
   XtPointer client_data,
   XtPointer call_data)
{
   plot_play(NULL);
}

void
cut_call(
   Widget w,
   XtPointer client_data,
   XtPointer call_data)
{
   plot_cut();
}

void
reset_call(
   Widget w,
   XtPointer client_data,
   XtPointer call_data)
{
   plot_reset();
}

void
erase_call(
   Widget w,
   XtPointer client_data,
   XtPointer call_data)
{
   plot_erase();
}

void
pause_call(
   Widget w,
   XtPointer client_data,
   XtPointer call_data)
{
   plot_pause();
}

void
stop_call(
   Widget w,
   XtPointer client_data,
   XtPointer call_data)
{
   plot_stop();
}

void
input_call(
   Widget w,
   XtPointer client_data,
   XtPointer call_data)
{
}

void
gui_load_file(
   Widget w,
   caddr_t client_data,
   XmFileSelectionBoxCallbackStruct  *cbs)
{
   char * file = NULL;

   if (cbs)
   {
      if (!XmStringGetLtoR(cbs->value, XmSTRING_DEFAULT_CHARSET, &file))
         return;

      if (strcmp(ADD_INPUT, client_data) == 0)
         plot_input(file);
      else if (strcmp("SaveClipAs", client_data) == 0)
         plot_play(file);
   
      XtFree(file);
   }
   XtUnmanageChild(w);
}

void
gui_cancel_file(
   Widget w,
   caddr_t client_data,
   XmFileSelectionBoxCallbackStruct  *cbs)
{
   XtUnmanageChild(w);
}


void
open_callback(
   Widget  w,
   char*   name,
   caddr_t call_data)
{
   Arg  args[10];
   int  n;
   Widget dialog;
   static Widget open_dialog = 0, add_dialog = 0, save_dialog = 0;
   XmString str;

   if (strcmp(ADD_INPUT, name) == 0)
      dialog = add_dialog;
   else if (strcmp("SaveClipAs", name) == 0)
      dialog = save_dialog;
   else
      dialog = open_dialog;

   if (!dialog)
   {
      if (strcmp(ADD_INPUT, name) == 0)
         str = XmStringCreateSimple(ADD_INPUT);
      else if (strcmp("SaveClipAs", name) == 0)
         str = XmStringCreateSimple("Save Clip");
      else
         str = XmStringCreateSimple("Open File");

      n = 0;
      XtSetArg(args[n], XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL); n++;
      XtSetArg(args[n], XmNdialogTitle, str); n++;
      dialog = XmCreateFileSelectionDialog(w, "FileBox", args, n);
      XtAddCallback(dialog, XmNokCallback, gui_load_file, name);
      XtAddCallback(dialog, XmNcancelCallback, gui_cancel_file, NULL);

      XmStringFree(str);

      if (strcmp(ADD_INPUT, name) == 0)
         add_dialog = dialog;
      else if (strcmp("SaveClipAs", name) == 0)
         save_dialog = dialog;
      else
         open_dialog = dialog;
   }
   XtManageChild(dialog);
   XtPopup(XtParent(dialog), XtGrabNone);
}


void
quit_callback(
   Widget  w,
   char*   name,
   caddr_t call_data)
{
   exit(0);
}

void
gui_build(int argc, char * argv[])
{
   Widget toplevel, Form;
   Widget ScrollWidget, LabelWidget, ModeWidget;
   Widget Form1, Form2, Form3, Form4;
   XtAppContext  app;


   toplevel = XtVaAppInitialize(&app, "Form1",
               opTable, XtNumber(opTable),
               &argc, argv,
               FallBack, NULL);

   Form = XmCreateForm(toplevel, "Form", NULL, 0);
   XtVaSetValues(Form,
                 XmNresizePolicy, XmRESIZE_ANY,
                 XmNallowShellResize, True,
                 NULL);

   Form1 = XmCreateForm(Form, "Form1", NULL, 0);
   XtVaSetValues(Form1,
                 XmNtopAttachment, XmATTACH_FORM,
                 XmNrightAttachment, XmATTACH_FORM,
                 XmNleftAttachment, XmATTACH_FORM,
                 NULL);

   Form4 = XmCreateForm(Form, "Form4", NULL, 0);
   XtVaSetValues(Form4,
                 XmNtopAttachment, XmATTACH_WIDGET,
                 XmNtopWidget, Form1,
                 XmNrightAttachment, XmATTACH_FORM,
                 XmNleftAttachment, XmATTACH_FORM,
                 NULL);

   Form2 = XmCreateForm(Form, "Form2", NULL, 0);
   XtVaSetValues(Form2,
                 XmNtopAttachment, XmATTACH_WIDGET,
                 XmNtopWidget, Form4,
                 XmNleftAttachment, XmATTACH_FORM,
                 NULL);

   Form3 = XmCreateForm(Form, "Form3", NULL, 0);
   XtVaSetValues(Form3,
                 XmNtopAttachment, XmATTACH_WIDGET,
                 XmNtopWidget, Form4,
                 XmNleftAttachment, XmATTACH_WIDGET,
                 XmNleftWidget, Form2,
                 XmNrightAttachment, XmATTACH_FORM,
                 XmNbottomAttachment, XmATTACH_FORM,
                 NULL);

   create_menu(Form1);

   create_buttons(Form4);

   ModeWidget = XmCreatePushButton(Form2, "Output", NULL, 0);
   XtVaSetValues(ModeWidget,
                 XmNtopAttachment, XmATTACH_WIDGET,
                 XmNtopWidget, Form4,
                 XmNleftAttachment, XmATTACH_FORM,
                 XmNrightAttachment, XmATTACH_POSITION,
                 XmNrightPosition, 100,
                 NULL);
   XtAddCallback(ModeWidget, XmNactivateCallback, mode_call, NULL);
   XtManageChild(ModeWidget);

   ScrollWidget = XmCreateScrolledWindow(Form2, "Scrolled", NULL, 0);
   XtVaSetValues(ScrollWidget,
                 XmNtopAttachment, XmATTACH_WIDGET,
                 XmNtopWidget, ModeWidget,
                 XmNleftAttachment, XmATTACH_FORM,
                 XmNrightAttachment, XmATTACH_POSITION,
                 XmNrightPosition, 100,
                 NULL);
   XtManageChild(ScrollWidget);

   ScrollForm = XmCreateForm(ScrollWidget, "Scroll Form", NULL, 0);
   XtVaSetValues(ScrollForm,
                 XmNtopAttachment, XmATTACH_FORM,
                 XmNbottomAttachment, XmATTACH_FORM,
                 XmNleftAttachment, XmATTACH_FORM,
                 XmNrightAttachment, XmATTACH_FORM,
                 NULL);
   XtManageChild(ScrollForm);

   LabelWidget = XmCreateLabel(ScrollForm, "Input Selector", NULL, 0);
   XtVaSetValues(LabelWidget,
                 XmNtopAttachment, XmATTACH_FORM,
                 XmNleftAttachment, XmATTACH_FORM,
                 XmNrightAttachment, XmATTACH_FORM,
                 NULL);
   XtManageChild(LabelWidget);

   last_widget = LabelWidget;

   plot_build(Form3);

   XtManageChild(Form1);
   XtManageChild(Form4);
   XtManageChild(Form2);
   XtManageChild(Form3);
   XtManageChild(Form);

   XtRealizeWidget(toplevel);

   XtAppMainLoop(app);
}

void
add_input_button(int num)
{
   Widget temp;
   char   str[100];


   sprintf(str, "Input %d", num);

   temp = XmCreatePushButton(ScrollForm, str, NULL, 0);
   XtVaSetValues(temp,
                 XmNtopAttachment, XmATTACH_WIDGET,
                 XmNtopWidget, last_widget,
                 XmNleftAttachment, XmATTACH_FORM,
                 XmNrightAttachment, XmATTACH_FORM,
                 NULL);
   XtAddCallback(temp, XmNactivateCallback, add_callback, (char *)num);
   XtManageChild(temp);

   last_widget = temp;

}


static void
create_menu(Widget form)
{
   Arg  args[20];
   int  n;
   Widget MenuBar, FileMenu, FileBar;
   Widget AddButton, SaveClipButton, QuitButton;
   Widget ViewMenu, ViewBar, EraseButton;
   string  str;


   n = 0;
   XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
   XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
   MenuBar = XmCreateMenuBar(form, "Menu Bar", args, n);
   XtManageChild(MenuBar);

   /************** FILE MENU *******************/
   n = 0;
   XtSetArg(args[n], XmNmnemonic, 'F'); n++;
   FileMenu = XmCreateCascadeButton(MenuBar, "File", args, n);
   XtManageChild(FileMenu);

   n = 0;
   FileBar = XmCreatePulldownMenu(MenuBar, "File Pulldown", args, n);

   n = 0;
   XtSetArg(args[n], XmNsubMenuId, FileBar); n++;
   XtSetValues(FileMenu, args, n);

   str = ADD_INPUT;
   str.append("...");

   n = 0;
   AddButton = XtCreateManagedWidget(
      str.c_str(), xmPushButtonWidgetClass, FileBar, args, n);
   XtAddCallback(AddButton, XmNactivateCallback, open_callback, (char *)ADD_INPUT);
   
   n = 0;
   SaveClipButton = XtCreateManagedWidget(
      "Save Clip as...", xmPushButtonWidgetClass, FileBar, args, n);
   XtAddCallback(SaveClipButton, XmNactivateCallback, open_callback, (char *)"SaveClipAs");
   
   n = 0;
   QuitButton = XtCreateManagedWidget(
      "Quit", xmPushButtonWidgetClass, FileBar, args, n);
   XtAddCallback(QuitButton, XmNactivateCallback, quit_callback, (char *)"Quit");



   /**************** VIEW MENU ******************/
   n = 0;
   XtSetArg(args[n], XmNmnemonic, 'V'); n++;
   ViewMenu = XmCreateCascadeButton(MenuBar, "View", args, n);
   XtManageChild(ViewMenu);

   n = 0;
   ViewBar = XmCreatePulldownMenu(MenuBar, "View Pulldown", args, n);

   n = 0;
   XtSetArg(args[n], XmNsubMenuId, ViewBar); n++;
   XtSetValues(ViewMenu, args, n);

   n = 0;
   EraseButton = XtCreateManagedWidget(
      "Erase Edits", xmPushButtonWidgetClass, ViewBar, args, n);
   XtAddCallback(EraseButton, XmNactivateCallback, erase_call, NULL);
}


static void
create_buttons(Widget form)
{
   Widget Enlarge, Reduce;
   Widget PlayWidget, CutWidget, ResetWidget, PauseWidget, StopWidget;
   Widget InsertWidget;


   Enlarge = XmCreatePushButton(form, "Zoom In", NULL, 0);
   XtVaSetValues(Enlarge,
                 XmNtopAttachment, XmATTACH_FORM,
                 XmNleftAttachment, XmATTACH_FORM,
                 XmNrightAttachment, XmATTACH_POSITION,
                 XmNrightPosition, 25,
                 NULL);
   XtAddCallback(Enlarge, XmNactivateCallback, enlarge_call, NULL);
   XtManageChild(Enlarge);

   Reduce = XmCreatePushButton(form, "Zoom Out", NULL, 0);
   XtVaSetValues(Reduce,
                 XmNtopAttachment, XmATTACH_FORM,
                 XmNleftAttachment, XmATTACH_WIDGET,
                 XmNleftWidget, Enlarge,
                 XmNrightAttachment, XmATTACH_POSITION,
                 XmNrightPosition, 50,
                 NULL);
   XtAddCallback(Reduce, XmNactivateCallback, reduce_call, NULL);
   XtManageChild(Reduce);

   CutWidget = XmCreatePushButton(form, "Cut", NULL, 0);
   XtVaSetValues(CutWidget,
                 XmNtopAttachment, XmATTACH_WIDGET,
                 XmNtopWidget, Enlarge,
                 XmNleftAttachment, XmATTACH_FORM,
                 XmNrightAttachment, XmATTACH_POSITION,
                 XmNrightPosition, 25,
                 NULL);
   XtAddCallback(CutWidget, XmNactivateCallback, cut_call, NULL);
   XtManageChild(CutWidget);

   InsertWidget = XmCreatePushButton(form, "Insert", NULL, 0);
   XtVaSetValues(InsertWidget,
                 XmNtopAttachment, XmATTACH_WIDGET,
                 XmNtopWidget, Enlarge,
                 XmNleftAttachment, XmATTACH_WIDGET,
                 XmNleftWidget, CutWidget,
                 XmNrightAttachment, XmATTACH_POSITION,
                 XmNrightPosition, 50,
                 NULL);
   XtAddCallback(InsertWidget, XmNactivateCallback, insert_call, NULL);
   XtManageChild(InsertWidget);

   ResetWidget = XmCreatePushButton(form, "Reset Markers", NULL, 0);
   XtVaSetValues(ResetWidget,
                 XmNtopAttachment, XmATTACH_WIDGET,
                 XmNtopWidget, Enlarge,
                 XmNleftAttachment, XmATTACH_WIDGET,
                 XmNleftWidget, InsertWidget,
                 XmNrightAttachment, XmATTACH_POSITION,
                 XmNrightPosition, 75,
                 NULL);
   XtAddCallback(ResetWidget, XmNactivateCallback, reset_call, NULL);
   XtManageChild(ResetWidget);

   PlayWidget = XmCreatePushButton(form, "Play", NULL, 0);
   XtVaSetValues(PlayWidget,
                 XmNtopAttachment, XmATTACH_WIDGET,
                 XmNtopWidget, CutWidget,
                 XmNleftAttachment, XmATTACH_FORM,
                 XmNrightAttachment, XmATTACH_POSITION,
                 XmNrightPosition, 25,
                 NULL);
   XtAddCallback(PlayWidget, XmNactivateCallback, play_call, NULL);
   XtManageChild(PlayWidget);

   PauseWidget = XmCreatePushButton(form, "Pause", NULL, 0);
   XtVaSetValues(PauseWidget,
                 XmNtopAttachment, XmATTACH_WIDGET,
                 XmNtopWidget, CutWidget,
                 XmNleftAttachment, XmATTACH_WIDGET,
                 XmNleftWidget, PlayWidget,
                 XmNrightAttachment, XmATTACH_POSITION,
                 XmNrightPosition, 50,
                 NULL);
   XtAddCallback(PauseWidget, XmNactivateCallback, pause_call, NULL);
   XtManageChild(PauseWidget);

   StopWidget = XmCreatePushButton(form, "Stop", NULL, 0);
   XtVaSetValues(StopWidget,
                 XmNtopAttachment, XmATTACH_WIDGET,
                 XmNtopWidget, CutWidget,
                 XmNleftAttachment, XmATTACH_WIDGET,
                 XmNleftWidget, PauseWidget,
                 XmNrightAttachment, XmATTACH_POSITION,
                 XmNrightPosition, 75,
                 NULL);
   XtAddCallback(StopWidget, XmNactivateCallback, stop_call, NULL);
   XtManageChild(StopWidget);

}

