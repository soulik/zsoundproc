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
#include <iostream.h>

#include <X11/X.h>

extern "C"
{
#include <Xm/Xm.h>
#include <Xm/Form.h>
#include <Xm/DrawingA.h>
#include <Xm/LabelG.h>
#include <Xm/PushBG.h>
#include <Xm/RowColumn.h>
#include <Xm/SeparatoG.h>
#include <Xm/FileSB.h>
#include <Xm/SelectioB.h>
#include <Xm/DialogS.h>
#include <Xm/PanedW.h>
#include <Xm/List.h>
#include <Xm/Text.h>
#include <Xm/Label.h>
#include <Xm/ToggleB.h>
}

#include "audio.h"
#include "popup.h"
#include "gui.h"
#include "string.h"
#include "aflibFile.h"
#include "aflibConfig.h"
#include "aflibDateTime.h"
#include "aflibEnvFile.h"
#include "aflibAudioRecorder.h"

extern void
popup_configure_dialog(
   Widget  w,
   caddr_t client_data,
   XmAnyCallbackStruct *cbs);

extern void
popup_play_dialog(
   Widget  w,
   caddr_t client_data,
   XmAnyCallbackStruct *cbs);

extern void
popup_record_callback(
   Widget  w,
   caddr_t client_data,
   XmAnyCallbackStruct *cbs);

extern void
popup_format_cb(
   Widget  w,
   caddr_t client_data,
   XmAnyCallbackStruct *cbs);

extern void
popup_program_dialog(
   Widget  w,
   caddr_t client_data,
   XmAnyCallbackStruct *cbs);

extern void
popup_exit_dialog(
   Widget  w,
   caddr_t client_data,
   XmAnyCallbackStruct *cbs);

extern void
popup_play_file(
   Widget w,
   caddr_t client_data,
   XmFileSelectionBoxCallbackStruct  *cbs);

extern void
popup_record_file(
   Widget w,
   caddr_t client_data,
   XmFileSelectionBoxCallbackStruct  *cbs);

extern void
popup_cancel_file(
   Widget w,
   caddr_t client_data,
   XmFileSelectionBoxCallbackStruct  *cbs);

extern void
promptCallback(
   Widget w,
   caddr_t client_data,
   XmSelectionBoxCallbackStruct  *cbs);

extern void
popup_record_dialog(
   Widget w);

extern void
popup_record_close(
   Widget  w,
   caddr_t client_data,
   XmAnyCallbackStruct *cbs);

extern void
popup_record_edit(
   Widget  w,
   caddr_t client_data,
   XmAnyCallbackStruct *cbs);

extern void
popup_record_add(
   Widget  w,
   caddr_t client_data,
   XmAnyCallbackStruct *cbs);

extern void
popup_record_delete(
   Widget  w,
   caddr_t client_data,
   XmAnyCallbackStruct *cbs);

extern void
popup_config_dialog(
   Widget w);

extern void
popup_record_update_list();


typedef struct _menu_item
{
   char         * label;
   WidgetClass  * _class;
   char           mnemonic;
   char         * accelerator;
   char         * accel_text;
   void          (*callback)(Widget, void *, void *);
   caddr_t        callback_data;
   struct _menu_item  *subitem;
} MenuItem;

MenuItem items[] = {
   { "Audio Player", &xmLabelGadgetClass, (char) 0, NULL, 
     NULL, NULL, NULL, NULL},
   { "sep", &xmSeparatorGadgetClass, (char) 0, NULL,
     NULL, NULL, NULL, NULL},
   { "Configure...", &xmPushButtonGadgetClass, 'C', "Ctrl<Key>C",
     "Ctrl+C", popup_configure_dialog, NULL, NULL},
   { "Play...", &xmPushButtonGadgetClass, 'P', "Ctrl<Key>P",
     "Ctrl+P", popup_play_dialog, NULL, NULL},
   { "Record...", &xmPushButtonGadgetClass, 'R', "Ctrl<Key>R",
     "Ctrl+R", popup_record_callback, NULL, NULL},
   { "Program Record...", &xmPushButtonGadgetClass, 'V', "Ctrl<Key>V",
     "Ctrl+V", popup_program_dialog, NULL, NULL},
   { "sep1", &xmSeparatorGadgetClass, (char) 0, NULL,
     NULL, NULL, NULL, NULL},
   { "Exit", &xmPushButtonGadgetClass, 'E', "Ctrl<Key>E",
     "Ctrl+E", popup_exit_dialog, NULL, NULL},
   { NULL, NULL, (char) 0, NULL, NULL, NULL, NULL, NULL}
};

