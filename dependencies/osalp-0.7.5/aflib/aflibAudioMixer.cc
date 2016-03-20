/*
 * Copyright: (C) 2000-2001 Bruce W. Forsberg
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

#include "aflibAudioMixer.h"
#include "aflibData.h"

#include "aflibDebug.h"


/*! \brief Constructor that requires a parent.
*/
aflibAudioMixer::aflibAudioMixer(
   aflibAudio& audio) : aflibAudio(audio)
{
}


/*! \brief Constructor that does not require a parent.
*/
aflibAudioMixer::aflibAudioMixer() : aflibAudio()
{
}


/*! \brief Destructor.
*/
aflibAudioMixer::~aflibAudioMixer()
{
   //delAllMix();
}


/*! \brief Adds a mix element.

    This will add an element to be mixed. Any input specified must exist. 
	 
	 If an input was passed to the aflibAudioMixer constructor then that input is
	 number 1. Use the return value from addParent there on.
	 
	 The amplitude can be specified as any value between 1 and 100.  This 
	 corresponds to 1 and 100 percent of full scale. 
	 
	 The in_chan is the channel number of the input to use. The out_chan is the 
	 same but indicates the output channel to use. The first channel for inputs 
	 and outputs start at 0. For example to mix a stereo input to a mono output 
	 one would call addMix twice with (1, 0, 0, 100) & (1, 1, 0, 100). This would
	 take channels 0 and 1 of input 1, and output both to channel 0. 
	 
	 If inputs differ in their configuration then the output configuration will 
	 be the greater of eachs configs.  See the documentation for setInputConfig
	 for exactly how this is done.
	 
	 If the mix was added successfully then AFLIB_SUCCESS will be returned. A mix
	 with same input, in_chan, out_chan values will replace the old values.
*/

aflibStatus
aflibAudioMixer::addMix(
   int  input,
   int  in_chan,
   int  out_chan,
   int  amp)
{
	/*TODO compute output config when mixes are added.*/
   aflibStatus status = AFLIB_SUCCESS;

   // need to check if in list. If so remove.
   delMix(input, in_chan, out_chan);

   aflibMixerItem item(input, in_chan, out_chan, amp);
   _mix_item.insert(item);

   // Invalidate chain since it has changed
   setNodeProcessed(FALSE);

   return (status);
}


/*! \brief Deletes a mix element.

    This will delele a current mix element. If successful then AFLIB_SUCCESS 
    will be returned else if the item was not found then AFLIB_NOT_FOUND will 
	 be returned. 
*/
aflibStatus
aflibAudioMixer::delMix(
   int  input,
   int  in_chan,
   int  out_chan)
{
   aflibStatus status = AFLIB_NOT_FOUND;
   set<aflibMixerItem, less < aflibMixerItem> >::iterator it;

   // Loop thru all items
   for (it = _mix_item.begin(); it != _mix_item.end(); it++)
   {
      // Look for the item to delete
      if ((input == (*it).getInput()) &&
          (in_chan == (*it).getInChannel()) &&
          (out_chan == (*it).getOutChannel()))
      {
         _mix_item.erase(it);
         status = AFLIB_SUCCESS;
         break;
      }
   }

   // Invalidate chain since it has changed
   setNodeProcessed(FALSE);

   return (status);
}


/*! \brief Deletes all mix elements.
 
    This will delele all current mix elements.
*/
void
aflibAudioMixer::delAllMix()
{
   // Erase all mix items
   if (_mix_item.size() != 0)
   {
      _mix_item.erase( _mix_item.begin(), _mix_item.end());
   }                                                                                        
}


/*! \brief Returns the current number of active mix elements. 
*/
int
aflibAudioMixer::getNumOfMixs()
{
   return (_mix_item.size());                                                              
}


/*! \brief Returns information on a specific mix element.

    This will retrieve the current values for a specific mix element. One should
	 call getNumOfMixes first. Mix numbers start at 1.
*/ 
aflibStatus
aflibAudioMixer::getMix(
   int   mix_num,
   int&  input,
   int&  in_chan,
   int&  out_chan,
   int&  amp)
{
   aflibStatus status = AFLIB_NOT_FOUND;
   set<aflibMixerItem, less < aflibMixerItem> >::iterator it;
   int j;
 
   input = 0;
   in_chan = 0;
   out_chan= 0;
   amp = 0;
 
   if (mix_num <= (int)_mix_item.size())
   {
      for (it = _mix_item.begin(), j = 1; it != _mix_item.end(); it++, j++)
      {
         if (j == mix_num)
         {
            input = (*it).getInput();
            in_chan = (*it).getInChannel();
            out_chan= (*it).getOutChannel();
            amp = (*it).getAmplitude();
            status = AFLIB_SUCCESS;
            break;
         }
      }
   }

   return (status);
}


