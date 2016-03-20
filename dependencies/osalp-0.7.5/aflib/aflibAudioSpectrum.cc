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
#include <iostream>
#include <math.h>

#include "aflibAudioSpectrum.h"
#include "aflibData.h"


/*! \brief Constuctor with an audio parent.

     This will setup for 4 responses per second and 16 samples with 1 channel. No
     callbacks are enabled.
*/
aflibAudioSpectrum::aflibAudioSpectrum(aflibAudio& audio) : aflibAudio(audio)
{
   // Set reasonable defaults
   _num_samples = 16;
   _responses = 4;
   _channels = 1;
   _in_real = NULL;
   _power_out = NULL;
   _spec_out = NULL;
   // Save the number of samples between responses
   _samples_between_responses = audio.getOutputConfig().getSamplesPerSecond() / _responses;
   _samples_counter = 0;

   // Allocate in array
   allocateMemory();

   // Initially callbacks are disabled
   _pm_func_ptr = NULL;
   _spectrum_func_ptr = NULL;
}


/*! \brief Destructor.
*/
aflibAudioSpectrum::~aflibAudioSpectrum()
{
   delete [] _in_real;
   delete [] _power_out;
   delete [] _spec_out;
}


/*! \brief Sets parameters for the audio spectrum class.

    This allows the user to set the number of samples to be returned
    for the spectrum callback. It also determines the size of the
    fft that is done for both the spectrum and power meter callbacks.
    The number of samples must be a power of 2 upto 512. Another words,
    valid values are: 2, 4, 8, 16, 32, 64, 128, 256, 512.
    It also allows the user to set the number of times the callback
    functions will get called a second. There are limits to how large
    this value can be. It also depends on the sample rate of the audio
    stream. Good values would be 10 or less. If an invalid number of
    samples is set then FALSE will be returned.
*/
bool
aflibAudioSpectrum::setParameters(
   int num_samples,
   int resp_per_sec,
   int channels)
{
   bool ret_status = TRUE;

   _responses = resp_per_sec;
   _channels = channels;
   _samples_between_responses = getInputConfig().getSamplesPerSecond() / _responses;
   _samples_counter = 0;

   // Make sure a valid value was entered
   switch (num_samples)
   {
      case 2:
      case 4:
      case 8:
      case 16:
      case 32:
      case 64:
      case 128:
      case 256:
      case 512:
         _num_samples = num_samples;
      break;


      default:
         ret_status = FALSE;
      break;
   }

   // Reallocate memory since params have changed
   allocateMemory();

   return (ret_status);
}


/*! \brief Enables and disables power meter callback.

    This function allows one to register a callback that will get
    called whenever there is data processed showing what the power
    is. It will be called based on the responses per second that was
    set in the setParamters call. The user must register a function
    that receives a double:
    func(double val);
    The value passed will be in decibels. In order to turn off the
    callback pass NULL to this function.
*/
void
aflibAudioSpectrum::setPowerMeterCallback( powermeter_callback func_ptr)
{
   _pm_func_ptr = func_ptr;
	_pm = TRUE;
}


/*! \brief Enables and disables audio spectrum callback.

    This function allows one to register a callback that will get
    called whenever there is data processed showing what the audio
    spectrum is. It will be called based on the responses per second
    that was set in the setParamters call. The user must register a function
    that receives an int and a double array:
    func(int array_size, *double array);
    The first value passed will be the size of the array passed. Users
    should not read beyond this size. The array values passed will be
    in decibels. When func is called users should copy the data if
    they wish to save it as the memory will go out of scope when func
    is exitted. In order to turn off the callback pass NULL to this function. 
*/
void
aflibAudioSpectrum::setAudioSpectrumCallback( spectrum_callback func_ptr)
{
   _spectrum_func_ptr = func_ptr;
	_spectrum = TRUE;
}

void
aflibAudioSpectrum::setPowerMeter(double* power_out)
{
if (_pm_func_ptr)
	_pm_func_ptr(power_out);
}		

void
aflibAudioSpectrum::setAudioSpectrum(int num_samples,double* spec_out)
{
if (_spectrum_func_ptr)
   _spectrum_func_ptr(num_samples, spec_out);
}