#define DEFAULT_DEVICE "/dev/audio"


static Widget  _form = NULL;
static Widget  list1;
static Widget  _text1, _text2, _text3, _text4, _text5, _text6;
static string  _record_file;
static int     _channels = 1;
static int     _samples_per_second = 22050;
static int     _bytes_per_sample = 1;
static string  _format_type;
static int     _timer_record_mode = FALSE;
static aflibConfig  _config;


aflibAudioRecorder *  _recorder = NULL;

void
popup_configure_dialog(
   Widget  w,
   caddr_t client_data,
   XmAnyCallbackStruct *cbs)
{
   popup_device_dialog(_form);
}

void
popup_play_dialog(
   Widget  w,
   caddr_t client_data,
   XmAnyCallbackStruct *cbs)
{
   Arg  args[10];
   int  n;
   static Widget dialog = 0;
   XmString  str;

   if (!dialog)
   {
      str = XmStringCreateSimple("Select File to Play");

      n = 0;
      XtSetArg(args[n], XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL); n++;
      XtSetArg(args[n], XmNdialogTitle, str); n++;
      dialog = XmCreateFileSelectionDialog(_form, "FileBox", args, n);
      XtUnmanageChild(XmSelectionBoxGetChild(dialog, XmDIALOG_HELP_BUTTON));
      XtAddCallback(dialog, XmNokCallback, popup_play_file, NULL);
      XtAddCallback(dialog, XmNcancelCallback, popup_cancel_file, NULL);

      XmStringFree(str);
   }

   XtManageChild(dialog);
   XtPopup(XtParent(dialog), XtGrabNone);

}

void
popup_record_callback(
   Widget  w,
   caddr_t client_data,
   XmAnyCallbackStruct *cbs)
{
   Arg  args[10];
   int  n;
   static Widget dialog = 0;
   XmString  str;

   if (!dialog)
   {
      str = XmStringCreateSimple("Select File to Record");

      n = 0;
      XtSetArg(args[n], XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL); n++;
      XtSetArg(args[n], XmNdialogTitle, str); n++;
      dialog = XmCreateFileSelectionDialog(_form, "FileBox", args, n);
      XtUnmanageChild(XmSelectionBoxGetChild(dialog, XmDIALOG_HELP_BUTTON));
      XtAddCallback(dialog, XmNokCallback, popup_record_file, NULL);
      XtAddCallback(dialog, XmNcancelCallback, popup_cancel_file, NULL);

      XmStringFree(str);
   }
   XtManageChild(dialog);
   XtPopup(XtParent(dialog), XtGrabNone);
}

void
popup_program_dialog(
   Widget  w,
   caddr_t client_data,
   XmAnyCallbackStruct *cbs)
{
   popup_record_dialog(_form);
}

void
popup_exit_dialog(
   Widget  w,
   caddr_t client_data,
   XmAnyCallbackStruct *cbs)
{
   audio_stop();
   exit(1);
}

void
popup_play_file(
   Widget w,
   caddr_t client_data,
   XmFileSelectionBoxCallbackStruct  *cbs)
{
   char * file = NULL;

   if (cbs)
   {
      if (!XmStringGetLtoR(cbs->value, XmSTRING_DEFAULT_CHARSET, &file))
         return;

      audio_play(file);
      XtFree(file);
   }
   XtUnmanageChild(w);
}

