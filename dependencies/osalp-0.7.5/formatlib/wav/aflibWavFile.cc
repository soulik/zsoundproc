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

// class for WAV audio file reading and writing
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <iostream.h>

#include "aflibWavFile.h"
#include "aflibFileItem.h"
#include "aflibData.h"
#include "aflibDebug.h"

#define MODULE_NAME "aflibWavFile"

extern "C"
{
   aflibFile *
   getAFileObject() { return ((aflibFile *)new aflibWavFile()); }

	void
	query(list<aflibFileItem*>& support_list)
	{

		aflibFileItem* item;
		item	= new aflibFileItem();

		item->setFormat("WAV");
		item->setDescription("Microsoft Wave File Format");
		item->setExtension(".wav");
		item->setName(MODULE_NAME);
		item->setMagic("0(R), 1(I), 2(F), 3(F), 8(W), 9(A), 10(V), 11(E)");
		support_list.push_back(item);
	
		item	= new aflibFileItem();

		item->setFormat("WAV-ULAW");
		item->setDescription("Microsoft G.711 U-Law Wave File Format");
		item->setExtension(".wav");
		item->setName(MODULE_NAME);
		item->setValue1("ULAW");
		support_list.push_back(item);

		item	= new aflibFileItem();

		item->setFormat("WAV-ALAW");
		item->setDescription("Microsoft G.711 A-Law Wave File Format");
		item->setExtension(".wav");
		item->setName(MODULE_NAME);
		item->setValue1("ALAW");
		support_list.push_back(item);
	}
}

aflibWavFile::aflibWavFile()
{
   _handle = 0;
   _total_samples = 0;
}

aflibWavFile::~aflibWavFile()
{
   if (_handle != 0)
   {
      afCloseFile(_handle);
   }
}

aflibStatus
aflibWavFile::afopen(
   const char * file,
   aflibConfig* cfg)
{
   aflibStatus  status = AFLIB_SUCCESS;
   aflibConfig  input_cfg;


   _handle = afOpenFile(file, "r", NULL);
   if (_handle == AF_NULL_FILEHANDLE)
   {
      status = AFLIB_ERROR_OPEN;
      return (status);
   }

   // Store user specified results as defaults
   if (cfg != NULL)
   {
      input_cfg = *cfg;
   }

   afSetVirtualByteOrder(_handle, AF_DEFAULT_TRACK, AF_BYTEORDER_LITTLEENDIAN);
   input_cfg.setDataOrientation(AFLIB_INTERLEAVE);
   input_cfg.setDataEndian(AFLIB_ENDIAN_LITTLE);
   input_cfg.setChannels(afGetChannels(_handle, AF_DEFAULT_TRACK));
   input_cfg.setTotalSamples(afGetFrameCount(_handle, AF_DEFAULT_TRACK));
   _total_samples = afGetFrameCount(_handle, AF_DEFAULT_TRACK);

   int sampfmt, sampwidth;
   afGetSampleFormat(_handle, AF_DEFAULT_TRACK, &sampfmt, &sampwidth);

   if (sampwidth == 8 && sampfmt == AF_SAMPFMT_UNSIGNED)
   {
      input_cfg.setSampleSize(AFLIB_DATA_8U);
   }
   else if (sampwidth == 8 && sampfmt == AF_SAMPFMT_TWOSCOMP)
   {
      input_cfg.setSampleSize(AFLIB_DATA_8S);
   }
   else if (sampwidth == 16 && sampfmt == AF_SAMPFMT_UNSIGNED)
   {
      input_cfg.setSampleSize(AFLIB_DATA_16U);
   }
   else if (sampwidth == 16 && sampfmt == AF_SAMPFMT_TWOSCOMP)
   {
      input_cfg.setSampleSize(AFLIB_DATA_16S);
   }
   else
   {
      status = AFLIB_ERROR_UNSUPPORTED;
      return (status);

   }
   input_cfg.setSamplesPerSecond((int)afGetRate(_handle, AF_DEFAULT_TRACK));

   // Seek to start of audio data
   afSeekFrame(_handle, AF_DEFAULT_TRACK, 0);

   // Set input and output audio configuration
   setInputConfig(input_cfg);
   setOutputConfig(input_cfg);

   return (status);
}

aflibStatus
aflibWavFile::afcreate(
   const char * file,
   const aflibConfig& cfg)
{
   AFfilesetup  setup;
   aflibStatus  status = AFLIB_SUCCESS;
   aflibConfig  output_cfg(cfg);

   (void)unlink(file);

   setup = afNewFileSetup();

   // IF ULAW compression then enable compression
   if (_value1 == "ULAW")
      afInitCompression(setup, AF_DEFAULT_TRACK, AF_COMPRESSION_G711_ULAW);
   else if (_value1 == "ALAW")
      afInitCompression(setup, AF_DEFAULT_TRACK, AF_COMPRESSION_G711_ALAW);
   afInitFileFormat(setup, AF_FILE_WAVE);
   afInitChannels(setup, AF_DEFAULT_TRACK, cfg.getChannels());

   if (_value1 == "ULAW" || _value1 == "ALAW")
   {
      afInitSampleFormat(setup, AF_DEFAULT_TRACK, AF_SAMPFMT_TWOSCOMP, 16);
      output_cfg.setSampleSize(AFLIB_DATA_16S);
   }
   else if ((cfg.getSampleSize() == AFLIB_DATA_8U) || (cfg.getSampleSize() == AFLIB_DATA_8S))
   {
      afInitSampleFormat(setup, AF_DEFAULT_TRACK, AF_SAMPFMT_UNSIGNED, 8);
      output_cfg.setSampleSize(AFLIB_DATA_8U);
   }
   else if ((cfg.getSampleSize() == AFLIB_DATA_16U) || (cfg.getSampleSize() == AFLIB_DATA_16S))
   {
      afInitSampleFormat(setup, AF_DEFAULT_TRACK, AF_SAMPFMT_TWOSCOMP, 16);
      output_cfg.setSampleSize(AFLIB_DATA_16S);
   }
   else
   {
      cerr << "Illegal case!" << endl;
   }

   afInitRate(setup, AF_DEFAULT_TRACK, (double)cfg.getSamplesPerSecond());

   _handle = afOpenFile(file, "w", setup);
   if (_handle == AF_NULL_FILEHANDLE)
   {
      status = AFLIB_ERROR_OPEN;
      afFreeFileSetup(setup);
      return (status);
   }

   if (cfg.getDataEndian() == AFLIB_ENDIAN_BIG)
      afSetVirtualByteOrder(_handle, AF_DEFAULT_TRACK, AF_BYTEORDER_BIGENDIAN);
   else
      afSetVirtualByteOrder(_handle, AF_DEFAULT_TRACK, AF_BYTEORDER_LITTLEENDIAN);

   afFreeFileSetup(setup);

   // Set input and output audio configuration
   setInputConfig(cfg);
   setOutputConfig(output_cfg);

   return(status);
}

