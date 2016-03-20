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
#include <stdlib.h>
#include <iostream>

using std::cerr;
using std::endl;

#include "aflibAudio.h"
#include "aflibAudioMixer.h"
#include "aflibData.h"
#include "aflibDebug.h"
#include "aflibAudioSampleRateCvt.h"

int aflibAudio::_level = 0;


/*! \brief Constructor.

     This constructor is for derived classes that have no parent audio object. 
*/
aflibAudio::aflibAudio() : aflibChain(), aflibMemCache()
{
   // This node is enabled by default
   _enable = TRUE;

   // All conversions enabled by default
   _enable_data_size = TRUE;
   _enable_endian = TRUE;
   _enable_channels = TRUE;
   _enable_sample_rate = TRUE;

   _cvt = NULL;
   _mix = NULL;
}

/*! \brief Constructor.

     This constructor is for derived classes that have a parent audio object. 
*/
aflibAudio::aflibAudio(
   aflibAudio& audio) : aflibChain(audio), aflibMemCache()
{
   // This node is enabled by default
   _enable = TRUE;

   // All conversions enabled by default
   _enable_data_size = TRUE;
   _enable_endian = TRUE;
   _enable_channels = TRUE;
   _enable_sample_rate = TRUE;

   _cvt = NULL;
   _mix = NULL;

   setInputConfig(audio.getInputConfig());
}


/*! \brief Destructor.
*/
aflibAudio::~aflibAudio()
{
   delete _cvt;
   delete _mix;
}


/*! \brief Enables or disables an object in a chain.

    This allows this node to be enabled or disabled. This assists in removing
    a node from a chain without having to destruct it. This is useful to enable
    an application to toggle a filter in and out for example.
*/
void
aflibAudio::enable(bool enable)
{
   // IF Changing state of chain then reprocess chain
   if (enable != _enable)
      setNodeProcessed(FALSE);

   _enable = enable;
 
}


/*! \brief Gets current state of object.
 
    This allows one to determine if an object is Enabled or Disabled. TRUE means
    that the object is currently enabled.
*/
bool
aflibAudio::getEnable() const
{
   return(_enable);
}


/*! \brief Enables or disables data size conversion.
 
    This allows the automatic data size conversion feature to be enabled or disabled.
    This is already enabled by default. If one does not want a node to perform a 
    data size conversion then this can be set on a node to disable it.
*/
void
aflibAudio::enableDataSizeConversion(bool enable)
{
   _enable_data_size = enable;
}


/*! \brief Enables or disables endian conversion.
 
    This allows the endian data conversion feature to be enabled or disabled.
    This is already enabled by default. If one does not want a node to perform an 
    endian conversion then this can be set on a node to disable it.
*/
void
aflibAudio::enableEndianConversion(bool enable)
{
   _enable_endian = enable;
}


/*! \brief Enables or disables sample rate conversion.
 
    This allows the automatic sample rate conversion feature to be enabled or disabled.
    This is already enabled by default. If one does not want a node to perform a 
    sample rate conversion then this can be set on a node to disable it.
*/
void
aflibAudio::enableSampleRateConversion(bool enable)
{
   _enable_sample_rate = enable;
}


/*! \brief Enables or disables number of channels conversion.
 
    This allows the automatic channel conversion feature to be enabled or disabled.
    This is already enabled by default. If one does not want a node to perform a 
    mixing operation to convert the number of channels then this can be set on
    a node to disable it.
*/
void
aflibAudio::enableChannelsConversion(bool enable)
{
   _enable_channels = enable;
}


/*! \brief Sets the input audio data configuration of an object.
 
    This is a virtual function that derived classes can override if needed.
    It allows the caller to set the configuration of the audio data of an
    object. By default it saves the output audio configuration to be the
    same as the input data configuration that is passed in. This should be
    sufficient for most derived classes. For those classes that will have
    a change from the output to input config mapping then this function
    should be overriden and the output config processed and saved.
*/
void
aflibAudio::setInputConfig(const aflibConfig& cfg)
{
   _cfg_input = cfg;

   // As a default create a 1-1 config mapping between input and output
   setOutputConfig(_cfg_input);
}


/*! \brief Returns the input audio data configuration of an object.
*/ 
const aflibConfig&
aflibAudio::getInputConfig() const
{
   return (_cfg_input);
}