void
popup_record_file(
   Widget w,
   caddr_t client_data,
   XmFileSelectionBoxCallbackStruct  *cbs)
{
   char * file = NULL;

   if (cbs)
   {
      if (!XmStringGetLtoR(cbs->value, XmSTRING_DEFAULT_CHARSET, &file))
         return;

      _record_file = file;
      XtFree(file);
   }
   XtUnmanageChild(w);

   popup_config_dialog(_form);
}


void
popup_cancel_file(
   Widget w,
   caddr_t client_data,
   XmFileSelectionBoxCallbackStruct  *cbs)
{
   XtUnmanageChild(w);
}


Widget
popup_build(
   Widget  parent,
   int     menu_type)
{
   Widget  menu, widget;
   int i;
   XmString str;

   _form = parent;

   menu = XmCreatePopupMenu(parent, "_popup", NULL, 0);

   for (i = 0; items[i].label != NULL; i++)
   {
      widget = XtVaCreateManagedWidget(items[i].label,
         *items[i]._class, menu, 
         NULL);

      if (items[i].mnemonic)
      {
         XtVaSetValues(widget, XmNmnemonic, items[i].mnemonic, NULL);
      }

      if (items[i].accelerator)
      {
         str = XmStringCreateSimple(items[i].accel_text);
         XtVaSetValues(widget,
            XmNaccelerator, items[i].accelerator,
            XmNacceleratorText, str,
            NULL);
         XmStringFree(str);
      }

      if (items[i].callback)
      {
         XtAddCallback(widget, XmNactivateCallback,
            items[i].callback, items[i].callback_data);
      }
   }
   return (menu);
}


void
popup_device_dialog(
   Widget w)
{
   Arg   args[10];
   int   n;
   Widget  widget, dead_widget, text_widget;
   XmString  str, str1;
   string value;
   string key(PORT_KEY);
   aflibEnvFile env(ENV_FILE);


   str = XmStringCreateLtoR("Please enter the audio device now.\n\
For linux you should use /dev/audio for most cases.",
      XmSTRING_DEFAULT_CHARSET);
   env.readValueFromFile(key, value);

   str1 = XmStringCreateSimple("Configure");
   
   /* Create dialog */
   n = 0;
   XtSetArg(args[n], XmNselectionLabelString, str); n++;
   XtSetArg(args[n], XmNdialogStyle, XmDIALOG_PRIMARY_APPLICATION_MODAL); n++;
   XtSetArg(args[n], XmNdialogTitle, str1); n++;
   widget = XmCreatePromptDialog(w, "Configure", args, n);
   XtAddCallback(widget, XmNokCallback, promptCallback, NULL);

   /* Get the text widget and insert a default value */
   text_widget = XmSelectionBoxGetChild(widget, XmDIALOG_TEXT);
   n = 0;
   XtSetArg(args[n], XmNvalue, value.c_str()); n++;
   XtSetValues(text_widget, args, n);

   /* get rid of two buttons. We only want OK button */
   dead_widget = XmSelectionBoxGetChild(widget, XmDIALOG_HELP_BUTTON);
   XtUnmanageChild(dead_widget);
   dead_widget = XmSelectionBoxGetChild(widget, XmDIALOG_CANCEL_BUTTON);
   XtUnmanageChild(dead_widget);

   XtManageChild(widget);

   XmStringFree(str);
   XmStringFree(str1);
}

void
promptCallback(
   Widget w,
   caddr_t client_data,
   XmSelectionBoxCallbackStruct  *cbs)
{
   char * str;
   string key, device_str;

   /* get the device value entered in the prompt dialog */
   if (!XmStringGetLtoR(cbs->value, XmSTRING_DEFAULT_CHARSET, &str))
   {
      return;
   }

   audio_device(str);

   key = PORT_KEY;
   device_str = str;
   aflibEnvFile env(ENV_FILE);
   env.writeValueToFile(key, device_str);

   XtFree(str);
}

