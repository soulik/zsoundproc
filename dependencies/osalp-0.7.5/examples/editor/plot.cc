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


#include "plot.h"
#include "gui.h"
#include "plot_data.h"
#include "graph_widget.h"
#include "connect_widget.h"

#include "aflibConfig.h"
#include "aflibAudioFile.h"
#include "aflibAudioEdit.h"
#include "aflibAudioSpectrum.h"
#include "aflibAudioSampleRateCvt.h"
#include "aflibData.h"
#include "aflibSampleData.h"
#include <string>
#include <vector>

#define DRAW_WIDTH 500
#define DRAW_HEIGHT 100
#define PLAY_DELTA 1024

enum mode_enum
{
EDITOR_OUTPUT,
EDITOR_INPUT
};


static aflibAudioFile * input = NULL;
static aflibAudioEdit * edit = NULL;
static aflibAudioFile * output = NULL;
static XtWorkProcId  work_id = 0;
static long long current_position = 0;
static bool currently_playing = FALSE;
static graph_widget *g_w = NULL;
static graph_widget *i_w = NULL;
static connect_widget *c_w = NULL;
static vector<plot_data *> input_array;
static int  input_num = 0;
static Widget parent_widget;
static long long start_sample, stop_sample;
static bool save_as_mode = FALSE;
static mode_enum mode = EDITOR_OUTPUT;



static void
plot_connect()
{
   if (edit)
   {
      c_w->set_parameters(g_w->getBeginSamples(), g_w->getEndSamples(),
         i_w->getStartSamples(), i_w->getStopSamples(), edit,
         input_array[input_num-1]->getID());
   }
}

static void
plot_output()
{
   aflibConfig config;
   aflibStatus status = AFLIB_SUCCESS;
   int  * array;
   int ch_array[2];
   long long position;
   long long display_samples;
   int       num_samples;


   // process at least one pixel to get config data
   position = 0;
   num_samples = 1;
   edit->process(status, position, num_samples);
   config = edit->getOutputConfig();

   // calculate start and stop samples based on shift and divisor
   display_samples = config.getTotalSamples() / g_w->getDivisor();
   start_sample = g_w->getShiftData() * display_samples / DRAW_WIDTH;
   stop_sample = start_sample + display_samples;
   // Dont display past end of data
   if (stop_sample > config.getTotalSamples())
   {
      stop_sample = config.getTotalSamples();
      start_sample = stop_sample - display_samples;
   }

   array = new int[DRAW_WIDTH * config.getChannels()];

   aflibSampleData sample(edit);
   sample.setMaxMinMode(TRUE);
   sample.setOutputRange(0, DRAW_HEIGHT);
   sample.setNumSamplePoints(DRAW_WIDTH);
   sample.setParametersSamples(start_sample, stop_sample);
   // TBD this needs to be fixed for more than two channels
   ch_array[0] = 1;
   ch_array[1] = 2;
   sample.setChannels(2, ch_array);
   sample.process(array);

   g_w->setChannels(config.getChannels());
   g_w->setMarkerResolution(sample.getDelta());
   g_w->setSamplesPerSecond(config.getSamplesPerSecond());
   g_w->setStartSamples(start_sample);
   g_w->setStopSamples(stop_sample);
   g_w->setBeginSamples(0);
   g_w->setEndSamples(stop_sample - start_sample);
   g_w->setTotalSamples(config.getTotalSamples());

   g_w->load_new_data(array);
   g_w->perform_expose();

   delete [] array;

   plot_connect();

}