/*! \brief Sets the output audio data configuration of an object.
 
    This is a virtual function that derived classes can override if needed.
    Most derived classes will not need to override this function but can 
    simply call it to store the output audio configuration. It is virtual
    so that it can be overriden by the aflibAudioFile class which needs to
    implement its own.
*/
void
aflibAudio::setOutputConfig(const aflibConfig& cfg)
{
   _cfg_output = cfg;
}


/*! \brief Returns the output audio data configuration of an object.
*/ 
const aflibConfig&
aflibAudio::getOutputConfig() const
{
   return (_cfg_output);
}


/*! \brief Main processing function for pulling data thru an audio chain.

    This is the main processing function for pulling data thru an audio chain. It can
    be called from any aflibAudio derived object. Users should call the process function
    at the end of a chain. This function will then call each process function in an
    audio chain until the start of the chain is reached. When the start of the chain is
    reached then an aflibData object of the proper size will be allocated and each
    aflibAudio derived objects compute_segment function will be called to process the
    data until the end of the chain is reached. This implementation will handle
    multiple parents automatically. It will request data from the same position and
    size from each parent of an object. It data of a different size or position is
    needed from each parent then this will need to be overriden in the derived
    class. This is not recommended.
    The first parameter will return a status to the caller. If everything worked OK then
    AFLIB_SUCCESS will be returned. The second is a position parameter. This indicates at
    which position in an audio source to read data from. This is a suggestion. It will be
    ignored if reading from an audio device or from an audio format that does not
    support random access, like mp3. The third parameter is the number of samples to
    pull thru the chain. If 0 is passed then the library will pick the size. This is a
    suggestion. It will return the actual number of samples used when pulling data thru
    the chain. If the caller passes FALSE to free_memory then the pointer to the
    aflibData class containing the data will be returned. It is then the
    responsibility of the calling app to free this memory using delete. If no parameter
    is passed then it will not be returned by default. Instead NULL will be returned.
*/ 
aflibData *
aflibAudio::process(
   aflibStatus& ret_status,
   long long position,
   int& num_samples,
   bool free_memory) 
{
   int  list_size = 0;
   aflibData * data = NULL;
   long length;
   long long orig_position = position;
   int orig_num_samples = num_samples;
   aflibStatus save_status;
   list<aflibData *> d_list;
   list<aflibData *>::iterator it_data ;
   map<int, aflibAudio *, less<int> > audio_list;
   map<int, aflibAudio *, less<int> >::iterator it;


   // Set default return status and level indicator
   ret_status = AFLIB_SUCCESS;
   incrementLevel();

   // Check to see if chain has been preprocessed if at start of chain
   examineChain();

   audio_list = this->getParents();
   list_size = audio_list.size();

   if (list_size > 1)
   {
      // disable caching if more than one parent
      setCacheEnable(FALSE);
   }

   // IF no parents then cache are input.
   if (list_size == 0)
      {
      // IF node is not enabled then skip processing
      if (_enable == TRUE)
      {
         if (num_samples == 0)
            num_samples = 4096;

         data = new aflibData(num_samples);

         // Lets look in the cache and see if we have data
         if (getCacheEnable() == TRUE)
            lookupData(position, num_samples);

         d_list.push_back(data);

         // IF not all the data is in the cache
         if (num_samples != 0)
         {
            ret_status = compute_segment(d_list, position);
         }
         // ELSE all data is in the cache
         else
      {
           // Since we will get all data from the cache we need to get the config
           // data from the object.
           data->setConfig(getOutputConfig());
         }

         if (getCacheEnable() == TRUE)
         {
            // Cache audio data and Fill in data object with data from cache
            fillDataFromCache(*data,
               position, num_samples, orig_position, orig_num_samples);
         }
            }
            }
   // ELSE call parent and let it process it first then process result
   else
   {
      // Lets look in the cache and see if we have data
      if (getCacheEnable() == TRUE)
         lookupData(position, num_samples);

      // IF all data is in cache then we are done
      if (num_samples != 0)
      {
         // set initial status to success
         save_status = AFLIB_SUCCESS;
         for (it = audio_list.begin(); it != audio_list.end(); it++)
           {
            d_list.push_back(((aflibAudio*)( (*it).second))->process(
                   ret_status, position, num_samples, FALSE));

            // IF there is a error returned then set overall error state to error
            if (save_status == AFLIB_SUCCESS)
               save_status = ret_status;
      }
         ret_status = save_status;
   }

      if (ret_status == AFLIB_SUCCESS)
      {
         // ptr can be NULL if parent was not enabled
         if ((*d_list.begin()) == NULL)
         {
            d_list.erase(d_list.begin());
            if (num_samples == 0)
               d_list.push_back(new aflibData(4096));
            else
               d_list.push_back(new aflibData(num_samples));
   }

         // IF data was entirely in cache then data list will be empty
         // TBD what about of num_samples specified was 0.
         if (d_list.size() == 0)
            d_list.push_back(new aflibData(getInputConfig(), orig_num_samples));

         if (_enable == TRUE)
   {
      for (it_data = d_list.begin(); it_data != d_list.end(); it_data++)
        {
               // Convert data to correct format
               convertData(**it_data);
            }

            if (getCacheEnable() == TRUE)
            {
               // Cache audio data and Fill in data object with data from cache
               fillDataFromCache(**(d_list.begin()),
                  position, num_samples, orig_position, orig_num_samples);
   }

            ret_status = compute_segment(d_list, position);
         }
      }
    }

   // Set num_samples with correct value
   if (*(d_list.begin()))
   {
      (*(d_list.begin()))->getLength(length);
      num_samples = (int)length;
   }

   // IF caller does not want memory returned then free
   if (free_memory == TRUE)
      {
      for (it_data = d_list.begin(); it_data != d_list.end(); it_data++)
        delete (*it_data);
      d_list.erase(d_list.begin(), d_list.begin());
      d_list.push_back(NULL);
   }

   decrementLevel();
   return (*(d_list.begin()));
}