void
popup_record_dialog(
   Widget w)
{
   Widget shell, pane, form1, row;
   Widget form2, close, edit, delete1, add;
   Widget label1, label2, label3, label4, label5, label6;
   Widget label11, label12, label13, label14, label15, label16;
   Widget file;
   Widget place1, place2, place3, place4, place5;
   Arg   args[10];
   int   n;


   n = 0;
   XtSetArg(args[n], XmNdeleteResponse, XmDESTROY); n++;
   shell = XmCreateDialogShell(w, "Programmed Record", args, n);

   n = 0;
   pane = XmCreatePanedWindow(shell, "Record Pane", args, n);

   n = 0;
   form1 = XmCreateForm(pane, "Form1", args, n);

   n = 0;
   XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
   XtSetArg(args[n], XmNpacking, XmPACK_COLUMN); n++;
   XtSetArg(args[n], XmNnumColumns, 4); n++;
   row = XmCreateRowColumn(form1, "RowCol1", args, n);

   n = 0;
   label1 = XmCreateLabel(row, "Start Date", args, n);
   XtManageChild(label1);

   n = 0;
   label2 = XmCreateLabel(row, "Start Time", args, n);
   XtManageChild(label2);

   n = 0;
   label3 = XmCreateLabel(row, "Stop Date", args, n);
   XtManageChild(label3);

   n = 0;
   label4 = XmCreateLabel(row, "Stop Time", args, n);
   XtManageChild(label4);

   n = 0;
   label5 = XmCreateLabel(row, "Each Limit", args, n);
   XtManageChild(label5);

   n = 0;
   label6 = XmCreateLabel(row, "Max Limit", args, n);
   XtManageChild(label6);

   n = 0;
   label11 = XmCreateLabel(row, "(MM/DD/YYYY)", args, n);
   XtManageChild(label11);

   n = 0;
   label12 = XmCreateLabel(row, "(HH:MM:SS)", args, n);
   XtManageChild(label12);

   n = 0;
   label13 = XmCreateLabel(row, "(MM/DD/YYYY)", args, n);
   XtManageChild(label13);

   n = 0;
   label14 = XmCreateLabel(row, "(HH:MM:SS)", args, n);
   XtManageChild(label14);

   n = 0;
   label15 = XmCreateLabel(row, "(Bytes)", args, n);
   XtManageChild(label15);

   n = 0;
   label16 = XmCreateLabel(row, "(Bytes)", args, n);
   XtManageChild(label16);

   n = 0;
   XtSetArg(args[n], XmNcolumns, 10); n++;
   XtSetArg(args[n], XmNmarginWidth, 2); n++;
   _text1 = XmCreateText(row, "Start Date", args, n);
   XtManageChild(_text1);

   n = 0;
   XtSetArg(args[n], XmNcolumns, 10); n++;
   XtSetArg(args[n], XmNmarginWidth, 2); n++;
   _text2 = XmCreateText(row, "Start Time", args, n);
   XtManageChild(_text2);

   n = 0;
   XtSetArg(args[n], XmNcolumns, 10); n++;
   XtSetArg(args[n], XmNmarginWidth, 2); n++;
   _text3 = XmCreateText(row, "Stop Date", args, n);
   XtManageChild(_text3);

   n = 0;
   XtSetArg(args[n], XmNcolumns, 10); n++;
   XtSetArg(args[n], XmNmarginWidth, 2); n++;
   _text4 = XmCreateText(row, "Stop Time", args, n);
   XtManageChild(_text4);

   n = 0;
   XtSetArg(args[n], XmNcolumns, 10); n++;
   XtSetArg(args[n], XmNmarginWidth, 2); n++;
   _text5 = XmCreateText(row, "Each File Limit", args, n);
   XtManageChild(_text5);

   n = 0;
   XtSetArg(args[n], XmNcolumns, 10); n++;
   XtSetArg(args[n], XmNmarginWidth, 2); n++;
   _text6 = XmCreateText(row, "Max File Limit", args, n);
   XtManageChild(_text6);

   n = 0;
   file = XmCreatePushButtonGadget(row, "File", args, n);
   XtAddCallback(file, XmNactivateCallback, popup_record_callback, shell);
   XtManageChild(file);

   n = 0;
   place1 = XmCreateLabel(row, "", args, n);
   XtManageChild(place1);

   n = 0;
   place2 = XmCreateLabel(row, "", args, n);
   XtManageChild(place2);

   n = 0;
   place3 = XmCreateLabel(row, "", args, n);
   XtManageChild(place3);

   n = 0;
   place4 = XmCreateLabel(row, "", args, n);
   XtManageChild(place4);

   n = 0;
   place5 = XmCreateLabel(row, "", args, n);
   XtManageChild(place5);

   n = 0;
   XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
   XtSetArg(args[n], XmNtopWidget, row); n++;
   XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
   XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
   XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
   XtSetArg(args[n], XmNvisibleItemCount, 3); n++;
   list1 = XmCreateScrolledList(form1, "List", args, n);

   XtManageChild(row);
   XtManageChild(list1);
   XtManageChild(form1);

   n = 0;
   XtSetArg(args[n], XmNfractionBase, 4); n++;
   form2 = XmCreateForm(pane, "Form2", args, n);

   n = 0;
   XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
   XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
   XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
   XtSetArg(args[n], XmNleftPosition, 0); n++;
   XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
   XtSetArg(args[n], XmNrightPosition, 1); n++;
   close = XmCreatePushButtonGadget(form2, "Close", args, n);
   XtAddCallback(close, XmNactivateCallback, popup_record_close, shell);
   XtManageChild(close);

   n = 0;
   XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
   XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
   XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
   XtSetArg(args[n], XmNleftPosition, 1); n++;
   XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
   XtSetArg(args[n], XmNrightPosition, 2); n++;
   edit = XmCreatePushButtonGadget(form2, "Edit", args, n);
   XtAddCallback(edit, XmNactivateCallback, popup_record_edit, NULL);
   XtManageChild(edit);

   n = 0;
   XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
   XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
   XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
   XtSetArg(args[n], XmNleftPosition, 2); n++;
   XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
   XtSetArg(args[n], XmNrightPosition, 3); n++;
   delete1 = XmCreatePushButtonGadget(form2, "Delete", args, n);
   XtAddCallback(delete1, XmNactivateCallback, popup_record_delete, NULL);
   XtManageChild(delete1);

   n = 0;
   XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
   XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
   XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
   XtSetArg(args[n], XmNleftPosition, 3); n++;
   XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
   XtSetArg(args[n], XmNrightPosition, 4); n++;
   add = XmCreatePushButtonGadget(form2, "Add", args, n);
   XtAddCallback(add, XmNactivateCallback, popup_record_add, NULL);
   XtManageChild(add);

   XtManageChild(form2);
   XtManageChild(pane);

   XtPopup(shell, XtGrabNone);

   _timer_record_mode = TRUE;

   if (_recorder == NULL)
   {
      _recorder = new aflibAudioRecorder();
   }

   popup_record_update_list();
}