aflibStatus
aflibWavFile::afread(
   aflibData& data,
   long long position )
{
   // Read data from a wave file. A -1 for position means read from the
   // current position. Position should be specified as samples. It starts
   // at position 1. 

   void * p_data;
   long   new_length;
   aflibStatus  status = AFLIB_SUCCESS;


   data.setConfig(getInputConfig());
   p_data = data.getDataPointer();

   // Seek to correct position
   if (position != -1 && position < _total_samples)
	{
      afSeekFrame(_handle, AF_DEFAULT_TRACK, position);
	}

	position = afTellFrame(_handle, AF_DEFAULT_TRACK);

   // Don't read past total samples available
   if (_total_samples < (position + data.getLength()))
   {
      new_length = _total_samples - position;
      if (new_length < 0)
         new_length = 0;
   }
   else
   {
      new_length = data.getLength();
   }

   if (new_length != 0)
      new_length = (long)
         afReadFrames(_handle, AF_DEFAULT_TRACK, p_data, new_length);

   // IF we reached the end of the file then return error
   if (new_length == 0)
   {
      status = AFLIB_END_OF_FILE;
      data.adjustLength(0);
   }
   // IF we read the last chunk
   else if (new_length != data.getLength())
   {
      // When we only have a partial read then readjust the length of data
      data.adjustLength(new_length);
   }
	aflib_debug("WAV afread: position: %d, new_length: %d", position,new_length);

   return(status);
}

aflibStatus
aflibWavFile::afwrite(
   aflibData& data,
   long long position )
{
   void * p_data;
   long   new_length;
   aflibStatus  status = AFLIB_SUCCESS;

   p_data = data.getDataPointer();
 
   new_length = (long)
      afWriteFrames(_handle, AF_DEFAULT_TRACK, p_data, data.getLength());
 
   if (new_length == 0)
   {
      status = AFLIB_END_OF_FILE;
   }
   else if (new_length != data.getLength())
   {
      // Need something for this case
   }                                                                              

   return(status);
}

bool
aflibWavFile::isDataSizeSupported(aflib_data_size size)
{
   int sampfmt, sampwidth;
   aflib_data_size value = AFLIB_SIZE_UNDEFINED;


   // If handle not yet allocated then indicate sizes supported
   if (_handle == 0)
      return (size == AFLIB_DATA_8U || size == AFLIB_DATA_16S);

   // Get the size of the data
   afGetSampleFormat(_handle, AF_DEFAULT_TRACK, &sampfmt, &sampwidth);
   if (sampwidth == 8 && sampfmt == AF_SAMPFMT_UNSIGNED)
   {
      value = AFLIB_DATA_8U;
   }
   else if (sampwidth == 8 && sampfmt == AF_SAMPFMT_TWOSCOMP)
   {
      value = AFLIB_DATA_8S;
   }
   else if (sampwidth == 16 && sampfmt == AF_SAMPFMT_UNSIGNED)
   {
      value = AFLIB_DATA_16U;
   }
   else if (sampwidth == 16 && sampfmt == AF_SAMPFMT_TWOSCOMP)
   {
      value = AFLIB_DATA_16S;
   }

   return (size == value);
}
 
bool
aflibWavFile::isEndianSupported(aflib_data_endian end)
{
   int end_value;
   aflib_data_endian value;

   // If handle not yet allocated then indicate any endian is supported
   if (_handle == 0)
      return (TRUE);

   // Get the endian of the data
   end_value = afGetVirtualByteOrder(_handle, AF_DEFAULT_TRACK);
   if (end_value == AF_BYTEORDER_LITTLEENDIAN)
      value = AFLIB_ENDIAN_LITTLE;
   else
      value = AFLIB_ENDIAN_BIG;

   return (end == value);
}

bool
aflibWavFile::isSampleRateSupported(int& rate)
{
   int value;
   double double_value;
   bool ret_value = FALSE;


   // If handle not yet allocated then indicate any sample rate is supported
   if (_handle == 0)
      return (TRUE);

   // Get the rate of the data
   double_value = afGetRate(_handle, AF_DEFAULT_TRACK);
   value = (int)(double_value + 0.5);

   // IF same rate then TRUE else return desired rate
   if (value == rate)
      ret_value = TRUE;
   else
      rate = value;

   return (ret_value);
}


