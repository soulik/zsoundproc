// This file contains no license whatsoever. It is provided in the public domain as an example
// of how to use the audio library
//
// Bruce Forsberg
// forsberg@tns.net
// 
//


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <X11/X.h>

extern "C"
{
#include <Xm/Xm.h>
}

#include "gui.h"
#include "audio.h"

#include "aflibConfig.h"
#include "aflibFile.h"
#include "aflibAudioFile.h"
#include "aflibAudioSpectrum.h"
#include "aflibAudioRecorder.h"
#include "aflibAudioPitch.h"
#include "aflibData.h"
#include "aflibSampleData.h"
#include <string>


#define PLAY_DELTA 2048

static int play_delta = PLAY_DELTA;
static bool play = FALSE;
static bool timer_record = FALSE;
static aflibAudio  *input = NULL;
static aflibAudioSpectrum  *spec = NULL;
static aflibAudioRecorder *recorder = NULL;
static aflibAudioPitch *pitch= NULL;
static aflibAudio  *output = NULL;
static aflibConfig     input_config;
static long long  position = 0;
static long long  total_size = 0;
static long long  samples_per_second = 0;
static string play_file;
static string device_file;
static double pitch_value = 1.0;
static bool pitch_enable = FALSE;


Boolean
work_app(XtPointer client_data)
{
   aflibStatus  status;
   int num_samples = play_delta;

   if (timer_record == TRUE)
   {
      recorder->process(status, 0, num_samples);
      if (status != AFLIB_SUCCESS)
      {
         audio_stop();
      }
      usleep(1);
   }
   else if (play == TRUE)
   {
      output->process(status, position, num_samples);
      if (status != AFLIB_SUCCESS)
      {
            audio_stop();
      }
      position += num_samples;
      gui_set_slider_position(position, total_size, samples_per_second);
   }
   else
   {
      usleep(1);
   }

   return FALSE;
}

void
audio_spectrum(int array_size, double *db_array)
{
   gui_spectrum(array_size, db_array);
}

void
audio_play(char * _play)
{
   aflibStatus status;
        

   if (_play != NULL)
   {
      play_file = _play;
    }

    // If no chain then create
   if (output == NULL)
   {
      input = new aflibAudioFile(AFLIB_AUTO_TYPE, play_file.c_str(),
         &input_config, &status);

      if (status != AFLIB_SUCCESS)
      {
         delete input;
         input = NULL;
         cerr << "Can't play file " << play_file.c_str() << endl;
         return;
      }

      spec = new aflibAudioSpectrum(*input);
      spec->setParameters(32, 8, 2);
      spec->setAudioSpectrumCallback(audio_spectrum);

      pitch = new aflibAudioPitch(*spec, pitch_value);
      pitch->enable(pitch_enable);
      //pitch->setCacheEnable(FALSE);

      aflibConfig output_config(input_config);
      output = new aflibAudioFile(*pitch, AFLIB_DEV_TYPE, device_file.c_str(),
         &output_config, &status);

      if (status != AFLIB_SUCCESS)
      {
         delete input;
         delete spec;
         delete output;
         input = NULL;
         output = NULL;
         spec = NULL;
         cerr << "Can't open output device " << device_file.c_str() << endl;
         return;
      }

      total_size = output_config.getTotalSamples();
      samples_per_second = output_config.getSamplesPerSecond();

      play_delta = (int)(PLAY_DELTA * ((double)samples_per_second / 44100.0));

      gui_set_channels(output_config.getChannels());
      gui_set_bits(output_config.getBitsPerSample());

      // Just display the file name with no directory path
      int pos = play_file.rfind("/");
      string file_str = play_file.substr(pos + 1);
      gui_set_file(file_str);

      gui_set_format(((aflibAudioFile *)input)->getFormat());

#if 0
      int num_samples = 10000;
      aflibData * _data = output->process(status, 0, num_samples, FALSE);

      FILE *fd = fopen ("data.fil", "w");
      for (int i = 0; i < 10000; i++)
      {
         fprintf(fd, "%d %d\n", i, _data->getSample(i, 0));
      }
      delete _data;
      fclose(fd);
#endif
   }
   play = TRUE;
}