void
popup_record_close(
   Widget  w,
   caddr_t client_data,
   XmAnyCallbackStruct *cbs)
{
   XtDestroyWidget((Widget) client_data);
   _timer_record_mode = TRUE;

   audio_timer_record(_recorder);
}

void
popup_record_edit(
   Widget  w,
   caddr_t client_data,
   XmAnyCallbackStruct *cbs)
{
}

void
popup_record_add(
   Widget  w,
   caddr_t client_data,
   XmAnyCallbackStruct *cbs)
{
   char * str = NULL;
   string s_date, s_time;
   long long each_limit;
   long long max_limit;

   // Get Start Date
   str = XmTextGetString(_text1);
   s_date = str;
   XtFree(str);

   // Get Start Time
   str = XmTextGetString(_text2);
   s_time = str;
   XtFree(str);

   aflibDateTime start(s_date, s_time);

   // Get Stop Date
   str = XmTextGetString(_text3);
   s_date = str;
   XtFree(str);

   // Get Stop Time
   str = XmTextGetString(_text4);
   s_time = str;
   XtFree(str);

   // Get Each Limit
   str = XmTextGetString(_text5);
   //each_limit = atoll(str);
   sscanf(str, "%qd", &each_limit);
   XtFree(str);

   // Get Max Limit
   str = XmTextGetString(_text6);
   //max_limit = atoll(str);
   sscanf(str, "%qd", &max_limit);
   XtFree(str);

   aflibDateTime stop(s_date, s_time);

   _recorder->addRecordItem(start, stop, _record_file, _format_type,
      max_limit, each_limit, _config);

   popup_record_update_list();
}

