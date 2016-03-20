/*
 * Copyright: (C) 2000 Bruce W. Forsberg
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

/*! \class aflibMpgFile
    \brief Derived class to read MP3 audio files.
 
  This class will read audio files of the MP3 (MPEG Audio Compression
  Format) format. This module uses the library from http://splay.sourceforge.net .
*/


#ifndef _AFLIBMPGFILE_H
#define _AFLIBMPGFILE_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "aflibFile.h"
#include "stdio.h"
#include "stdlib.h"

class Mpegtoraw;

class aflibMpgFile : public aflibFile {

public:

   aflibMpgFile();

   ~aflibMpgFile();

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

private:

short int* _buffer;
int 			_bufpos;
long 			_buflen;
long 		_samples_per_frame;
long		_total_frames;
int		_channels;
Mpegtoraw* _decoder;
long 		_current_frame;

};


#endif
