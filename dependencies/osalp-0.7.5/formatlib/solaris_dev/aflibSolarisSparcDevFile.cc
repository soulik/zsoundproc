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
#include <sys/audio.h>
#include <sys/mixer.h>
#include <stropts.h>
#include <string.h>

#include "aflibSolarisSparcDevFile.h"
#include "aflibBufferDev.h"
#include "aflibData.h"

#include "aflibFileItem.h"

#define MODULE_NAME "aflibDevFile"

#define RECORD_BLOCK    512


extern "C"
{
   aflibFile *
   getAFileObject() { return ((aflibFile *)new aflibSolarisSparcDevFile()); }

	void
	query(list<aflibFileItem*>& support_list)
	{

		aflibFileItem* item;
		item	= new aflibFileItem();

		item->setFormat("DEVICE");
		item->setDescription("Sparc Device");
		item->setName(MODULE_NAME);
		support_list.push_back(item);
	
	}
}


aflibSolarisSparcDevFile::aflibSolarisSparcDevFile()
{
   _fd_int = -1;
   _size = AFLIB_SIZE_UNDEFINED;
   _sample_array = NULL;
   _buffer = new aflibBufferDev();
}

aflibSolarisSparcDevFile::~aflibSolarisSparcDevFile()
{
   if (_fd_int != -1)
   {
      // throw away all buffered data else close will hang until buffer is drained
      // We dont do this for write since all queued data will be lost.
      ioctl(_fd_int, I_FLUSH, FLUSHR);
      close(_fd_int);
   }

   if (_sample_array != NULL)
      free(_sample_array);

   if (_buffer != NULL)
      delete _buffer;
}

aflibStatus
aflibSolarisSparcDevFile::afopen(
   const char * file,
   aflibConfig* cfg)
{
   aflibStatus status = AFLIB_SUCCESS;
   aflibConfig  input_cfg;
   am_sample_rates_t  *sr;
   unsigned int  num;

   _create_mode = FALSE;
   _file = file;

   if ((_fd_int = ::open(file, O_RDONLY, 0)) == -1)
   {
      cout << "Unable to open device" << endl;
      return (AFLIB_ERROR_OPEN);
   }

   for (num = 20; num < 10000; num += 2)
   {
      _sample_array = malloc(AUDIO_MIXER_SAMP_RATES_STRUCT_SIZE(num));
      sr = (am_sample_rates_t *)_sample_array;
      sr->num_samp_rates = num;
      sr->type = AUDIO_RECORD;
      ioctl(_fd_int, AUDIO_MIXER_GET_SAMPLE_RATES, sr);
      if (sr->num_samp_rates <= num)
      {
         break;
      }
      free(sr);
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
         _size = AFLIB_DATA_8S;
      }
      else if (cfg->getBitsPerSample() == 16)
      {
         _size = AFLIB_DATA_16S;
      }
      else
      {
         // Some unknown setting
         _size = AFLIB_DATA_8S;
      }
   }
   else
   {
      _size = AFLIB_DATA_16S;
   }
   input_cfg.setSampleSize(_size);
   input_cfg.setDataOrientation(AFLIB_INTERLEAVE);
   input_cfg.setDataEndian(AFLIB_ENDIAN_BIG);

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
aflibSolarisSparcDevFile::afcreate(
   const char * file,
   const aflibConfig& config)
{
   aflibStatus status = AFLIB_SUCCESS;
   aflibConfig output_cfg(config);
   am_sample_rates_t  *sr;
   unsigned int  num;


   _create_mode = TRUE;
   _file = file;

   if ((_fd_int = ::open(file, O_WRONLY, 0)) == -1)
   {
      cout << "Unable to open device" << endl;
      return (AFLIB_ERROR_OPEN);
   }

   for (num = 20; num < 10000; num += 2)
   {
      _sample_array = malloc(AUDIO_MIXER_SAMP_RATES_STRUCT_SIZE(num));
      sr = (am_sample_rates_t *)_sample_array;
      sr->num_samp_rates = num;
      sr->type = AUDIO_PLAY;
      ioctl(_fd_int, AUDIO_MIXER_GET_SAMPLE_RATES, sr);
      if (sr->num_samp_rates <= num)
      {
         break;
      }
      free(sr);
   }

   if ((config.getBitsPerSample() == 16) && (config.getDataOrientation() == AFLIB_INTERLEAVE))
   {
      _size = AFLIB_DATA_16S;
   }
   else if (config.getBitsPerSample() == 8)
   {
      _size = AFLIB_DATA_8S;
   }
   else
   {
      cerr << "Unsupported sample format" << endl;
      close (_fd_int);
      _fd_int = -1;
      return (AFLIB_ERROR_UNSUPPORTED);
   }

   output_cfg.setSampleSize(_size);
   output_cfg.setDataEndian(AFLIB_ENDIAN_BIG);

   _snd_stereo = config.getChannels() - 1;

   // Find a supported sample rate to use
   _snd_speed = sr->samp_rates[sr->num_samp_rates-1];
   for (num = 0; num < sr->num_samp_rates; num++)
   {
      if (config.getSamplesPerSecond() <= (signed int)sr->samp_rates[num])
      {
         _snd_speed = sr->samp_rates[num];
         break;
      }
   }

   output_cfg.setSamplesPerSecond(_snd_speed);

   // Set input and output audio configuration
   setInputConfig(config);
   setOutputConfig(output_cfg);

   programDevice();

   return(status);
}


