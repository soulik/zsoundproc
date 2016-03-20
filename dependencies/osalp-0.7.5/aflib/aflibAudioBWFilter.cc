/* 
This file was derived from the following work listed below. It
was modified for this class by Bruce W. Forsberg (forsberg@tns.net).
 
Modified from spkit software. Files
butterwo
bwbandpa
bwbandre
bwhighpa
bwlowpas
 
    Sound Processing Kit - A C++ Class Library for Audio Signal Processing
    Copyright (C) 1995-1998 Kai Lassfolk
 
    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.
 
    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.
 
    You should have received a copy of the GNU Library General Public
    License along with this library; if not, write to the Free
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <iostream>
#include <math.h>

#include "aflibAudioBWFilter.h"
#include "aflibData.h"

#define  REDUCTION_VALUE 0.90


/*! \brief Constuctor with an audio parent.

     This constructor will setup the Butterworth filter. It accepts a mode
     which can be one of LOW_PASS, HIGH_PASS, BAND_PASS, and BAND_REJECT.
     The freq parameter applies to all 4 modes while bw is ignored for 
     LOW_PASS and HIGH_PASS. The freq parameter represents the corner
     frequency for LOW_PASS or HIGH_PASS and the center frequency for
     BAND_PASS or BAND_REJECT. The bw parameter represents the bandwidth.
     Both freq and bw should be specified in Hertz.
*/
aflibAudioBWFilter::aflibAudioBWFilter(
   aflibAudio& audio,
   aflibAudioBWFilter::AFLIB_BWFILTER_MODE mode,
   double              freq,
   double              bw) : aflibAudio(audio)
{
   _mode = mode;
   _freq = freq;
   _bw = bw;
   _buf_x0 = NULL;
   _buf_x1 = NULL;
   _buf_y0 = NULL;
   _buf_y1 = NULL;
   _chan = 0;
}



/*! \brief Destructor.
*/
aflibAudioBWFilter::~aflibAudioBWFilter()
{
   delete [] _buf_x0;
   delete [] _buf_x1;
   delete [] _buf_y0;
   delete [] _buf_y1;
}


/*! \brief Sets parameters for the butterworth filter class.
 
    This allows the user to change the filter parameters of the butterworth
    filter after the class has been setup. With this one could change the
    center frequency or bandwidth dynamically as data is streaming.
*/
void
aflibAudioBWFilter::setParameters(
   aflibAudioBWFilter::AFLIB_BWFILTER_MODE mode,
   double              freq,
   double              bw)
{
   const aflibConfig&  cfg = getInputConfig();

   // Save current settings
   _mode = mode;
   _freq = freq;
   _bw = bw;

   // Setup all coefficients for proper mode
   if (_mode == aflibAudioBWFilter::BAND_PASS)
   {
      _C = 1.0 / tan(M_PI * _bw / cfg.getSamplesPerSecond());
      _D = 2 * cos(2 * M_PI * _freq / cfg.getSamplesPerSecond());
 
      _a[0] = 1.0 / (1.0 + _C);
      _a[1] = 0.0;
      _a[2] = -_a[0];
 
      _b[0] = -_C * _D * _a[0];
      _b[1] = (_C - 1.0) * _a[0];
   }
   else if (_mode == aflibAudioBWFilter::LOW_PASS)
   {
      _C = 1.0 / tan(M_PI * _freq / cfg.getSamplesPerSecond());
 
      _a[0] = 1.0 / (1.0 + sqrt(2.0) * _C + _C * _C);
      _a[1] = 2.0 * _a[0];
      _a[2] = _a[0];
 
      _b[0] = 2 * (1.0 - _C * _C) * _a[0];
      _b[1] = (1.0 - sqrt(2.0) * _C + _C * _C) * _a[0];
   }
   else if (_mode == aflibAudioBWFilter::HIGH_PASS)
   {
      _C = tan(M_PI * _freq / cfg.getSamplesPerSecond());
 
      _a[0] = 1.0 / (1.0 + sqrt(2.0) * _C + _C * _C);
      _a[1] = -2.0 * _a[0];
      _a[2] = _a[0];
 
      _b[0] = 2 * (_C * _C - 1.0) * _a[0];
      _b[1] = (1.0 - sqrt(2.0) * _C + _C * _C) * _a[0];
   }
   else if (_mode == aflibAudioBWFilter::BAND_REJECT)
   {
      _C = tan(M_PI * _bw / cfg.getSamplesPerSecond());
      _D = 2 * cos(2 * M_PI * _freq / cfg.getSamplesPerSecond());
 
      _a[0] = 1.0 / (1.0 + _C);
      _a[1] = -_D * _a[0];
      _a[2] = _a[0];
 
      _b[0] = _a[1];
      _b[1] = (1.0 - _C) * _a[0];
   }
}


void
aflibAudioBWFilter::setInputConfig(const aflibConfig& cfg)
{
   // This function overrides the virtual function in the aflibAudio base class.
   // We do this so that we can read the audio configuration data. We then
   // recalculate any data that is dependant on the audio configuration
 

   aflibAudio::setInputConfig(cfg);

   // Process any new information on new config data
   _chan = cfg.getChannels();
   allocateMemory();
   setParameters(_mode, _freq, _bw);
}


void
aflibAudioBWFilter::allocateMemory()
{
   int  i;


   // Remove any unused memory
   delete [] _buf_x0;
   delete [] _buf_x1;
   delete [] _buf_y0;
   delete [] _buf_y1;

   // Allocate new arrays
   _buf_x0 = new double[_chan];
   _buf_x1 = new double[_chan];
   _buf_y0 = new double[_chan];
   _buf_y1 = new double[_chan];

   // Zero out memory
   for (i = 0; i < _chan; i++)
   {
      _buf_x0[i] = 0.0;
      _buf_x1[i] = 0.0;
      _buf_y0[i] = 0.0;
      _buf_y1[i] = 0.0;
   }
}


aflibStatus
aflibAudioBWFilter::compute_segment(
   list<aflibData *>& data,
   long long position) 
{
   int i, j;
   long length;
   double value, output;
   aflibData  *ptr = NULL;


   // Get a pointer to data object
   ptr = (*(data.begin()));

   // Get length of data to process
   ptr->getLength(length);

   // For every data element of every channel process
   for (i = 0; i < _chan; i++)
   {
      for (j = 0; j < length; j++)
      {
         value = (double)(ptr->getSample(j, i));
         output = _a[0] * value + _a[1] * _buf_x0[i] + _a[2] * _buf_x1[i]
                   - _b[0] * _buf_y0[i] - _b[1] * _buf_y1[i];

         ptr->setSample((int)(output * REDUCTION_VALUE), j, i);
 
         _buf_x1[i] = _buf_x0[i];
         _buf_x0[i] = value;
         _buf_y1[i] = _buf_y0[i];
         _buf_y0[i] = output;
      }
   }

   return (AFLIB_SUCCESS);
}


/*! \brief Inform base class that this object only supports 16bit signed data.
 
   This overrides the virtual function in the base class. This algorithm works
   only on 16 bit signed data. Base class must convert the data.
*/
bool
aflibAudioBWFilter::isDataSizeSupported(aflib_data_size size)
{
   bool state = FALSE;

   if (size == AFLIB_DATA_16S)
      state = TRUE;
 
   return (state);
}