void
popup_record_update_list()
{
   int segs = 0;
   aflibDateTime start_date, stop_date;
   string  file;
   string file_type;
   long long max_limit;
   long long each_limit;
   aflibConfig config;
   int i;
   char buf[2048];
   XmString  str;

   segs = _recorder->getNumberOfRecordItems();

   XmListDeleteAllItems(list1);

   for (i = 1; i <= segs; i++)
   {
      _recorder->getRecordItem(i,
         start_date, stop_date, file,
         file_type, max_limit, each_limit, config);


      sprintf(buf, "%d  %2.2d/%2.2d/%4d %2.2d:%2.2d:%2.2d  %2.2d/%2.2d/%4d %2.2d:%2.2d:%2.2d  %lld %lld %s",
         i, start_date.getMonth(), start_date.getDay(), start_date.getYear(),
         start_date.getHour(), start_date.getMinute(), start_date.getSecond(),
         stop_date.getMonth(), stop_date.getDay(), stop_date.getYear(),
         stop_date.getHour(), stop_date.getMinute(), stop_date.getSecond(),
         each_limit, max_limit, file.c_str());

      str = XmStringCreateSimple(buf);
      XmListAddItem(list1, str, i);
      XmStringFree(str);
   }
}


void
popup_record_delete(
   Widget  w,
   caddr_t client_data,
   XmAnyCallbackStruct *cbs)
{
   int *position_list;
   int position_count;
   XmListGetSelectedPos(list1, &position_list, &position_count);

   if (position_count != 0)
   {
      _recorder->removeRecordItem(position_list[0]);
   }

   // TBD probably need to delete something

   popup_record_update_list();
}

void
popup_bit_callback(
   Widget w,
   caddr_t client_data,
   XmAnyCallbackStruct *cbs)
{
   _bytes_per_sample = (int)client_data;
}

void
popup_channels_callback(
   Widget w,
   caddr_t client_data,
   XmAnyCallbackStruct *cbs)
{
   _channels = (int)client_data;
}

void
popup_rate_callback(
   Widget w,
   caddr_t client_data,
   XmAnyCallbackStruct *cbs)
{
   _samples_per_second = (int)client_data;
}


void
popup_config_ok(
   Widget w,
   caddr_t client_data,
   XmAnyCallbackStruct *cbs)
{
   aflibConfig config;

   config.setSamplesPerSecond(_samples_per_second);
   if (_bytes_per_sample == 1)
   {
      config.setSampleSize(AFLIB_DATA_8S);
   }
   else
   {
      config.setSampleSize(AFLIB_DATA_16U);
   }
   config.setChannels(_channels);

   if (_timer_record_mode == TRUE)
   {
      _config = config;
   }
   else
   {
      audio_record(_record_file.c_str(), config, _format_type);
      gui_set_record_mode(TRUE);
   }

   XtDestroyWidget((Widget) client_data);
}

void
popup_config_cancel(
   Widget w,
   caddr_t client_data,
   XmAnyCallbackStruct *cbs)
{
   XtDestroyWidget((Widget) client_data);
}

