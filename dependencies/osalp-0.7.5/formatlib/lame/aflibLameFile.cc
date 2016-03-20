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


// class for MP3 audio file writing
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream.h>
#include <string>

#include "aflibLameFile.h"
#include "aflibData.h"

#include "aflibFileItem.h"

#define MODULE_NAME "aflibLameFile"


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
   getAFileObject() { return ((aflibFile *)new aflibLameFile()); }

void
query(list<aflibFileItem*>& support_list)
{

aflib_format_t formats[] = {
{
"LAME-24Khz",
"MPEG Lame Encoder 24 Khz output rate",
".mp3",
"24",
"NONE",
"NONE",
"0(ÿ)"
},
{
"LAME-32Khz",
"MPEG Lame Encoder 32 Khz output rate",
".mp3",
"32",
"NONE",
"NONE",
"0(ÿ)"
},
{
"LAME-48Khz",
"MPEG Lame Encoder 48 Khz output rate",
".mp3",
"48",
"NONE",
"NONE",
"0(ÿ)"
},
{
"LAME-64Khz",
"MPEG Lame Encoder 64 Khz output rate",
".mp3",
"64",
"NONE",
"NONE",
"0(ÿ)"
},
{
"LAME-96Khz",
"MPEG Lame Encoder 96 Khz output rate",
".mp3",
"96",
"NONE",
"NONE",
"0(ÿ)"
},
{
"LAME-128Khz",
"MPEG Lame Encoder 128 Khz output rate",
".mp3",
"128",
"NONE",
"NONE",
"0(ÿ)"
},
{
"LAME-160Khz",
"MPEG Lame Encoder 160Khz output rate",
".mp3",
"160",
"NONE",
"NONE",
"0(ÿ)"
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


aflibLameFile::aflibLameFile()
{
   _fd = 0;
   _length_value = 0;
   _sample_rate = 0;
}

aflibLameFile::~aflibLameFile()
{
   // Should be moved to a close function
   if (_fd != 0)
   {
      pclose(_fd);
   }
}

aflibStatus
aflibLameFile::afopen(
   const char * file,
   aflibConfig* cfg)
{
   aflibStatus  status = AFLIB_SUCCESS;

   status = AFLIB_ERROR_UNSUPPORTED;
   return(status);
}
    
aflibStatus
aflibLameFile::afcreate(
   const char * file,
   const aflibConfig& cfg)
{
   aflibStatus  status = AFLIB_SUCCESS;
   string cmd_str("lame -r -x --nohist ");
   aflibConfig  output_cfg;

   // Store our current configuration
   output_cfg = cfg;
    
   // Remove file if it exists
   (void)unlink(file);

   cmd_str.append("-b ");
   cmd_str.append(_value1);
   cmd_str.append(" -h ");
        
   if (cfg.getChannels() == 1)
   {
      cmd_str.append("-m m ");
   }
   else if (cfg.getChannels() == 2)
   {
      cmd_str.append("-m j ");
   }
   else
   {
      status = AFLIB_ERROR_UNSUPPORTED;
      return(status);
   }

   // samples per second
   // If less than 32 Kbits per second output then use 16K sample rate as input
   if ( atoi(_value1.c_str()) < 32)
   {
      cmd_str.append("-s 16 ");
      output_cfg.setSamplesPerSecond(16000);
   }
   else if (cfg.getSamplesPerSecond() == 22050)
   {
      cmd_str.append("-s 22.05 ");
      output_cfg.setSamplesPerSecond(22050);
   }
   else
   {
      // If 44100 or not 22050 then use 44100
      cmd_str.append("-s 44.1 ");
      output_cfg.setSamplesPerSecond(44100);
    }

   _sample_rate = output_cfg.getSamplesPerSecond();

   cmd_str.append(" - ");
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
aflibLameFile::afread(
   aflibData& data,
   long long position )
{
   aflibStatus  status = AFLIB_SUCCESS;

   status = AFLIB_ERROR_UNSUPPORTED;
   return(status);
}

aflibStatus
aflibLameFile::afwrite(
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
aflibLameFile::isDataSizeSupported(aflib_data_size size)
{
   return (size == AFLIB_DATA_16S);
}
 
bool
aflibLameFile::isEndianSupported(aflib_data_endian end)
{
   return (end == AFLIB_ENDIAN_LITTLE);
}

bool
aflibLameFile::isSampleRateSupported(int& rate)
{
   bool ret_value = FALSE;
 

   // If not yet allocated then 
   if (_fd == 0)
      return (rate == 22050 || rate == 44100 || rate == 16000);

   // IF same rate then TRUE else return desired rate
   if (rate == _sample_rate)
      ret_value = TRUE;
   else
      rate = _sample_rate;

   return (ret_value);
} 

