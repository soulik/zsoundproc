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


#include <math.h>
#include <iostream>

using std::cerr;
using std::endl;

#include "aflibAudioConstantSrc.h"
#include "aflibData.h"


/*! \brief Constructor - defaults to a DC level fullscale signal.
*/
aflibAudioConstantSrc::aflibAudioConstantSrc(aflibAudio& audio) : aflibAudio(audio)
{
    // Default to a DC level fullscale
   _wave = aflibAudioConstantSrc::WAVEFORM_DCLEVEL;
   _amp_max = 100.0;
   _amp_min = -100.0;
   _freq = 0;
}


/*! \brief Constructor - defaults to a DC level fullscale signal.
*/
aflibAudioConstantSrc::aflibAudioConstantSrc() : aflibAudio()
{
    // Default to a DC level fullscale
   _wave = aflibAudioConstantSrc::WAVEFORM_DCLEVEL;
   _amp_max = 100.0;
   _amp_min = -100.0;
   _freq = 0;
}


/*! \brief Destructor
*/
aflibAudioConstantSrc::~aflibAudioConstantSrc()
{
}


/*! \brief - Sets the type of waveform to be output.

    Allow the user to select the waveform type, amplitude, and frequency. The amplitude
    will be the peak amplitude of the signal specified in percent. Both a max and min
    are required. For full scale one would use 100.0 for max and -100.0 for min. For the
    waveform DC level only max is used and min must be less than or equal to max. The
    frequency must be in Hz.
*/
void
aflibAudioConstantSrc::setWaveformType(
   aflibAudioConstantSrc::waveform_type wave,
   double max_amp,
   double min_amp,
   int  frequency)
{
   double temp;


   _wave = wave;

   _amp_max = max_amp;
   if (_amp_max > 100.0)
      _amp_max = 100.0;
   else if (_amp_max < -100.0)
      _amp_max = -100.0;

   _amp_min = min_amp;
   if (_amp_min > 100.0)
      _amp_min = 100.0;
   else if (_amp_min < -100.0)
      _amp_min = -100.0;

   // IF values are backwards then swap
   if (_amp_max < _amp_min)
   {
      temp = _amp_max;
      _amp_max = _amp_min;
      _amp_min = temp;
   }

   _freq = frequency;
}


aflibStatus
aflibAudioConstantSrc::compute_segment(
   list<aflibData *>& data,
   long long position) 
{
   // This is the function that does the work. It will output the waveform and
   // amplitude based on the values that were set in the function setWaveformType.
   // It bases its output on the position passed in.

   int c, i;
   int period = 0;
   int cycle_period = 0;
   double value;
   int int_value;
   int  min_value, max_value, mid_value, half_value;
   double min_double, max_double, m ,b;


   (*data.begin())->getMinMax(min_value, max_value);
   half_value = (max_value - min_value) / 2;
   mid_value = max_value - half_value;
   min_double = (double)half_value * ((double)fabs(_amp_min) / 100.0);
   min_double = (double)mid_value - min_double;
   max_double = (double)half_value * ((double)_amp_max / 100.0);
   max_double = (double)mid_value + max_double;
   min_value = (int)min_double;
   max_value = (int)max_double;

   b = (min_double + max_double) / 2;
   m = (max_double - min_double) / 2;

   if (_freq != 0)
   {
      period = (*data.begin())->getConfig().getSamplesPerSecond() / _freq;
      cycle_period = (int)(position % (long long)period);
   }

   // DC LEVEL
   if (_wave == aflibAudioConstantSrc::WAVEFORM_DCLEVEL)
   {
      // For every sample in every channel store a dc level
      for (c = 0; c < (*data.begin())->getConfig().getChannels(); c++)
      {
         for (i = 0; i < (*data.begin())->getLength(); i++)
         {
            (*data.begin())->setSample(max_value, i, c);
         }
      }
   }
   // TRIANGLE
   else if (_wave == aflibAudioConstantSrc::WAVEFORM_TRIANGLE)
   {
      // For every sample in every channel
      for (c = 0; c < (*data.begin())->getConfig().getChannels(); c++)
      {
         for (i = 0; i < (*data.begin())->getLength(); i++)
         {
            value = (double) ((i + cycle_period) % period);
            // First quarter cycle ramp up to + max
            if (value < period / 4)
            {
               value = value / ((double)period / 4.0);
            }
            // 2nd quarter ramp from + max to middle
            else if (value < period / 2)
            {
               value = (value - period / 4.0) / ((double)period / 4.0);
               value = 1.0 - value;
            }
            // 3rd quarter ramp from mid to - min
            else if ( value < (period * 3 / 4) )
            {
               value = (value - period / 2.0) / ((double)period / 4.0);
               value = value * -1.0;
            }
            // 4th quarter from - min to middle
            else
            {
               value = (value - (period * 3 / 4.0)) / ((double)period / 4.0);
               value = (1.0 - value) * -1.0;
            }

            value = m * value + b;
            (*data.begin())->setSample((int)value, i, c);
         }
      }
   }
   // SINE WAVE
   else if (_wave == aflibAudioConstantSrc::WAVEFORM_SINEWAVE)
   {
      // For every sample in every channel store the sine wave
      for (c = 0; c < (*data.begin())->getConfig().getChannels(); c++)
      {
         for (i = 0; i < (*data.begin())->getLength(); i++)
         {
            // Compute correct value for position in waveform
            value = sin(2 * M_PI * (double)(i + cycle_period) / (double)period);

            value = m * value + b;
            (*data.begin())->setSample((int)value, i, c);
         }
      }
   }
   // SQUARE WAVE
   else if (_wave == aflibAudioConstantSrc::WAVEFORM_SQUAREWAVE)
   {
      // For every sample in every channel
      for (c = 0; c < (*data.begin())->getConfig().getChannels(); c++)
      {
         for (i = 0; i < (*data.begin())->getLength(); i++)
         { 
            value = (double) ((i + cycle_period) % period);
            if (value < period / 2)
            { 
               int_value = max_value;
            }
            else
            {
               int_value = min_value;
            }
            (*data.begin())->setSample(int_value, i, c);
         }
      }
   }
   else
   {
      cerr << "Unsupported waveform type" << endl;
   }

   return (AFLIB_SUCCESS);
}