void
aflibSolarisSparcDevFile::programDevice()
{
   audio_info_t   info;


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

   // Initialize the audio info
   AUDIO_INITINFO(&info);

   if (_create_mode == TRUE)
   {
      if (_size == AFLIB_DATA_16S)
      {
         info.play.encoding = AUDIO_ENCODING_LINEAR;
         info.play.precision = 16;
      }
      else if (_size == AFLIB_DATA_8S)
      {
         info.play.encoding = AUDIO_ENCODING_LINEAR;
         info.play.precision = 8;
      }
      else
         cout << "Error data size illegal for Solaris Audio device " << endl;

      info.play.channels = _snd_stereo + 1;
      info.play.sample_rate = _snd_speed;
   }
   else
   {
      if (_size == AFLIB_DATA_16S)
      {
         info.record.encoding = AUDIO_ENCODING_LINEAR;
         info.record.precision = 16;
      }
      else if (_size == AFLIB_DATA_8S)
      {
         info.record.encoding = AUDIO_ENCODING_LINEAR;
         info.record.precision = 8;
      }
      else
         cout << "Error data size illegal for Solaris Audio device " << endl;

      info.record.channels = _snd_stereo + 1;
      info.record.sample_rate = _snd_speed;

      _buffer->init(500000);
      info.record.buffer_size = RECORD_BLOCK;
   }

   if (ioctl(_fd_int, AUDIO_SETINFO, &info) == -1)
   {
      cerr << "Unable to program Solaris audio device" << endl;
      close (_fd_int);
      _fd_int = -1;
      return;
   }
}


aflibStatus
aflibSolarisSparcDevFile::afread(
   aflibData& data,
   long long position )
{
   void * p_data;
   long   total_length;
   aflibStatus  status = AFLIB_SUCCESS;
   int arg = 0;
   int mess;
   unsigned char temp_buf[RECORD_BLOCK];


   data.setConfig(getInputConfig());
   total_length = data.getTotalLength();
   p_data = data.getDataPointer();

   // IF not enough buffer space then lower length of data
   if (total_length > _buffer->getBufferSize() - 1)
   {
      data.adjustTotalLength(_buffer->getBufferSize() - 1);
      total_length = data.getTotalAdjustLength();
   }

   // Wait for a buffer to have data
   while (_buffer->bytes_available() < total_length)
   {
      mess = ioctl(_fd_int, I_NREAD, &arg);
      if (mess != 0)
      {
         ssize_t size = read(_fd_int, temp_buf, RECORD_BLOCK);
         if (size == RECORD_BLOCK)
         {
            _buffer->write(temp_buf, RECORD_BLOCK);
         }
         else
         {
            cerr << "ERROR buffer write " << endl;
         }
      }
      else
      {
         usleep(1);
      }
   }

   _buffer->read((unsigned char *)p_data, (int)total_length);

   return (status);
}

aflibStatus
aflibSolarisSparcDevFile::afwrite(
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


bool
aflibSolarisSparcDevFile::isDataSizeSupported(aflib_data_size size)
{
   // If handle not yet allocated then indicate sizes supported
   if (_fd_int == -1)
      return (size == AFLIB_DATA_8S || size == AFLIB_DATA_16S);

   return (size == _size);
}
 
bool
aflibSolarisSparcDevFile::isEndianSupported(aflib_data_endian end)
{
   // Solaris on Sparc device only supports big endian
   return (end == AFLIB_ENDIAN_BIG);
}

bool
aflibSolarisSparcDevFile::isSampleRateSupported(int& rate)
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


