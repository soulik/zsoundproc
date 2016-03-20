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


#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "aflibAudioSampleRateCvt.h"
#include "aflibConverter.h"
#include "aflibData.h"
#include "aflibDebug.h"


/*! \brief Basic constructor to place object in a chain

    Different sampling methods can be employed. By setting linear_interpolation to
    TRUE you are selecting the fastest and least quality method of sampling. With this
    set to TRUE the high_quality and filter_interpolation have no effect. This method
    just does a simple interpolation process when resampling occurs. Thus alias
    distoration can occur with this mode. To use high quality resampling the
    linear_interpolation flag should be set to FALSE.
    With linear_interpolation set to FALSE then the high_quality flag can be set. If
    it is set to FALSE then a small filter will be used. If TRUE a large filter will
    be used. The large filter is the best quality and also is the most CPU intensive.
    For most applications the small filter should be more than adequate. With the small
    and large filter a third parameter can be set, filter_interpolation. With
    filter_interpolation set then the filter coefficients used for both the small and
    large filtering will be interpolated as well.
    Defaults for the 3 filter parameters are set in the constructor which will create
    a small filter. This should be sufficient for most cases.
    For a description of factor see the setFactor member function description.
*/
aflibAudioSampleRateCvt::aflibAudioSampleRateCvt(
   aflibAudio& audio,
   double factor,
   bool  linear_interpolation,
   bool  high_quality,
   bool  filter_interpolation) : aflibAudio(audio)
{
   _output_samples = 0;
   _initial = TRUE;
   _next_output_position = 0;
   _next_input_position = 0;
   _save_samples = 0;
   _init_chan = 0;
   _in_array = NULL;
   _in_array_size = 0;
   _out_array = NULL;
   _out_array_size = 0;
   _prev_in_count = 0;
   _vol = 1.0;

   _converter = new aflibConverter(high_quality, linear_interpolation,
      filter_interpolation);

   setFactor(factor);

   // For some factor rates we need extra data at the end to extrapolate
   if (_factor <= 1.0)
      _extra_sample = 50;
   else
      _extra_sample = (int)_factor + 50;

   // By default enable caching in base class
   setCacheEnable(TRUE);
}


/*! \brief Destructor
*/
aflibAudioSampleRateCvt::~aflibAudioSampleRateCvt()
{
   delete _converter;
   delete [] _in_array;
   delete [] _out_array;
}

/*! \brief Sets the sample rate conversion factor.

     This will set the sample rate conversion factor. Virtually any value, within
     reason, can be set. For instance to convert 22050 to 44100 a factor of 0.5
     should be used. To convert 44100 to 22050 a factor of 2.0 should be used. This
     will change the factor that was set in the constructor. This function can also
     be used to change the final amplitude. This only applies to the small and large
     filters and not the linear mode.
*/
void
aflibAudioSampleRateCvt::setFactor(
   double factor,
   double volume)
{
   _factor = factor;
   _vol = volume;

   // calculate the output configuration now that we have a new factor
   const aflibConfig config = getInputConfig();
   setInputConfig(config);

   _initial = TRUE;
}


/*! \brief Sets the input and output audio data configuration of this object.

   This function overrides the aflibAudio base class function. It will change
   the output rate sample rate in the output audio configuration.
*/
void
aflibAudioSampleRateCvt::setInputConfig(const aflibConfig& cfg)
{
   aflibConfig config = cfg;

   config.setSamplesPerSecond((int)(cfg.getSamplesPerSecond() * _factor + 0.5));
   config.setTotalSamples((long long)(cfg.getTotalSamples() * _factor + 0.5));

   // call the base classes setInputConfig to store input configuration
   aflibAudio::setInputConfig(cfg);
   // Store output configuration
   setOutputConfig(config);
}


