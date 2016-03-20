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

/*! \class aflibMpg123File
    \brief Derived class to read MP3 audio files using the mpg decoder.
 
  This class will read audio files of the MP3 (MPEG Audio Compression
  Format) format. This module uses the mpg decoder to decode MP3 files. 
  The mpg decoder can be obtained from http://www.mpg123.de. Also the decoder
  must be located in the PATH of your environment in order for this module to
  find it.
*/


#ifndef _AFLIBMPG123FILE_H
#define _AFLIBMPG123FILE_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


#include "aflibFile.h"
#include <string>


class aflibMpg123File : public aflibFile {

public:

   aflibMpg123File();

   ~aflibMpg123File();

   aflibStatus
   afopen(
      const char * file,
      aflibConfig* cfg);

   aflibStatus
   afcreate(
      const char * file,
      const aflibConfig& cfg);

   aflibStatus
   afread(
      aflibData& data,
      long long position = -1);

   aflibStatus
   afwrite(
      aflibData& data,
      long long position = -1);

   bool
   isDataSizeSupported(aflib_data_size size);
 
   bool
   isEndianSupported(aflib_data_endian end);
 
   bool
   isSampleRateSupported(int& rate);

bool
setItem(const char* item, const void* value);

private:


FILE*    _fd;
int     	_sample_rate;
int 		_depth;
int		_channels;
long 		_current_sample;
long		_last_frame;
long 		_samples_per_frame;
int 		_version;
int 		_blkalign;
string 	_mpeg_cmd;
string 	_filename;

};


#endif