void
aflibAudio::examineChain()
{
   // This function will examine the chain if necessary and perform any preprocessing
   // that is necessary


   if (_level == 1)
   {
      // IF chain has not been preprocessed
      if (FALSE == checkChain(this))
      {
         preprocessChain(this, NULL);
      }
   }
}


bool
aflibAudio::checkChain(const aflibAudio * audio) const
{
   // This will check the chain that is setup to see if it has been preprocessed
   // since the last change to the chain. This function will handle multiple parents
   // for a derived object. This function is recursive.

   map<int, aflibAudio *, less<int> > audio_list = audio->getParents();
   map<int, aflibAudio *, less<int> >::iterator it;
   bool ret_value = TRUE;

   // Loop thru all parents of this object
   for (it = audio_list.begin(); it != audio_list.end(); it++)
   {
      // Check down the chain of this object
      ret_value = checkChain((*it).second);

      // If chain is not setup then we are done
      if (ret_value == FALSE)
         break;
   }

   // IF nodes are processed AND this node is enabled then check this node
   if ((ret_value == TRUE) && (audio->getEnable()))
   {
      ret_value = audio->getNodeProcessed();
   }

   return (ret_value);
}

void
aflibAudio::preprocessChain(
   aflibAudio * audio,
   aflibAudio * child)
{
   // This function will preprocess a chain by starting at the beginning of the chain
   // going down the chain and storing the output configuration of the parent to the
   // input configuration of the child. If child is null then we are at the end of
   // the chain. This function will handle multiple parents. This function is recursive.

   map<int, aflibAudio *, less<int> > audio_list = audio->getParents();
   map<int, aflibAudio *, less<int> >::iterator it;

   // Loop thru all parents of this object to get to the top of the chain 
	
   for (it = audio_list.begin(); it != audio_list.end(); it++)
   {
      preprocessChain((*it).second, audio);
   }

   // IF there is a child
   if (child != NULL)
   {
      if (child->getEnable() == TRUE)
      {
         // set the input of the child to the output of the parent
         child->setInputConfig(audio->getOutputConfig());

         // IF node is enabled
         if (audio->getEnable() == TRUE)
         {
            // see if child needs to have input sample rate converted
	/* TODO might be better to use input no instead of pointer.
	 * or else consolidate both these functions into one */
            audio = child->convertChannels(*audio);
            audio = child->convertSampleRate(*audio);
         }
      }
      else
      {
         child->aflibAudio::setInputConfig(audio->getOutputConfig());
      }
   }

   // Set this node to have been preprocessed
   audio->setNodeProcessed(TRUE);
}

