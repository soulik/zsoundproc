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


#ifndef _AFLIBDATA_H_
#define _AFLIBDATA_H_


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "aflibConfig.h"


/*! \class aflibData
    \brief Main data class for a segment of audio data.

 This is the main data class that defines a segment of audio data. The
 length of data can be set in the constructor. The configuration of the
 data can also be set either in the constructor or seperately. The actual
 space for the data is allocated in this class and should not be freed or
 allocated outside this class. This class provides access to this memory
 via a getDataPointer function.  But it is encouraged to use the getSample
 and setSample functions to manipulate the data. These functions are not
 efficient and will be made so in the future.  For those who wish to use the pointer
 access function then a call to getTotalLength should be made first.
 This will tell them the total length of the allocated memory buffer. It
 is up to the caller to make sure one does not write or read beyond this
 buffer. This class also provides convience functions to manipulate data.
 Functions convertToSize and convertToEndian will convert from one format to
 another. Function zeroData will zero all audio data.

 A copy constructor and assignment operator are provided so that an aflibData
 object can be set to the same data as another aflibData object.
*/

class aflibData {

public:

   aflibData(long length);

   aflibData(
      const aflibConfig& config,
      long length);

   aflibData(const aflibData& data);

   ~aflibData();

   aflibData&
   operator=(const aflibData& data);

   void
   setConfig(const aflibConfig& config);

   const aflibConfig&
   getConfig() const;

   void
   getLength(long& length) const;

   long
   getLength() const;

   void
   getOrigLength(long& length) const;

   long
   getOrigLength() const;

   void
   adjustLength(long length);

   void
   adjustTotalLength(long length);

   long
   getTotalLength();

   long
   getTotalAdjustLength();

   void *
   getDataPointer() const;

   void
   setSample(
      int sample,
      long position,
      int channel);

   int
   getSample(
      long position,
      int channel);

   void
   getMinMax(
      int& min_value,
      int& max_value) const;

   void
   convertToSize(aflib_data_size data_size);

   void
   convertToEndian(aflib_data_endian endian);

   aflib_data_endian
   getHostEndian() const;

   void
   zeroData();

private:

   void
   init();

   void
   setHostEndian();

   void
   allocate();


   void *    _data;        // holds pointer to data
   int       _initialized; // indicates if data has been initialized
   aflibConfig  _config;   // data organization
   int       _byte_inc;    // number of bytes for each sample
   long      _length;
   long      _adj_length;
   long      _total_length;
   aflib_data_endian  _endian;

};


#endif
