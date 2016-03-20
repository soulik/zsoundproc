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

// class for Device audio file reading and writing

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


#include <stdio.h>
#include <stdlib.h>
#include <iostream.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/soundcard.h>
#include <string.h>

#include "aflibDevFile.h"
#include "aflibData.h"
#include "aflibFileItem.h"

#define OPEN_BUFFER     0.5
#define CREATE_BUFFER   0.2

#define MODULE_NAME "aflibDevFile"

extern "C"
{
   aflibFile *
   getAFileObject() { return ((aflibFile *)new aflibDevFile()); }

	void
	query(list<aflibFileItem*>& support_list)
	{

		aflibFileItem* item;
		item	= new aflibFileItem();

		item->setFormat("DEVICE");
		item->setDescription("OSS Device");
		item->setName(MODULE_NAME);
		support_list.push_back(item);
	
	}
}


aflibDevFile::aflibDevFile()
{
   _fd_int = -1;
   _size = AFLIB_SIZE_UNDEFINED;
}

aflibDevFile::~aflibDevFile()
{
   if (_fd_int != -1)
   {
      close(_fd_int);
   }
}

aflibStatus
aflibDevFile::afopen(
   const char * file,
   aflibConfig* cfg)
{
   aflibStatus status = AFLIB_SUCCESS;
   aflibConfig  input_cfg;

   _create_mode = FALSE;
   _file = file;
   _snd_buffer = OPEN_BUFFER;

   if ((_fd_int = ::open(file, O_RDONLY, 0)) == -1)
   {
      cout << "Unable to open device" << endl;
      return (AFLIB_ERROR_OPEN);
   }

   // Store defaults if user has specified
   if (cfg != NULL)
   {
      input_cfg = *cfg;
   }

   if (cfg != NULL)
      {
      if (cfg->getBitsPerSample() == 8)
      {
         _snd_format = AFMT_U8;
      }
      else if (cfg->getBitsPerSample() == 16)
      {
         _snd_format = AFMT_S16_LE;
      }
      else
      {
         // Some unknown setting
         _snd_format = AFMT_U8;
      }
   }
   else
   {
      _snd_format = AFMT_S16_LE;
   }
   input_cfg.setDataOrientation(AFLIB_INTERLEAVE);
   input_cfg.setDataEndian(AFLIB_ENDIAN_LITTLE);

   if (_snd_format == AFMT_S16_LE)
   {
      input_cfg.setSampleSize(AFLIB_DATA_16S);
      _size = AFLIB_DATA_16S;
   }
   else
   {
      input_cfg.setSampleSize(AFLIB_DATA_8U);
      _size = AFLIB_DATA_8U;
   }

   if (cfg != NULL)
   {
      input_cfg.setChannels(cfg->getChannels());
   }
   else
   {
      input_cfg.setChannels(1);
   }

   _snd_stereo = input_cfg.getChannels() - 1;

   if (cfg != NULL)
   {
      _snd_speed = cfg->getSamplesPerSecond();
   }
   else
   {
      _snd_speed = 44100;
   }
   input_cfg.setSamplesPerSecond(_snd_speed);

   // Set the input and output audio configuration data
   setInputConfig(input_cfg);
   setOutputConfig(input_cfg);

   programDevice();

   return (status);
}

aflibStatus
aflibDevFile::afcreate(
   const char * file,
   const aflibConfig& config)
{
   aflibStatus status = AFLIB_SUCCESS;
   aflibConfig output_cfg(config);


   _create_mode = TRUE;
   _file = file;
   _snd_buffer = CREATE_BUFFER;

   if ((_fd_int = ::open(file, O_WRONLY, 0)) == -1)
   {
      cout << "Unable to open device" << endl;
      return (AFLIB_ERROR_OPEN);
   }

   if ((config.getBitsPerSample() == 16) && (config.getDataOrientation() == AFLIB_INTERLEAVE))
   {
      _snd_format = AFMT_S16_LE;
      _size = AFLIB_DATA_16S;
   }
   else if (config.getBitsPerSample() == 8)
   {
      _snd_format = AFMT_U8;
      _size = AFLIB_DATA_8U;
   }
   else
   {
      cerr << "Unsupported sample format" << endl;
      close (_fd_int);
      _fd_int = -1;
      return (AFLIB_ERROR_UNSUPPORTED);
      }

   output_cfg.setSampleSize(_size);

   _snd_stereo = config.getChannels() - 1;

   _snd_speed = config.getSamplesPerSecond();   

   // Set input and output audio configuration
   setInputConfig(config);
   setOutputConfig(output_cfg);

   programDevice();

   return(status);
}

 
void
aflibDevFile::programDevice()
{
   if (_fd_int != -1)
      close(_fd_int);

   if (_create_mode == TRUE)
   {
      if ((_fd_int = ::open(_file.c_str(), O_WRONLY, 0)) == -1)
      {
         cout << "Unable to open device" << endl;
         return;
      }
   }
   else
   {
      if ((_fd_int = ::open(_file.c_str(), O_RDONLY, 0)) == -1)
      {
         cout << "Unable to open device" << endl;
         return;
      }
   }

   // We need to change the buffering in driver so that we get somewhat real time
   // performance. This provides 2 buffers each. The size of each buffer is determined
   // by the input_cfg data and the factor value.
   int arg = 0x00020000 + createBuffer(getOutputConfig(), _snd_buffer);
   if (ioctl(_fd_int, SNDCTL_DSP_SETFRAGMENT, &arg) == -1)
   {
      cout << "Unable to set buffer sizes" << endl;
   }

   if (ioctl(_fd_int, SNDCTL_DSP_SETFMT, &_snd_format) == -1)
   {
      cerr << "Unable to program format in device" << endl;
      close (_fd_int);
      _fd_int = -1;
      return;
   }
 
   if (ioctl(_fd_int, SNDCTL_DSP_STEREO, &_snd_stereo) == -1)
   {
      cerr << "Unable to program channels in device" << endl;
      close (_fd_int);
      _fd_int = -1;
      return;
   }

   if (ioctl(_fd_int, SNDCTL_DSP_SPEED, &_snd_speed) == -1)
   {
      cerr << "Unable to program speed in device" << endl;
      close (_fd_int);
      _fd_int = -1;
      return;
   }
}