aflibAudio*
aflibAudio::convertSampleRate(aflibAudio& parent)
{

// Called after parent sets input_cfg to it's output_cfg

   aflibConfig config(getInputConfig());
   int   sample_rate = config.getSamplesPerSecond();
   int   input_sample_rate = sample_rate;
   double  factor;


   // IF sample rate conversion is enabled for child 
   if (_enable_sample_rate == TRUE)
   {
      // If sample rate is not supported by child
      if (!isSampleRateSupported(sample_rate))
      {
			// If sample_rate is overridden by user then above should
			// return overridden rate if it was supported
         factor = (double)sample_rate/(double)input_sample_rate;
	/* TODO allow for the fact a child can have more then one parent */
         if (_cvt)
            _cvt->setFactor( factor );
         else
            _cvt = new aflibAudioSampleRateCvt(parent, factor);
		aflib_debug("aflibAudio: converting by factor %0.2f", factor);

         // Remove old parent and install new parent
         replaceParent(parent, *_cvt);

         // Setup the config data for this new node
         _cvt->setInputConfig(parent.getOutputConfig());

			// Set child sample rate to new sample_rate
			config.setSamplesPerSecond(sample_rate);			

         // Set this new node to have already been processed
         _cvt->setNodeProcessed(TRUE);
	return _cvt;
      }
	}
	return &parent;
}

aflibAudio*
aflibAudio::convertChannels(aflibAudio& parent)
{

	// Called after parent sets input_cfg to it's output_cfg

	aflibConfig config(getInputConfig());
	int   channels = config.getChannels();
	int   input_channels = channels;


	// IF channels conversion is enabled for child 
	if (_enable_channels == TRUE)
	{
		// If sample rate is not supported by child
		if (!isChannelsSupported(channels))
		{
			// If sample_rate is overridden by user then above should
			// return overridden rate if it was supported
			/* TODO allow for the fact a child can have more then one parent */
			if (!_mix)
				_mix = new aflibAudioMixer(parent);

			_mix->delAllMix();

			/* TODO more intellegence about types of mixes to add */
			if(input_channels == 2)
			{
				/* left to mono - right to mono */
				_mix->addMix(1,1,0, 100);
				_mix->addMix(1,0,0, 100);
			}	
			else if(input_channels == 1)
			{
				/* mono to left - mono to right */
				_mix->addMix(1,0,0, 100);
				_mix->addMix(1,0,1, 100);
			} 
			else 
			{	
				/* route every input channel to every output channel */
				for(int j = 0; j < channels; j++){
					for(int i = 0;i < input_channels; i++){
						_mix->addMix(1,i,j,100);
					}
				}
			}

			// Remove old parent and install new parent
			replaceParent(parent, *_mix);

			// Setup the config data for this new node
			_mix->setInputConfig(parent.getOutputConfig());

			// Set child sample rate to new sample_rate
			config.setChannels(channels);			
      
			// Set this new node to have already been processed
			_mix->setNodeProcessed(TRUE);
			return _mix;
		}
	}
	return &parent;
}

