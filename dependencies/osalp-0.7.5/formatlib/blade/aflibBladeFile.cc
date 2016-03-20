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


// class for MP3 audio file reading and writing


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream.h>
#include <string>

#include "aflibBladeFile.h"
#include "aflibData.h"

#include "aflibFileItem.h"

#define MODULE_NAME "aflibBladeFile"


typedef struct aflib_format {
	string mod_format;
	string mod_desc;
	string mod_ext;
	string mod_value1;
	string mod_value2;
	string mod_value3;
	string mod_magic;
}aflib_format_t;

extern "C"
{
   aflibFile *
   getAFileObject() { return ((aflibFile *)new aflibBladeFile()); }

void
query(list<aflibFileItem*>& support_list)
{

aflib_format_t formats[] = {
{
"BLADE-32Khz",
"MPEG Blade Encoder 32 Khz output rate",
".mp3",
"32",
"NONE",
"NONE",
"NONE",
},
{
"BLADE-48Khz",
"MPEG Blade Encoder 48 Khz output rate",
".mp3",
"48",
"NONE",
"NONE",
"NONE",
},
{
"BLADE-64Khz",
"MPEG Blade Encoder 64 Khz output rate",
".mp3",
"64",
"NONE",
"NONE",
"NONE",
},
{
"BLADE-96Khz",
"MPEG Blade Encoder 96 Khz output rate",
".mp3",
"96",
"NONE",
"NONE",
"NONE",
},
{
"BLADE-128Khz",
"MPEG Blade Encoder 128 Khz output rate",
".mp3",
"128",
"NONE",
"NONE",
"NONE",
},
{
"BLADE-160Khz",
"MPEG Blade Encoder 160Khz output rate",
".mp3",
"160",
"NONE",
"NONE",
"NONE",
},
// Better way to do this one? 
{"NONE","NONE","NONE","NONE","NONE","NONE","NONE"}

};

	aflibFileItem* item;
	for(aflib_format_t* format=&formats[0];format->mod_format !="NONE";format++){			
		//aflib_format_t* format = &formats[0];
		item	= new aflibFileItem();

		item->setFormat(format->mod_format);
		item->setDescription(format->mod_desc);
		item->setExtension(format->mod_ext);
		item->setName(MODULE_NAME);
		item->setValue1(format->mod_value1);
		item->setValue2(format->mod_value2);
		item->setValue3(format->mod_value3);
		support_list.push_back(item);
	}
	
}
} 


aflibBladeFile::aflibBladeFile()
{
   _fd = 0;
   _length_value = 0;
}

aflibBladeFile::~aflibBladeFile()
{
   // Should be moved to a close function
   if (_fd != 0)
   {
      pclose(_fd);
   }
}

aflibStatus
aflibBladeFile::afopen(
   const char * file,
   aflibConfig* cfg)
{
   aflibStatus  status = AFLIB_SUCCESS;

   status = AFLIB_ERROR_UNSUPPORTED;
   return(status);
}

aflibStatus
aflibBladeFile::afcreate(
   const char * file,
   const aflibConfig& cfg)
{
   aflibStatus  status = AFLIB_SUCCESS;
   string cmd_str("bladeenc -progress=0 ");
   aflibConfig  output_cfg;

   output_cfg = cfg;

   // Remove file if it exists
   (void)unlink(file);

   cmd_str.append("-br ");
   cmd_str.append(_value1);
   cmd_str.append(" ");

   if (cfg.getChannels() == 1)
   {
      cmd_str.append("-rawmono ");
   }
   else if (cfg.getChannels() == 2)
   {
      cmd_str.append("-rawstereo ");
   }
   else
   {
      status = AFLIB_ERROR_UNSUPPORTED;
      return(status);
   }

   // samples per second
   if (cfg.getSamplesPerSecond() == 48000)
   {
      cmd_str.append("-rawfreq=48000 ");
   }
   else if (cfg.getSamplesPerSecond() == 32000)
   {
      cmd_str.append("-rawfreq=32000 ");
   }
   else
   {
      cmd_str.append("-rawfreq=44100 ");
      output_cfg.setSamplesPerSecond(44100);
   }

   cmd_str.append("stdin ");
   cmd_str.append(file);

   // Route stderr to /dev/null so it does not get output
   cmd_str.append(" 2> /dev/null");

   _fd = popen(cmd_str.c_str(), "w");
   if (_fd == NULL)
   {
      status = AFLIB_ERROR_OPEN;
      return (status);
   }

   // Make sure pipe is flushed so that file is created by process
   fflush(_fd);

   // Set input and output audio configuration
   setInputConfig(cfg);
   setOutputConfig(output_cfg);

   return (status);
}

aflibStatus
aflibBladeFile::afread(
   aflibData& data,
   long long position )
{
   aflibStatus  status = AFLIB_SUCCESS;

   status = AFLIB_ERROR_UNSUPPORTED;
   return(status);
}

aflibStatus
aflibBladeFile::afwrite(
   aflibData& data,
   long long position )
{
   void * p_data;
   long   total_length;
   aflibStatus  status = AFLIB_SUCCESS;


   total_length = data.getTotalLength();
   p_data = data.getDataPointer();

   fwrite(p_data, total_length, 1, _fd);

   _length_value += total_length;

   return (status);
}

bool
aflibBladeFile::isDataSizeSupported(aflib_data_size size)
{
   return (size == AFLIB_DATA_16S);
}
 
bool
aflibBladeFile::isEndianSupported(aflib_data_endian end)
{
   return (end == AFLIB_ENDIAN_LITTLE);
}
 
bool
aflibBladeFile::isSampleRateSupported(int& rate)
{
   int value;
   bool ret_value = FALSE;
 
 
   // If not yet allocated then
   if (_fd == 0)
      return (rate == 22050 || rate == 44100);
 
   // Get the rate of the data
   value = getInputConfig().getSamplesPerSecond();

   // IF same rate then TRUE else return desired rate
   if (rate == value)
      ret_value = TRUE;
   else
      rate = value;
 
   return (ret_value);
}