void
popup_format_cb(
   Widget  w,
   caddr_t client_data,
   XmAnyCallbackStruct *cbs)
{
   _format_type = (char *)client_data;
}

void
popup_config_dialog(
   Widget w)
{
   Widget shell, pane, form1, form2;
   Widget channels, stereo, mono;
   Widget label1, label2, label3, label4;
   Widget bitwid, bit8, bit16;
   Widget data_rate, rate_22050, rate_44100;
   Widget data_format;
   Widget cancel, ok;
   Widget format_menu, format_pulldown, temp;
   Arg   args[10];
   int   n;
   list<string> formats;
   list<string> descriptions;
   list<string>::iterator  it;


   n = 0;
   XtSetArg(args[n], XmNdeleteResponse, XmDESTROY); n++;
   shell = XmCreateDialogShell(w, "Record Config", args, n);

   n = 0;
   pane = XmCreatePanedWindow(shell, "Config Pane", args, n);

   n = 0;
   form1 = XmCreateForm(pane, "Form1", args, n);

   n = 0;
   XtSetArg(args[n], XmNradioBehavior, True); n++;
   XtSetArg(args[n], XmNradioAlwaysOne, True); n++;
   XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
   XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
   XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
   XtSetArg(args[n], XmNrightPosition, 20); n++;
   channels = XmCreateRowColumn(form1, "Channels", args, n);
   XtManageChild(channels);

   n = 0;
   XtSetArg(args[n], XmNborderWidth, 0); n++;
   label1 = XmCreateLabel(channels, "MODE", args, n);
   XtManageChild(label1);
   n = 0;
   XtSetArg(args[n], XmNset, True); n++;
   XtSetArg(args[n], XmNborderWidth, 0); n++;
   mono = XmCreateToggleButton(channels, "Mono", args, n);
   XtManageChild(mono);
   XtAddCallback(mono, XmNarmCallback, popup_channels_callback, (void *)1);
   n = 0;
   XtSetArg(args[n], XmNset, False); n++;
   XtSetArg(args[n], XmNborderWidth, 0); n++;
   stereo = XmCreateToggleButton(channels, "Stereo", args, n);
   XtManageChild(stereo);
   XtAddCallback(stereo, XmNarmCallback, popup_channels_callback, (void *)2);

   n = 0;
   XtSetArg(args[n], XmNradioBehavior, True); n++;
   XtSetArg(args[n], XmNradioAlwaysOne, True); n++;
   XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
   XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET); n++;
   XtSetArg(args[n], XmNleftWidget, channels); n++;
   XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
   XtSetArg(args[n], XmNrightPosition, 40); n++;
   bitwid = XmCreateRowColumn(form1, "Bit Width", args, n);
   XtManageChild(bitwid);

   n = 0;
   XtSetArg(args[n], XmNborderWidth, 0); n++;
   label2 = XmCreateLabel(bitwid, "BITS", args, n);
   XtManageChild(label2);
   n = 0;
   XtSetArg(args[n], XmNset, True); n++;
   XtSetArg(args[n], XmNborderWidth, 0); n++;
   bit8 = XmCreateToggleButton(bitwid, "8 Bit", args, n);
   XtManageChild(bit8);
   XtAddCallback(bit8, XmNarmCallback, popup_bit_callback, (void *)8);
   n = 0;
   XtSetArg(args[n], XmNset, False); n++;
   XtSetArg(args[n], XmNborderWidth, 0); n++;
   bit16 = XmCreateToggleButton(bitwid, "16 Bit", args, n);
   XtManageChild(bit16);
   XtAddCallback(bit16, XmNarmCallback, popup_bit_callback, (void *)16);

   n = 0;
   XtSetArg(args[n], XmNradioBehavior, True); n++;
   XtSetArg(args[n], XmNradioAlwaysOne, True); n++;
   XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
   XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET); n++;
   XtSetArg(args[n], XmNleftWidget, bitwid); n++;
   XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
   XtSetArg(args[n], XmNrightPosition, 60); n++;
   data_rate = XmCreateRowColumn(form1, "Date Rate", args, n);
   XtManageChild(data_rate);

   n = 0;
   XtSetArg(args[n], XmNborderWidth, 0); n++;
   label3 = XmCreateLabel(data_rate, "RATE", args, n);
   XtManageChild(label3);
   n = 0;
   XtSetArg(args[n], XmNset, True); n++;
   XtSetArg(args[n], XmNborderWidth, 0); n++;
   rate_22050 = XmCreateToggleButton(data_rate, "22050", args, n);
   XtManageChild(rate_22050);
   XtAddCallback(rate_22050, XmNarmCallback, popup_rate_callback, (void *)22050);
   n = 0;
   XtSetArg(args[n], XmNset, False); n++;
   XtSetArg(args[n], XmNborderWidth, 0); n++;
   rate_44100 = XmCreateToggleButton(data_rate, "44100", args, n);
   XtManageChild(rate_44100);
   XtAddCallback(rate_44100, XmNarmCallback, popup_rate_callback, (void *)44100);

   n = 0;
   XtSetArg(args[n], XmNradioBehavior, True); n++;
   XtSetArg(args[n], XmNradioAlwaysOne, True); n++;
   XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
   XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET); n++;
   XtSetArg(args[n], XmNleftWidget, data_rate); n++;
   XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
   data_format = XmCreateRowColumn(form1, "Format", args, n);
   XtManageChild(data_format);

   n = 0;
   XtSetArg(args[n], XmNborderWidth, 0); n++;
   label4 = XmCreateLabel(data_format, "FORMAT", args, n);
   XtManageChild(label4);

   n = 0;
   format_pulldown = XmCreatePulldownMenu(data_format, "pulldown", args, n);

   n = 0;
   XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
   XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
   XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
   XtSetArg(args[n], XmNtopWidget, label4); n++;
   XtSetArg(args[n], XmNsubMenuId, format_pulldown); n++;
   format_menu = XmCreateOptionMenu(data_format, "Format Menu", args, n);

   aflibFile::returnSupportedFormats(formats, descriptions);
   for (it = formats.begin(); it != formats.end(); it++)
   {
      char * format_str = (char *)(*it).c_str();
      n = 0;
      temp = XmCreatePushButtonGadget(format_pulldown, format_str, args, n);
      XtManageChild(temp);
      XtAddCallback(temp, XmNactivateCallback, popup_format_cb, format_str);
   }
   // Initialize to the first item in the list
   _format_type = formats.begin()->c_str();

   XtManageChild(format_menu);

   XtManageChild(form1);

   n = 0;
   XtSetArg(args[n], XmNfractionBase, 2); n++;
   form2 = XmCreateForm(pane, "Form2", args, n);

   n = 0;
   XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
   XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
   XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
   XtSetArg(args[n], XmNleftPosition, 0); n++;
   XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
   XtSetArg(args[n], XmNrightPosition, 1); n++;
   ok = XmCreatePushButtonGadget(form2, "OK", args, n);
   XtAddCallback(ok, XmNactivateCallback, popup_config_ok, shell);
   XtManageChild(ok);

   n = 0;
   XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
   XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
   XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
   XtSetArg(args[n], XmNleftPosition, 1); n++;
   XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
   XtSetArg(args[n], XmNrightPosition, 2); n++;
   cancel = XmCreatePushButtonGadget(form2, "Cancel", args, n);
   XtAddCallback(cancel, XmNactivateCallback, popup_config_cancel, shell);
   XtManageChild(cancel);

   XtManageChild(form2);
   XtManageChild(pane);

   XtPopup(shell, XtGrabNone);

   // initialize vars
   _channels = 1;
   _samples_per_second = 22050;
   _bytes_per_sample = 1;

}