void
aflibAudioMixer::setInputConfig(const aflibConfig& cfg)
{
   /* This function overrides the aflibAudio base class function. 
		
	   It will change the output configuration based on the mixing of the inputs.
		
		It will set the output config to:
		
		- The greatest sample rate of all inputs. 
		- The greatest data size amoung all the inputs. 
		- The greatest number of channels found in the added mixes.
		- The last parents endian type.  
		
	 */

   map<int, aflibAudio *, less<int> >  parent_list = getParents();
   map<int, aflibAudio *, less<int> >::iterator it;
   set<aflibMixerItem, less < aflibMixerItem> >::iterator it_item;
	
   int  sample_rate = 0;
   int  channels = 0;
   aflib_data_endian  endian = AFLIB_ENDIAN_LITTLE;
   aflib_data_size    size = AFLIB_DATA_8U;
	
   aflibConfig out_cfg(cfg);


   // Look at every parents data configuration
   for (it = parent_list.begin(); it != parent_list.end(); it++)
   {
      const aflibConfig& parent_cfg = ((*it).second)->getOutputConfig();

      // Pick the biggest sample rate
      if (parent_cfg.getSamplesPerSecond() > sample_rate)
      {
         sample_rate = parent_cfg.getSamplesPerSecond();
      }

      // pick last endian config. It does not really matter
	if(parent_cfg.getDataEndian() != AFLIB_ENDIAN_UNDEFINED)
	      endian = parent_cfg.getDataEndian();

		if(size == parent_cfg.getSampleSize())
		{
			/* we're good to go with this size */
		}
		else
			{
      // Pick 32S, 16S, 16U, 8U, or 8S in that order
			switch(parent_cfg.getSampleSize())
			{
				case AFLIB_DATA_32S:
            	size = AFLIB_DATA_32S;
					break;
				case AFLIB_DATA_16S:
            	size = AFLIB_DATA_16S;
					break;
				case AFLIB_DATA_16U:
            	size = AFLIB_DATA_16U;
					break;
				case AFLIB_DATA_8U:
            	size = AFLIB_DATA_8U;
					break;
				case AFLIB_DATA_8S:
            	size = AFLIB_DATA_8S;
					break;
				case AFLIB_SIZE_UNDEFINED:
				default:
					break;
			}
      }
   }

   // Find number of output channels
   for (it_item = _mix_item.begin(); it_item != _mix_item.end(); it_item++)
   {
      if (channels < (*it_item).getOutChannel() + 1)
      {
         channels = (*it_item).getOutChannel() + 1;
      }
      }


   // Set and Store the output configuration
   out_cfg.setSamplesPerSecond(sample_rate);
   out_cfg.setSampleSize(size);
   out_cfg.setDataEndian(endian);
   out_cfg.setChannels(channels);
	
   setOutputConfig(out_cfg);

   // Set the input config to be the same as the output. This is what we need 
	// inputted into each input. It will force the base classes to make the 
	// conversion.
   aflibAudio::setInputConfig(cfg);
   aflibAudio::setOutputConfig(out_cfg);
}

