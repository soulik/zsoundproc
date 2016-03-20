/*
 * Copyright: (C) 1999-2001 Bruce W. Forsberg
 *
 *   This library is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU Lesser General Public
 *   License as published by the Free Software Foundation; either
 *   version 2.1 of the License, or any later version.
 *
 *   This library is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *   Lesser General Public License for more details.
 *
 *   You should have received a copy of the GNU Lesser General Public
 *   License along with this library; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
 *
 *   Bruce Forsberg  forsberg@tns.net
 *
 */


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>

#include "aflibSampleData.h"
#include "aflibAudio.h"
#include "aflibConfig.h"
#include "aflibData.h"

#define MAX_SAMPLES 100


aflibSampleData::aflibSampleData(
   const aflibAudio * file)
{
   _minmax_mode = FALSE;
   _file = file;
   _output_max = 100;
   _output_min = 0;
   _num_points = 100;
   _start_sample = 0;
   _delta = 0;
   
   const aflibConfig& config = _file->getOutputConfig();

   _num_channels = config.getChannels();
   _stop_sample = config.getTotalSamples();

   _channel_array = new int[_num_channels];

   for (unsigned int i = 0; i < _num_channels; i++)
   {
      _channel_array[i] = i + 1;
   }

   switch (config.getSampleSize())
   {
      case AFLIB_DATA_8S:
         _a0 = 128;
         _a1 = 256;
      break;

      case AFLIB_DATA_8U:
         _a0 = 0;
         _a1 = 256;
      break;

      case AFLIB_DATA_16S:
         _a0 = 32768;
         _a1 = 65536;
      break;

      case AFLIB_DATA_16U:
         _a0 = 0;
         _a1 = 65536;
      break;

      default:
      break;
   }
}

aflibSampleData::~aflibSampleData()
{
   delete [] _channel_array;
}

void
aflibSampleData::setOutputRange(
   int  min,
   int  max)
{
   _output_max = max;
   _output_min = min;
}

void
aflibSampleData::setMaxMinMode(bool mode)
{
   _minmax_mode = mode;
}

void
aflibSampleData::setNumSamplePoints(
   unsigned int points)
{
   _num_points = points;
}

void
aflibSampleData::setParametersSamples(
   long long start_sample,
   long long stop_sample)
{
   _start_sample = start_sample;
   _stop_sample = stop_sample;
}

void
aflibSampleData::setParametersSeconds(
   double start_seconds,
   double stop_seconds)
{
   const aflibConfig& config = _file->getOutputConfig();

   _start_sample = (long long)(config.getSamplesPerSecond() * start_seconds);
   _stop_sample = (long long)(config.getSamplesPerSecond() * stop_seconds);
}

void
aflibSampleData::setChannels(
   int num_channels,
   int * array)
{
   const aflibConfig& config = _file->getOutputConfig();

   if (num_channels <= config.getChannels())
   {
      _num_channels = num_channels;
      for (unsigned int i = 0; i < _num_channels; i++)
      {
         _channel_array[i] = array[i];
      }
   }

}


bool
aflibSampleData::process(
   int * array)
{
   int max_min;
   unsigned int i, j, k;
   aflibData * data;
   int * ptr;
   aflibStatus  status;
   int num_samples;
   int accum;
   int minimum = 0;
	long good_sample = 0;

   max_min = _output_max - _output_min;
   _delta= (_stop_sample - _start_sample) / (_num_points);

   if (_delta == 0)
      _delta = 1;

   if (_minmax_mode == TRUE)
   {
      if (_delta / 2 > MAX_SAMPLES)
      {
         num_samples = MAX_SAMPLES;
      }
      else
      {
         num_samples = _delta / 2;
         if (num_samples < 1)
         {
            num_samples = 1;
         }
      }
   }
   else
   {
      num_samples = 1;
   }

   for (i = 0; i < _num_points; i++)
   {
      data = ((aflibAudio*)_file)->process(status, _start_sample + _delta * i, num_samples, FALSE);
      // TBD need to put something in array if error
      if (status == AFLIB_SUCCESS)
      {
			good_sample = _start_sample + _delta*i + num_samples;
         for (j = 0; j < _num_channels; j++)
         {
            ptr = &array[_num_points * j]; 
            // reset min max accumulation values
           	accum = (data->getSample(0, _channel_array[j] - 1) + _a0) * (max_min) / _a1 + _output_min;
            // Look at each sample trying to find min max in range
            for (k = 1; k < (unsigned int)num_samples; k++)
            {
               ptr[i] =
                  (data->getSample(k, _channel_array[j] - 1) + _a0) * (max_min) / _a1 + _output_min;
               // IF looking for max value
               if (minimum == 0)
               {
                  if (ptr[i] > accum)
                  {
                     accum = ptr[i];
                  }
               }
               // ELSE minimum value
               else
               {
                  if (ptr[i] < accum)
                  {
                     accum = ptr[i];
                  }
               }
            }
            ptr[i] = accum;
         }
      } else { // Premature EOF ?
			_num_points = i;
			_stop_sample = good_sample;
			delete data;
			break;
		}
      delete data;
      if (minimum == 0)
         minimum = 1;
      else
         minimum = 0;
   }
   return (TRUE);
}

int
aflibSampleData::getDelta() const
{
   return (_delta);
}