/*! \brief Performs the sample rate conversion of a segment of data.

   This is the function that does all the work. It does many things. It takes an
   input buffer of data from its parent and adds it to the end of any previously
   unused data from the previous pass. This is done so that data can be streamed.
   The new data is sent to the converter. The convertered data is then saved to the
   output buffer to be returned to the next object. Any unused data is saved for the
   next pass. NOTE: one can disable the caching in the aflibMemCache base class,
   which is enabled by default for this class, and instead use a simple caching
   implemented in this function. 
*/
aflibStatus
aflibAudioSampleRateCvt::compute_segment(
   list<aflibData *>& data,
   long long position) 
{
   int out_count, in_count, old_in_count, channels;
   long  i;
   int   c, counter;

   aflibData * in_data = NULL, * out_data = NULL;
   
   in_data = *data.begin();
   
   aflibConfig out_config(in_data->getConfig());

   channels  = out_config.getChannels();
   in_count = old_in_count = in_data->getLength();

   // If we need to re initialize the rate converter then do so
   if ((_initial == TRUE) || (_init_chan != channels))
   {
      _init_chan = channels;
      _converter->initialize(_factor, _init_chan, _vol);
   }

   // This is set in process. This may have to change in future because this assumes
   // this func gets called after process with data object in that order.
	/* FIXME: sloppy bug-fix. if parent returns less then the requeseted
    * number of samples out count needs to be changed.  This should
    * be fixed in aflibConvertor class resample function instead of here.	
	 */
	
	if(in_count < _requested_samples){
		out_count = (int)ceil((in_count)*_factor);
	} else {
	
	   out_count  = _output_samples;
	}

   // We will reuse the in_array if at all possible
   if ((_in_array) && ((channels * (in_count + _save_samples)) <= _in_array_size))
   {
      // Reuse previously allocated memory
   }
   else
   {
      // Free old memory and allocate new memory
      _in_array_size = channels * (in_count + _save_samples);
      delete [] _in_array;
      _in_array = new short[_in_array_size];
   }

   // We will reuse the out_array if at all possible
   if ((_out_array) && (channels * out_count <= _out_array_size))
   {
      // Reuse previously allocated memory
   }
   else
   {
      // Free old memory and allocate new memory
      _out_array_size = channels * out_count;
      delete [] _out_array;
      _out_array = new short[_out_array_size];
   }

   // If in_array has different alignment we may need to shift some data
   if ((_save_samples != 0) && (_prev_in_count != (in_count + _save_samples)))
   {
      // IF current array is smaller than previous one saved
      if (_prev_in_count > in_count + _save_samples)
      {
         for (c = 1; c < channels; c++)
         {
            for (i = 0; i < _save_samples; i++)
            {
               _in_array[c * (in_count + _save_samples) + i] =
                  _in_array[c * _prev_in_count + i];
            }
         }
      }
      // ELSE current array is larger
      else
      {
         for (c = channels-1; c >= 1; c--)
         {
            for (i = _save_samples-1; i >= 0; i--)
            {
               _in_array[c * (in_count + _save_samples) + i] =
                  _in_array[c * _prev_in_count + i];
            }
         }
      }
   }

   for (c = 0; c < channels; c++)
   {
      for (i = 0; i < in_count; i++)
      {
         _in_array[c * (in_count + _save_samples) + _save_samples + i] =
            (short)(in_data->getSample(i, c));
      }
   }

   // Since in_count is returned we need to add _save_samples
   in_count += _save_samples;
   // Perform the conversion
   int YCount = _converter->resample(in_count, out_count, _in_array, _out_array);

   out_config.setSamplesPerSecond((int)(out_config.getSamplesPerSecond() * _factor + 0.5));
   out_config.setTotalSamples((long long)(out_config.getTotalSamples() * _factor + 0.5));
   out_data = new aflibData(out_config, YCount);

   // Use in_count returned from above to calculate next position
   _next_input_position =  position - _save_samples + in_count;

   // Store output data into a new data structure
   counter = 0;
   for (c = 0; c < channels; c++)
   {
      for (i = 0; i < out_data->getLength(); i++)
      {
         out_data->setSample((int)(_out_array[counter++]), i, c);
      }
   }

   // IF data caching is enabled then don't cache in this class
   if (getCacheEnable() == TRUE)
   {
      _save_samples = 0;
   }
   else
   {
      // Save extra samples in buffer to use next pass
      _save_samples = old_in_count + _save_samples - in_count;
   }
   for (c = 0; c < channels; c++)
   {
      for (i = 0; i < _save_samples; i++)
      {
         _in_array[i + c * (in_count + _save_samples)] =
            _in_array[i + c * (in_count + _save_samples) + in_count];
      }
   }
   _prev_in_count = in_count + _save_samples;

   delete in_data;
   data.clear();
   data.push_back(out_data);

   _initial = FALSE;

   return (AFLIB_SUCCESS);
}


/*! \brief Performs some needed preprocessing before base class function is called.

   This function will take the position and number of samples requested and
   based on several factors will determine what position and number of samples
   is required from its parent. It will also try as best as possible to keep 
   the data streaming. This is so it can read from a device or a format that
   does not have random access. When this function is done it will call the
   aflibAudio base classes function of the same name.
*/
aflibData *
aflibAudioSampleRateCvt::process(
   aflibStatus& ret_status,
   long long position,
   int& num_samples,
   bool free_memory) 
{
   long long orig_position = position;
   int       orig_samples  = num_samples;
   static bool  prev_enable = FALSE;

   _output_samples = num_samples;

   if (getEnable() == TRUE)
   {
      // Change the number of samples we need based on sampling factor
      num_samples = (int)ceil(num_samples / _factor) + _extra_sample;

      if ((position > 0) && (prev_enable != FALSE))
      {
         // If next data block in a sequence
         if (position == _next_output_position)
         {
            // Read input data starting right where we left off plus left over data
            position = _next_input_position + _save_samples;

            // We will use _save_samples from previous run thus we don't need them
            num_samples -= _save_samples;
         }
         else
         {
            position = (long long)floor(position / _factor);

            // Since not in a sequence then throw data away
            _save_samples = 0;
         }
	_next_output_position = orig_position + orig_samples;
      }
      else if(position == 0)
      {
         // Since not in a sequence then throw data away
         _save_samples = 0;
			_next_output_position = orig_position + orig_samples;
      }
	else if(position == -1)
	{
//         _save_samples = 0;
         num_samples -= _save_samples;
	}

      prev_enable = TRUE;
   }
   else
   {
      prev_enable = FALSE;
   }
   
	aflib_debug("num_samples: %i,position: %Ld, _factor %f",
			num_samples,position,_factor);
	_requested_samples = num_samples;

   return (aflibAudio::process(ret_status, position, num_samples, free_memory));
}


/*! \brief Inform base class that this object only supports 16bit signed data.

   This overrides the virtual function in the base class. This algorithm works
   only on 16 bit signed data. Base class must convert the data.
*/
bool
aflibAudioSampleRateCvt::isDataSizeSupported(aflib_data_size size)
{
   bool state = FALSE;

   if (size == AFLIB_DATA_16S)
      state = TRUE;

   return (state);
}

/*! \brief Inform base class that this object only supports 16bit signed data.

   This overrides the virtual function in the base class. This algorithm works
   only on 16 bit signed data. Base class must convert the data.
*/
bool
aflibAudioSampleRateCvt::isChannelsSupported(int& channels)
{

   if (channels <= 2)
		return (TRUE);
	
	channels = 2;

   return (FALSE);
}