void
plot_input(
   char * text_file)
{
   aflibConfig config;
   aflibStatus status = AFLIB_SUCCESS;
   static char * text_str = NULL;
   int  * array;
   int ch_array[2];
   long long position;
   long long display_samples;
   int       num_samples;
   plot_data  * plot_input;
   int          input_id; 
   char         str[100];
   long long    start_sam, stop_sam;
   

 
   if (text_file != NULL)
   {
      aflibAudioFile * last_input = input;
      
      if (text_str != NULL)
      {
         free(text_str);
      }
      text_str = strdup(text_file);

      input = new aflibAudioFile(AFLIB_AUTO_TYPE, text_str, &config, &status);
      if (status != AFLIB_SUCCESS)
      {
         cout << "Error opening input !" << endl;
         delete input;
         input = last_input;
         return;
      }
      input->setCacheEnable(TRUE);

      if (edit)
      {
         // Add this as an input to audio editor
         input_id = edit->addParent(*(aflibAudio *)input);
      }
      else
      {
         edit = new aflibAudioEdit(*input);
         input_id = 1;
      }

      plot_input = new plot_data(input, input_id);
      // TBD need to place at correct position in vector
      input_array.push_back(plot_input);
      input_num = input_array.size();

      add_input_button(input_num);
   }

   // Set correct name for proper input
   sprintf(str, "Input %d", input_num);
   i_w->setName(str);

   // process at least one pixel to get config data
   position = 0;
   num_samples = 1;
   input->process(status, position, num_samples);
   config = input->getOutputConfig();
 
   // calculate start and stop samples based on shift and divisor
   display_samples = config.getTotalSamples() / i_w->getDivisor();
   start_sam = display_samples * i_w->getShiftData() / DRAW_WIDTH;
   stop_sam = start_sam + display_samples;
   // Dont display past end of data
   if (stop_sam > config.getTotalSamples())
   {
      stop_sam = config.getTotalSamples();
      start_sam = stop_sam - display_samples;
   }

 
   array = new int[DRAW_WIDTH * config.getChannels()];
 
   aflibSampleData sample(input);
   sample.setMaxMinMode(TRUE);
   sample.setOutputRange(0, DRAW_HEIGHT);
   sample.setNumSamplePoints(DRAW_WIDTH);
   sample.setParametersSamples(start_sam, stop_sam);
   // TBD this needs to be fixed for more than two channels
   ch_array[0] = 1;
   ch_array[1] = 2;
   sample.setChannels(2, ch_array);
   sample.process(array);

   i_w->setChannels(config.getChannels());
   i_w->setMarkerResolution(sample.getDelta());
   i_w->setSamplesPerSecond(config.getSamplesPerSecond());
   i_w->setStartSamples(start_sam);
   i_w->setStopSamples(stop_sam);
   i_w->setBeginSamples(0);
   i_w->setEndSamples(stop_sam - start_sam);
   i_w->setTotalSamples(config.getTotalSamples());
 
   i_w->load_new_data(array);
   i_w->perform_expose();

    delete [] array;

   plot_connect();
}


void
plot_select_input(int num)
{
   input = (aflibAudioFile *)input_array[num-1]->getAfLibAudio();
   input_num = num;

   // IF playing an input track and different input selected stop play
   if (mode == EDITOR_INPUT)
      plot_stop();

   plot_input(NULL);
   plot_connect();
}


void
plot_enlarge()
{
   if (mode == EDITOR_OUTPUT)
   {
      g_w->setDivisor( g_w->getDivisor() * 2 );
      g_w->setShiftData( g_w->getShiftData() * 2 );
      plot_output();
   }
   else
   {
      i_w->setDivisor( i_w->getDivisor() * 2 );
      i_w->setShiftData( i_w->getShiftData() * 2 );
      plot_input(NULL);
   }
}

void
plot_reduce()
{
   if (mode == EDITOR_OUTPUT)
   {
      g_w->setDivisor( g_w->getDivisor() / 2 );
      g_w->setShiftData( g_w->getShiftData() / 2 );
      plot_output();
   }
   else
   {
      i_w->setDivisor( i_w->getDivisor() / 2 );
      i_w->setShiftData( i_w->getShiftData() / 2 );
      plot_input(NULL);
   }
}

void
plot_shift_left(int value)
{
   if (mode == EDITOR_OUTPUT)
   {
      g_w->setShiftData( g_w->getShiftData() - value );
      plot_output();
   }
   else
   {
      i_w->setShiftData( i_w->getShiftData() - value );
      plot_input(NULL);
   }
}

void
plot_shift_right(int value)
{
   if (mode == EDITOR_OUTPUT)
   {
      g_w->setShiftData( g_w->getShiftData() + value );
      plot_output();
   }
   else
   {
      i_w->setShiftData( i_w->getShiftData() + value );
      plot_input(NULL);
   }
}

Boolean
plot_play_app(XtPointer client_data)
{
   aflibStatus status;
   int delta = PLAY_DELTA;
   static int counter = 0;
   graph_widget  * wid;
   int counter_delta;


   // IF saving a file or playing then filter X events
   if (save_as_mode == TRUE)
      counter_delta = 100;
   else
      counter_delta = 20;

   // IF processing output
   if (mode == EDITOR_OUTPUT)
   {
      wid = g_w;
   }
   else
   {
      wid = i_w;
   }

   counter++;
   currently_playing = TRUE;
   if (output == NULL)
   {
      current_position = 0;
      currently_playing = TRUE;
      return (TRUE);
   }

   if (current_position == 0)
   {
      current_position =  wid->getBeginSamples();
   }

   // Update the play marker
   wid->setCurrentSamples(current_position);

   // Don't stop until we reach end marker
   if (current_position >= wid->getEndSamples())
   {
      if (work_id != 0)
         XtRemoveWorkProc(work_id);
      work_id = 0;
      delete output;
      output = NULL;
      currently_playing = TRUE;
      current_position = 0;
   }
   else
   {
      output->process(status, current_position + start_sample, delta);

      current_position += delta;
   }

   // Filter the expose events otherwise they eat all the CPU.
   if ((counter % counter_delta == 0))
      wid->perform_expose();

   return(FALSE);
}