void
audio_record(
   const char * _play,
   aflibConfig& config,
   const string& format)
{
   aflibStatus status;

   if (_play != NULL)
   {
      play_file = _play;
   }

   // If no chain then create
   if (output == NULL)
   {
      input_config = config;
      input = new aflibAudioFile(AFLIB_DEV_TYPE, device_file.c_str(),
         &input_config, &status);

      if (status != AFLIB_SUCCESS)
      {
         delete input;
         input = NULL;
         cerr << "Can't open input device " << device_file.c_str() << endl;
         return;
      }
      total_size = input_config.getTotalSamples();
      samples_per_second = input_config.getSamplesPerSecond();

      play_delta = (int)(PLAY_DELTA * ((double)samples_per_second / 44100.0));

      spec = new aflibAudioSpectrum(*input);
      spec->setParameters(32, 8, 2);
      spec->setAudioSpectrumCallback(audio_spectrum);

      output = new aflibAudioFile(*spec, format, play_file.c_str(),
         &input_config, &status);

      if (status != AFLIB_SUCCESS)
      {
         delete input;
         delete spec;
         delete output;
         input = NULL;
         output = NULL;
         spec = NULL;
         cerr << "Can't create output file " << play_file.c_str() << endl;
         return;
      }

      gui_set_channels(input_config.getChannels());
      gui_set_bits(input_config.getBitsPerSample());

      // Just display the file name with no directory path
      int pos = play_file.rfind("/");
      string file_str = play_file.substr(pos + 1);
      gui_set_file(file_str);

      gui_set_format(format);

   }
   play = TRUE;
}

void
audio_timer_record(
   aflibAudio * base_ptr)
{
   aflibStatus status;
   aflibDateTime start_date, stop_date;
   string  file;
   string file_type;
   long long max_limit;
   long long each_limit;
   aflibConfig config;

   // If no chain then create
   if (output == NULL)
   {
      recorder = (aflibAudioRecorder *)base_ptr;

      int segs = recorder->getNumberOfRecordItems();
      if (segs == 0)
      {
         return;
      }

      recorder->getRecordItem(1,
         start_date, stop_date, file,
         file_type, max_limit, each_limit, config);

      input_config = config;
      input = new aflibAudioFile(AFLIB_DEV_TYPE, device_file.c_str(),
         &input_config, &status);

      total_size = input_config.getTotalSamples();
      samples_per_second = input_config.getSamplesPerSecond();

      spec = new aflibAudioSpectrum(*input);
      spec->setParameters(32, 5, 2);
      spec->setAudioSpectrumCallback(audio_spectrum);

      recorder->addParent(*spec);
   }
   timer_record = TRUE;
}


void
audio_pause()
{
   play = FALSE;
}

void
audio_stop()
{
   play = FALSE;
   position = 0;

   delete pitch;
   pitch = NULL;

   delete spec;
   spec = NULL;

   delete output;
   output = NULL;

   delete input;
   input = NULL;

   // If recording turn off
   gui_set_record_mode(FALSE);

}

void
audio_device( const char * device )
{
   device_file = device;
}

void
audio_set_position(int pos)
{
   position = (pos * total_size) / 100;
}

void
audio_set_pitch(double factor)
{
   pitch_value = factor;

   if (pitch)
   {
      pitch->setFactor(pitch_value, 0.95);
   }
}

void
audio_pitch()
{
   // Toggle the state of the pitch enable button
   if (pitch_enable == TRUE)
      pitch_enable = FALSE;
   else
      pitch_enable = TRUE;

   // IF pitch is an active object then change its state
   if (pitch)
      pitch->enable(pitch_enable);
}