aflibStatus
aflibDevFile::afread(
   aflibData& data,
   long long position )
{
   void * p_data;
   long   total_length;
   aflibStatus  status = AFLIB_SUCCESS;

   data.setConfig(getInputConfig());
   total_length = data.getTotalLength();
   p_data = data.getDataPointer();

   read(_fd_int, p_data, total_length);

   return (status);
}

aflibStatus
aflibDevFile::afwrite(
   aflibData& data,
   long long position )
{
   void * p_data;
   long   total_length;
   aflibStatus  status = AFLIB_SUCCESS;


   if (data.getLength() == data.getOrigLength())
   {
      total_length = data.getTotalLength();
   }
   else
   {
      total_length = (long)(data.getTotalLength() *
         ((double)data.getLength() / (double)data.getOrigLength()));
   }

   p_data = data.getDataPointer();

   if (write(_fd_int, p_data, total_length) == -1)
   {
      perror("");
      cerr << "Failed to write to audio device " << endl;
   }
   return (status);
}


/*! \brief Processes unique information for the Linux sound device.

   This will process any unique information for the Linux audio device. It currently
   supports one item. It is AFLIB_DEV_ITEM_BUFFER. It will expect a value that is of
   type double. This will create a buffer in the hardware, if it is supported, of
   the length specified in seconds. 
*/
bool
aflibDevFile::setItem(
   const char * item,
   const void * value)
{
   bool state = FALSE;

   // IF user wants to set buffer length
   if (strcmp(item, AFLIB_DEV_ITEM_BUFFER) == 0)
   {
      _snd_buffer = *((double *)value);
      programDevice();
      state = TRUE;
   }

   return (state);
}


bool
aflibDevFile::isDataSizeSupported(aflib_data_size size)
{
   // If handle not yet allocated then indicate sizes supported
   if (_fd_int == -1)
      return (size == AFLIB_DATA_8U || size == AFLIB_DATA_16S);

   return (size == _size);
}
 
bool
aflibDevFile::isEndianSupported(aflib_data_endian end)
{
   // Linux device only supports little endian
   return (end == AFLIB_ENDIAN_LITTLE);
}

bool
aflibDevFile::isSampleRateSupported(int& rate)
{
   int value;
   bool ret_value = FALSE;


   // If handle not yet allocated then indicate any sample rate is supported
   if (_fd_int == -1)
      return (TRUE);

   // Get the rate of the data
   value = getOutputConfig().getSamplesPerSecond();

   // IF same rate then TRUE else return desired rate
   if (rate == value)
      ret_value = TRUE;
   else
      rate = value;

   return (ret_value);
}


int
aflibDevFile::createBuffer(
   const aflibConfig& cfg,
   double  factor)
{
   // This calculates the buffer size that the audio device should use. It will be
   // based on the audio data type passed in as cfg and a multiplication factor in
   // seconds to buffer. Thus to have a 0.1 second buffer factor should be 0.1.

   int bytes_per_second;
   int bytes_counter;


   bytes_per_second = (int)((cfg.getBitsPerSample() / 8 * cfg.getChannels() *
           cfg.getSamplesPerSecond()) * factor);
   bytes_counter = 0;
   while (bytes_per_second != 0)
   {
      bytes_per_second = bytes_per_second >> 1;
      bytes_counter++;
   }

   return (bytes_counter);
}