void
aflibAudio::convertData(aflibData& data)
{
   const aflibConfig& config = data.getConfig();


   // IF data size conversion is enabled
   if (_enable_data_size == TRUE)
   {
      // If data size is not supported
      if (!isDataSizeSupported(config.getSampleSize()))
      {
         switch (config.getSampleSize())
         {
            case AFLIB_DATA_8S:
               // Find an acceptable data size to convert to
               if (isDataSizeSupported(AFLIB_DATA_8U))
                  data.convertToSize(AFLIB_DATA_8U);
               else if (isDataSizeSupported(AFLIB_DATA_16S))
                  data.convertToSize(AFLIB_DATA_16S);
               else if (isDataSizeSupported(AFLIB_DATA_16U))
                  data.convertToSize(AFLIB_DATA_16U);
               else if (isDataSizeSupported(AFLIB_DATA_32S))
                  data.convertToSize(AFLIB_DATA_32S);
               else
                  cerr << "No allowed data size to convert to" << endl;
            break;

            case AFLIB_DATA_8U:
               // Find an acceptable data size to convert to
               if (isDataSizeSupported(AFLIB_DATA_8S))
                  data.convertToSize(AFLIB_DATA_8S);
               else if (isDataSizeSupported(AFLIB_DATA_16U))
                  data.convertToSize(AFLIB_DATA_16U);
               else if (isDataSizeSupported(AFLIB_DATA_16S))
                  data.convertToSize(AFLIB_DATA_16S);
               else if (isDataSizeSupported(AFLIB_DATA_32S))
                  data.convertToSize(AFLIB_DATA_32S);
               else
                  cerr << "No allowed data size to convert to" << endl;
             break;

            case AFLIB_DATA_16S:
               // Find an acceptable data size to convert to
               if (isDataSizeSupported(AFLIB_DATA_16U))
                  data.convertToSize(AFLIB_DATA_16U);
               else if (isDataSizeSupported(AFLIB_DATA_8S))
                  data.convertToSize(AFLIB_DATA_8S);
               else if (isDataSizeSupported(AFLIB_DATA_8U))
                  data.convertToSize(AFLIB_DATA_8U);
               else if (isDataSizeSupported(AFLIB_DATA_32S))
                  data.convertToSize(AFLIB_DATA_32S);
               else
                  cerr << "No allowed data size to convert to" << endl;
             break;

            case AFLIB_DATA_16U:
               // Find an acceptable data size to convert to
               if (isDataSizeSupported(AFLIB_DATA_16S))
                  data.convertToSize(AFLIB_DATA_16S);
               else if (isDataSizeSupported(AFLIB_DATA_8U))
                  data.convertToSize(AFLIB_DATA_8U);
               else if (isDataSizeSupported(AFLIB_DATA_8S))
                  data.convertToSize(AFLIB_DATA_8S);
               else if (isDataSizeSupported(AFLIB_DATA_32S))
                  data.convertToSize(AFLIB_DATA_32S);
               else
                  cerr << "No allowed data size to convert to" << endl;
             break;

            case AFLIB_DATA_32S:
               // Find an acceptable data size to convert to
               if (isDataSizeSupported(AFLIB_DATA_16S))
                  data.convertToSize(AFLIB_DATA_16S);
               else if (isDataSizeSupported(AFLIB_DATA_16U))
                  data.convertToSize(AFLIB_DATA_16U);
               else if (isDataSizeSupported(AFLIB_DATA_8S))
                  data.convertToSize(AFLIB_DATA_8S);
               else if (isDataSizeSupported(AFLIB_DATA_8U))
                  data.convertToSize(AFLIB_DATA_8U);
               else
                  cerr << "No allowed data size to convert to" << endl;
             break;

            default:
            break;
         }
      }
   }

   // IF endian conversion is enabled
   if (_enable_endian == TRUE)
   {
      // If endian is supported
      if (!isEndianSupported(config.getDataEndian()))
      {
         if (config.getDataEndian() == AFLIB_ENDIAN_LITTLE)
            data.convertToEndian(AFLIB_ENDIAN_BIG);
         else
            data.convertToEndian(AFLIB_ENDIAN_LITTLE);
      }
   }
}

bool
aflibAudio::isDataSizeSupported(aflib_data_size size)
{
   // This is the base classes implmentation of the virtual function that
   // determines what data sizes are supported by a derived class. The base
   // classes implementation will return that all data sizes are supported. If
   // this is true for your derived class then this function does not need
   // to be overriden. If it is not then you will need to override this
   // function. A derived class should return TRUE under some condition.
 
   return (TRUE);
}

bool
aflibAudio::isEndianSupported(aflib_data_endian end)
{
   // This is the base classes implmentation of the virtual function that
   // determines what endian is supported by a derived class. The base
   // classes implementation will return that all endians are support. If
   // this is true for your derived class then this function does not need
   // to be overriden. If it is not then you will need to override this
   // function. A derived class should return TRUE under some condition.
 
   return (TRUE);
}

bool
aflibAudio::isSampleRateSupported(int& rate)
{
   // This is the base classes implmentation of the virtual function that
   // determines what sample rates are supported by a derived class. The base
   // classes implementation will return that all rates are supported. If
   // this is true for your derived class then this function does not need
   // to be overriden. If it is not then you will need to override this
   // function. If you return FALSE then you should set rate to the 
   // recommended sample rate that should be used. 
 
   return (TRUE);
}


bool
aflibAudio::isChannelsSupported(int& channels)
{
   // This is the base classes implmentation of the virtual function that
   // determines how many channels are supported by a derived class. The base
   // classes implementation will return true which states all numbers of 
   // channels are supported. If
   // this is true for your derived class then this function does not need
   // to be overriden. If it is not then you will need to override this
   // function. If you return FALSE then you should set channels to the 
   // recommended channels that should be used. 
 
   return (TRUE);
}