aflibStatus
aflibAudioMixer::compute_segment(
   list<aflibData *>& data,
   long long position)
{
	/* This function is the one that does the work. It will take the list of 
	  	audio data passed in and based on the mix information stored will generate
	   an output. It will assume all data passed in is of the same length. If 
	   it is not then the output data will be the longest of the inputs. It will 
	   then pad the missing data in the other inputs with zero. The results 
	   will be passed back into the first element of the list. This function 
	   could use many optimizations.
	 */

   signed long output_length = 0;;
	int min,max, output_channels;
	
	/* TODO create better handling of the mix_buffer */ 
	double *mix_buffer[99];
	
   aflibData * out_data;
	
   map<int, aflibAudio *, less<int> > audio_list = getParents();
   list<aflibData *>::iterator data_it;
   map<int, aflibAudio *, less<int> >::iterator audio_list_it;

   if (data.size() == 0)
   {
      return(AFLIB_NO_DATA);
   }

	/* We expect one data object per parent object */
	if ( data.size() != audio_list.size() )
   {
      return(AFLIB_ERROR_UNSUPPORTED);
	}

   /* Find the largest size of all data. 
	 * Should all be equal.  But won't be if we're at the end of some files
	 * we're mixing */
   for (data_it = data.begin(); data_it != data.end() ; data_it++)
      if (output_length < (*data_it)->getLength())
         output_length = (*data_it)->getLength();

	/* TODO create better handling of the mix_buffer */ 

	output_channels = getOutputConfig().getChannels();
	for(int ch = 0; ch < output_channels; ch++){
		/* allocate mix_buffer for this channel */
		mix_buffer[ch] = new double [output_length];
		/* initialize it to zero */
		memset(mix_buffer[ch],0,output_length*sizeof(double));
	}

	/* 
	 * Recurse through each mix info 
	 * Match the parent to the data 
	 * Add to output buffer 
	 */
   for (int mix_num = 1; mix_num <= getNumOfMixs(); mix_num++)
		{
   // Mixes are ordered by output channel
   	int  input, in_chan, out_chan, amp;
      if(getMix(mix_num, input, in_chan, out_chan, amp) != AFLIB_SUCCESS) 
		{
			/* Mix info for mix_num not found so...( does it really matter if
			 * mix_num is consecutive? ) */
			continue;
		}

		/* This shouldn't happen */
		if(out_chan > output_channels)
		{
			aflib_debug("Mix output channel greater then mixer output channel");
			continue;
		}

		/* Find the data that matches this mix's input */
		/* XXX assumes data map and parents map are same size */
      for (data_it = data.begin(), audio_list_it = audio_list.begin(); 
				data_it != data.end(); data_it++, audio_list_it++)
      {
         // IF this is the input we are looking for
         if (input == (*audio_list_it).first)
         {
            double mult = (double)amp / 100.0;

            /* XXX assumes data objects and audio_list are in the same order */
				
            for (int k = 0; k < (*data_it)->getLength(); k++)
            {
               /* calculate new data to store and
               sum with existing data in temporary mix buffer */
					mix_buffer[out_chan][k] = mix_buffer[out_chan][k] + 
						(double)((*data_it)->getSample(k, in_chan) * mult);
            }
            break;
         }
      }
   }

   // Allocate output data array to return
   out_data = new aflibData(getOutputConfig(), output_length);

	/* Get min and max values for output data type to clip too */
	out_data->getMinMax(min,max);

	/* Clip the samples in the input buffer and save them to the output array */
	for(int ch = 0; ch < output_channels; ch++)
	{
		double value;
		for (int k = 0; k < out_data->getLength(); k++)
		{
			value = mix_buffer[ch][k];

			value = (value < min) ? min : ( (value > max) ? max : value);
			
			out_data->setSample(value,k,ch);
		}
		delete [] (double*) mix_buffer[ch];
	}
	
   // remove all data elements and replace with output data
   for (data_it = data.begin(); data_it != data.end() ; data_it++)
   {
      delete (*data_it);
   }
   data.erase(data.begin(), data.end());
   data.push_back(out_data);

   return (AFLIB_SUCCESS);
}

void
aflibAudioMixer::parentWasDestroyed(int parent_id)
{
   // If a parent is destroyed then walk through the mix element list and 
	// delete them.

   int num_segs;
   int i;
   int input, in_chan, out_chan, amp;
 
 
   num_segs = getNumOfMixs();
 
   // Count backwards so that num_segs changing does not affect us
   // Remove all mixes from this input
   for (i = num_segs; i != 0; i--)
   {
      getMix(i, input, in_chan, out_chan, amp);
      if (parent_id == input)
      {
         delMix(input, in_chan, out_chan);
      }
   }
}

bool
aflibAudioMixer::isDataSizeSupported(aflib_data_size size)
{
   // This overrides the virtual function in the base class.
 
   bool state = FALSE;
 
   if (size == getInputConfig().getSampleSize())
      state = TRUE;
 
   return (state);
}
 
bool
aflibAudioMixer::isEndianSupported(aflib_data_endian end)
{
   // This overrides the virtual function in the base class.
 
   bool state = FALSE;
 
   if (end == getInputConfig().getDataEndian())
      state = TRUE;
 
   return (state);
}

bool
aflibAudioMixer::isSampleRateSupported(int& rate)
{
   // This overrides the virtual function in the base class. See if the rate requested
   // is the rate that we have computed that we will be outputting.

   int value;
   bool ret_value = FALSE;

   // Get the rate of the data
   value = getOutputConfig().getSamplesPerSecond();
 
   // IF same rate then TRUE else return desired rate
   if (rate == value)
      ret_value = TRUE;
   else
      rate = value;
 
   return (ret_value);
}