void
aflibAudioSpectrum::allocateMemory()
{
   delete [] _in_real;
   delete [] _power_out;
   delete [] _spec_out;

   _in_real = new double[_num_samples * 2 * _channels];
   _power_out = new double[_channels];
   _spec_out = new double[_num_samples * _channels];
}

void
aflibAudioSpectrum::setInputConfig(const aflibConfig& cfg)
{
   // This function overrides the virtual function in the aflibAudio base class.
   // We do this so that we can read the audio configuration data. We then
   // recalculate any data that is dependant on the audio configuration

   _samples_between_responses = cfg.getSamplesPerSecond() / _responses;
   _samples_counter = 0;

   aflibAudio::setInputConfig(cfg);
}

aflibStatus
aflibAudioSpectrum::compute_segment(
   list<aflibData *>& data,
   long long position) 
{
   // This is the function that does all the work. It will gather audio data and then
   // process it with the FFT when it is ready. data is passed in which contains
   // a segment of the audio data. If multithreading is introduced then this function
   // may need to be rewritten since data segments may come uot of sequence. Currently
   // is function requires them to come in sequence since it uses data from more
   // than one segment occasionally. We also in the future need to handle multi
   // channel support.

   double out_real[MAX_ARRAY_SIZE], out_imag[MAX_ARRAY_SIZE];
   int  i, j, k;
   double mag;
   bool   data_ready = FALSE;
   long length;
   int  min_value, max_value;
	double maxmin = 1.0;


   (*data.begin())->getLength(length);
   (*data.begin())->getMinMax(min_value, max_value);


	maxmin = (double)max_value - (double)min_value;


//	aflib_debug("min_value=%f\tmax_value=%f\tmaxmin=%f",min_value,max_value,maxmin);
   // Increment our sample counter
   _samples_counter += length;

   // Check if it is time to gather samples
   if (_samples_counter > _samples_between_responses)
   {
      int real_start, real_stop;

      // Find where we left off in the array we pass to the FFT
      real_start = _samples_counter - _samples_between_responses - length;
      if (real_start < 0)
         real_start = 0;

      // Find where we should stop gathering samples in this array for the FFT for this segment
      real_stop = _num_samples * 2;
      if ((real_stop - real_start) > length)
      {
         real_stop = real_start + length;
      }
      else
      {
         data_ready = TRUE;
      }

      for (k = 0; k < _channels; k++)
      {
         for (i = real_start, j = 0; i < real_stop; i++, j++)
         {
            _in_real[i + (k * _num_samples * 2)] = (double)(*data.begin())->getSample(j, k);
         }
      }
   }

   // IF we have data then lets process it
   if (data_ready)
   {
      for (k = 0; k < _channels; k++)
      {
         // IF one output is selected then process FFT
         if (_pm || _spectrum)
         {
            _fft.fft_double(_num_samples * 2, FALSE, &_in_real[k * _num_samples * 2], NULL, out_real, out_imag);
         }

         // IF user wants power returned
         if (_pm)
         {
            mag = out_real[0] * out_real[0] + out_imag[0] * out_imag[0];
            _power_out[_channels-1] = sqrt(mag);
            _power_out[_channels-1] = 
               20.0 * log10 (_power_out[_channels-1] / maxmin);
         }

         // IF user wants audio spectrum returned
         if (_spectrum)
         {
            for (i = 1; i < _num_samples; i++)
            {
               mag = out_real[i] * out_real[i] + out_imag[i] * out_imag[i];
               out_real[i] = sqrt(mag);
               _spec_out[i + (k * _num_samples) - 1] =
                   20.0 * log10 (out_real[i] / maxmin);
//               fprintf(stderr,"spec_out=%f\tout_real=%f\tmaxmin=%f\n",_spec_out[i + (k * _num_samples) - 1],out_real[i],maxmin);
            }
         }
      }

      if (_pm)
      {
         setPowerMeter(_power_out);
      }
      if (_spectrum)
      {
         setAudioSpectrum(_num_samples, _spec_out);
      }

      if (_samples_between_responses != 0)
      {
         // reset the samples counter by looking for next segment
         while (_samples_counter > _samples_between_responses)
         {
            _samples_counter -= _samples_between_responses;
         }
      }
   }

   return (AFLIB_SUCCESS);
}


