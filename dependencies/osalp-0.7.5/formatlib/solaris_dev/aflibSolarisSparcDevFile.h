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

/*! \class aflibSolarisSparcDevFile
    \brief Derived class to read and write to a Solaris Sparc audio device.

 This is the class for Solaris Sparc Device audio file reading and writing. It is based on
 the audio man page in Solaris 8.

*/

#ifndef _AFLIBSOLARISSPARCDEVFILE_H
#define _AFLIBSOLARISSPARCDEVFILE_H


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "aflibFile.h"


class aflibBufferDev;

class aflibSolarisSparcDevFile : public aflibFile {

public:

   aflibSolarisSparcDevFile();

   ~aflibSolarisSparcDevFile();

   aflibStatus
   afopen(
      const char * file,
      aflibConfig* cfg);

   aflibStatus
   afcreate(
      const char * file,
      const aflibConfig& config);

   aflibStatus
   afread(
      aflibData& data,
      long long position = -1);

   aflibStatus
   afwrite(
      aflibData& data,
      long long position = -1);

   void
   programDevice();

   bool
   isDataSizeSupported(aflib_data_size size);

   bool
   isEndianSupported(aflib_data_endian end);

   bool
   isSampleRateSupported(int& rate);

private:

bool   _create_mode;
int    _snd_format;
int    _snd_stereo;
int    _snd_speed;
int  _fd_int;
aflib_data_size  _size;
string  _file;
void  * _sample_array;
aflibBufferDev  * _buffer;

};


#endif
