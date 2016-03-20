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


#ifndef _AFLIBCONFIG_H_
#define _AFLIBCONFIG_H_


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

// Note that g++ does not really need this. If -O with compile func in .h will be inline
#define INLINE inline

#include "aflib.h"


/*! \class aflibConfig
    \brief Audio data configuration.

 This class defines how audio data is formatted. This is just a simple
 data class. Most functions are inline in order to speed execution.
*/


class aflibConfig {

public:

   aflibConfig();

   aflibConfig(
      int num_channels,
      aflib_data_size bits_per_sample,
      int samples_per_second);

   ~aflibConfig();

   //! Gets the number of channels.
   INLINE
   int
   getChannels() const
      { return (_channels); };

   //! Sets the number of channels.
   INLINE
   void
   setChannels(int channels) 
      { _channels = channels; };

   //! Gets the number of bits per sample.
   INLINE
   int
   getBitsPerSample() const
      { return (_bits_per_sample); };

   //! Gets the sample size.
   INLINE
   aflib_data_size
   getSampleSize() const
      { return (_sample_size); };

   //! Sets the sample size.
   INLINE
   void
   setSampleSize(aflib_data_size bits_per_sample)
      { _sample_size = bits_per_sample; _bits_per_sample = returnBitsPerSample(_sample_size); };

   //! Gets the samples per second.
   INLINE
   int
   getSamplesPerSecond() const
      { return (_samples_per_second); };

   //! Sets the samples per second.
   INLINE
   void
   setSamplesPerSecond(int samples_per_second)
      { _samples_per_second = samples_per_second; };

   //! Gets the data orientation.
   INLINE
   aflib_data_orientation
   getDataOrientation() const
      { return (_data_orientation); };

   //! Sets the data orientation.
   INLINE
   void
   setDataOrientation(aflib_data_orientation data_orientation)
      { _data_orientation = data_orientation; };

   //! Gets the endian of the data.
   INLINE
   aflib_data_endian
   getDataEndian() const
      { return (_data_endian); };

   //! Sets the endian of the data.
   INLINE
   void
   setDataEndian(aflib_data_endian data_endian)
      { _data_endian = data_endian; };

   //! Gets the total number of samples.
   INLINE
   long long 
   getTotalSamples() const 
      { return(_total_size); };

   //! Sets the total number of samples.
   INLINE
   void
   setTotalSamples(long long total)
      { _total_size = total; };

private:

   int
   returnBitsPerSample(aflib_data_size size);

   int _channels;
   int _bits_per_sample;
   aflib_data_size  _sample_size;
   int _samples_per_second;
   aflib_data_orientation _data_orientation;
   aflib_data_endian _data_endian;
   long long  _total_size;

};


#endif
