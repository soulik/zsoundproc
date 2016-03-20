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


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "aflibConfig.h"


/*! \brief Constructor.
*/
aflibConfig::aflibConfig():
   _channels(0),
   _bits_per_sample(0),
   _sample_size(AFLIB_SIZE_UNDEFINED),
   _samples_per_second(0),
   _data_orientation(AFLIB_ORIENTATION_UNDEFINED),
   _data_endian(AFLIB_ENDIAN_UNDEFINED),
   _total_size(0)
{
}


/*! \brief Constructor with parameters.
*/
aflibConfig::aflibConfig(
   int num_channels,
   aflib_data_size bits_per_sample,
   int samples_per_second):
   _channels(num_channels),
   _sample_size(bits_per_sample),
   _samples_per_second(samples_per_second),
   _data_orientation(AFLIB_ORIENTATION_UNDEFINED),
   _data_endian(AFLIB_ENDIAN_UNDEFINED),
   _total_size(0)
{

   _bits_per_sample = returnBitsPerSample(_sample_size);
}


/*! \brief Destructor.
*/
aflibConfig::~aflibConfig()
{
}


/*! \brief Returns bits per sample based on data type.
*/
int
aflibConfig::returnBitsPerSample(aflib_data_size size)
{
   int ret_value = 0;

   if (_sample_size == AFLIB_SIZE_UNDEFINED)
   {
      ret_value = 0;
   }
   else if ((_sample_size == AFLIB_DATA_8S) || (_sample_size == AFLIB_DATA_8U))
   {
      ret_value = 8;
   }
   else if ((_sample_size == AFLIB_DATA_16S) || (_sample_size == AFLIB_DATA_16U))
   {
      ret_value = 16;
   }
   else if (_sample_size == AFLIB_DATA_32S) 
   {
      ret_value = 32;
   }

   return (ret_value);
}


