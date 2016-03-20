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

/*! \class aflibDevFile
    \brief Derived class to read and write to a Linux audio device.

 This is the class for Linux Device audio file reading and writing. It is based on
 the OSS API. It is as follows:

 Controlling the format data can be done by ioctl commands as follows:
    SNDCTL_DSP_SETFMT - Set Format of data to and from port
         AFMT_U8     - Unsigned 8 bit data
         AFMT_S16_LE - Signed 16 bit data Little Endian
    SNDCTL_DSP_STEREO - Set number of channels
         0           - Mono
         1           - Stereo
    SNDCTL_DSP_SPEED  - Sample Rate in samples per second
    SNDCTL_DSP_SETFRAGMENT - Set buffer sizes
         0xMMMMSSSS  - 2 to the SSSS power gives size of buffer. MMMM is num of fragments
    SNDCTL_DSP_GETBLKSIZE  - Get fragment size in bytes
*/

#ifndef _AFLIBDEVFILE_H
#define _AFLIBDEVFILE_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "aflibFile.h"

#define AFLIB_DEV_ITEM_BUFFER "aflib_dev_item_buffer"

class aflibDevFile : public aflibFile {

public:

   aflibDevFile();

   ~aflibDevFile();

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

   bool
   setItem(
      const char * item,
      const void * value);

   void
   programDevice();

   bool
   isDataSizeSupported(aflib_data_size size);

   bool
   isEndianSupported(aflib_data_endian end);

   bool
   isSampleRateSupported(int& rate);

private:

   int
   createBuffer(
      const aflibConfig& cfg,
      double factor);

bool   _create_mode;
int    _snd_format;
int    _snd_stereo;
int    _snd_speed;
double _snd_buffer;
int  _fd_int;
aflib_data_size  _size;
string  _file;

};


#endif