void
plot_play(char * text_file)
{
   aflibConfig config;
   aflibStatus status = AFLIB_SUCCESS;
   Display *dis = XtDisplay(parent_widget);
   XtAppContext app = XtDisplayToApplicationContext(dis);
   aflibAudio  * base;


   // IF we are processing output
   if (mode == EDITOR_OUTPUT)
   {
      // If chain not setup then exit
      if (edit == NULL)
         return;
      base = edit;
   }
   else
   {
      base = input_array[input_num-1]->getAfLibAudio();
   }

   config = base->getOutputConfig();
   work_id = XtAppAddWorkProc(app, plot_play_app, NULL);

   delete output;

   if (text_file == NULL)
   {
      save_as_mode = FALSE;
      output = new aflibAudioFile(*base, AFLIB_DEV_TYPE, "/dev/audio", &config, &status);
   }
   else
   {
      save_as_mode = TRUE;

      // reset display to show all data for progress
      g_w->setDivisor(1);
      g_w->setShiftData(0);
      plot_output();

      // TBD need to add select output type
      output = new aflibAudioFile(*base, AFLIB_WAV_TYPE, text_file, &config, &status);
   }
   if (status != AFLIB_SUCCESS)
   {
      cout << "Error opening output device !" << endl;
   }
}

void
plot_pause()
{
   Display *dis = XtDisplay(parent_widget);
   XtAppContext app = XtDisplayToApplicationContext(dis);

   if (currently_playing == TRUE)
   {
      if (work_id != 0)
         XtRemoveWorkProc(work_id);
      work_id = 0;
      currently_playing = FALSE;
   }
   else
   {
      work_id = XtAppAddWorkProc(app, plot_play_app, NULL);
      currently_playing = TRUE;
   }
}

void
plot_stop()
{
   if (work_id != 0)
      XtRemoveWorkProc(work_id);
   work_id = 0;
   delete output;
   output = NULL;
   currently_playing = TRUE;
   current_position = 0;
   i_w->perform_expose();
   g_w->perform_expose();
}

void
plot_cut()
{
   if (edit == NULL)
      return;

   edit->removeSegment(g_w->getBeginSamples() + start_sample,
                       g_w->getEndSamples() + start_sample);

   plot_output();
}

void
plot_reset()
{
   graph_widget  * wid;


   // IF processing output
   if (mode == EDITOR_OUTPUT)
   {
      wid = g_w;
   }
   else
   {
      wid = i_w;
   }

   wid->setBeginSamples(wid->getStartSamples());
   wid->setEndSamples(wid->getStopSamples());

   wid->perform_expose();
}

void
plot_erase()
{
   // Remove all segments from this input
   edit->removeSegmentsFromInput(input_array[input_num-1]->getID());

   plot_output();
}

void
plot_build(
   Widget parent)
{
   parent_widget = parent;

   g_w = new graph_widget(parent, NULL, NULL, NULL, NULL, DRAW_HEIGHT, DRAW_WIDTH, 1);
   g_w->setName("Output Channel");

   c_w = new connect_widget(parent, NULL, NULL, g_w->getWidget(), NULL, 50, DRAW_WIDTH);

   i_w = new graph_widget(parent, NULL, NULL, c_w->getWidget(), NULL, DRAW_HEIGHT, DRAW_WIDTH, 1);
   i_w->setName("Input");
}

void
plot_insert()
{
   if (edit)
   {
      // Insert data between begin and end markers of input to begin marker of output
      edit->addSegment(input_array[input_num-1]->getID(), i_w->getBeginSamples(),
         i_w->getEndSamples(), g_w->getBeginSamples());
   }
   plot_output();
}

void
plot_set_mode(bool set_mode)
{
   // 1 = Output Mode
   // 0 = Input Mode

   if (set_mode == 1)
      mode = EDITOR_OUTPUT;
   else
      mode = EDITOR_INPUT;

   // IF playing, stop
   plot_stop();
}



