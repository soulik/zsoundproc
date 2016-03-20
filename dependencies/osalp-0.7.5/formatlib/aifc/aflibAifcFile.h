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

/*! \class aflibAifcFile
    \brief Derived class to read and write AIFC audio files.

  This class will read and write audio files of the AIFC (Audio Interchange File
  Format - Compressed) format. This format is identical to AIFF except that the
  AIFC format allows for reading and writing compressed data. This module uses
  the audiofile library to support AIFC. Currently it supports only uncompressed
  data.
*/

#ifndef _AFLIBAIFCFILE_H
#define _AFLIBAIFCFILE_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "aflibFile.h"
#include <audiofile.h>

class aflibAifcFile : public aflibFile {

public:

   aflibAifcFile();

   ~aflibAifcFile();

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

AFfilehandle  _handle;
long long     _total_samples;
};


#endif
