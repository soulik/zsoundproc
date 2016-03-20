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


#include <stdio.h>
#include <iostream>
#include <string>
using std::cerr;
using std::endl;

#include "aflibData.h"

#ifdef HAVE_BYTESWAP_H
#include <byteswap.h>
#define  swab_short(x)  bswap_16(x) 
#define  swab_int(x)    bswap_32(x) 
#else
#define  swab_short(x)  (((unsigned short)(x) << 8) | ((unsigned short)(x) >> 8))
#define  swab_int(x)   ((((x) & 0xff000000) >> 24) | (((x) & 0x00ff0000) >> 8) | \
                        (((x) & 0x0000ff00) << 8)  | (((x) & 0x000000ff) << 24))
#endif

/*Should be faster  */
#define RIGHT(datum,bits) ((datum) >> bits)
#define LEFT(datum,bits) ((datum) << bits)


/*! \brief Constructor.
*/
aflibData::aflibData(long length)
{
   // _data is not initialized yet
   _initialized = FALSE;
   _data = NULL;
   _length = length;
   _adj_length = length;
   _total_length = 0;
   _byte_inc = 0;

   setHostEndian();
}


/*! \brief Constructor.
*/
aflibData::aflibData(
   const aflibConfig& config,
   long length)
{
   _config = config;
   _length = length;
   _adj_length = length;
   _data   = NULL;

   setHostEndian();

   _byte_inc = _config.getBitsPerSample() / 8;

   init();
}


/*! \brief Copy Constructor.

    This copy constructor will create another data object with the same data as the
    other one. It will have newly allocated memory with an identical configuration and
    data.
*/
aflibData::aflibData(const aflibData& data)
{
   _data = NULL;
   _config = data.getConfig();
   data.getLength(_adj_length);
   data.getOrigLength(_length);
   _byte_inc = _config.getBitsPerSample() / 8;

   setHostEndian();

   init();

   if (data.getDataPointer() != NULL)
   {
      memcpy(_data, data.getDataPointer(), _total_length);   
   }
}


/*! \brief Assignment operator.

    This will set one data object with the same data as the
    other one. It will have newly allocated memory with an identical configuration and
    data.
*/
aflibData&
aflibData::operator=(const aflibData& data)
{
   _config = data.getConfig();
   data.getOrigLength(_length);
   data.getLength(_adj_length);
   _byte_inc = _config.getBitsPerSample() / 8;

   setHostEndian();

   init();

   if (data.getDataPointer() != NULL)
   {
      memcpy(_data, data.getDataPointer(), _total_length);   
   }
   return (*this);
}


/*! \brief Destructor.
*/
aflibData::~aflibData()
{
   delete [] (unsigned char *) _data;
}


/*! \brief Set the audio configuration of the data.

    This allows one to set the audio configuration of this data class. This can
    be used when it was not set in the constructor or one needs to change it. Any
    audio data stored will be lost and a new array allocated.
*/
void
aflibData::setConfig(const aflibConfig& config)
{
   _config = config;

   _byte_inc = _config.getBitsPerSample() / 8;

   init();
}


/*! \brief Gets audio configuration data.
*/
const aflibConfig&
aflibData::getConfig() const
{
   return (_config);
}


/*! \brief Gets length of data.
    This returns the number of samples. It does not matter how many channels.
    It only means samples no matter how many channels.
*/
void
aflibData::getLength(long& length) const
{
   length = _adj_length;
}


/*! \brief Same as getLength(length);.
*/
long
aflibData::getLength() const
{
   return (_adj_length);
}


/*! \brief Get the original length.
    This returns the original number of samples that was used to allocate
    space. If the samples are adjusted down then this is the original value
    and not the adjusted value. It does not matter how many channels.
    It only means samples no matter how many channels.
*/
void
aflibData::getOrigLength(long& length) const
{
   length = _length;
}


/*! \brief Same as getOrigLength(length).
*/
long
aflibData::getOrigLength() const
{
   return (_length);
}


/*! \brief Adjust size of data array.

    This allows one to adjust the size of the data array downward without allocating
    new data. This is useful when a data object is passed to a read file class with a
    certain size. If we are at the end of the file and only a portion of the data
    requested is available then the size of the data object can be changed downward.
*/
void
aflibData::adjustLength(long length)
{
   if (length < _length)
   {
      _adj_length = length;
   }
}


/*! \brief Adjust total size of data array.

    This allows one to adjust the size of the data array downward without allocating
    new data. This is useful when a data object is passed to a read file class with a
    certain size. If we are at the end of the file and only a portion of the data
    requested is available then the size of the data object can be changed downward.
    The parameter passed is in total bytes not samples
*/
void
aflibData::adjustTotalLength(long length)
{
   long sample_length;

   sample_length = length / (_config.getChannels() * _byte_inc);
   adjustLength(sample_length);
}


/*! \brief Gets the total length of memory that is allocated.
*/
long
aflibData::getTotalLength()
{
   return _total_length;
}


/*! \brief Gets the total length of memory that is adjusted.
*/
long
aflibData::getTotalAdjustLength()
{
   return (_adj_length * _config.getChannels() * _byte_inc);
}


/*! \brief Get a pointer to the internal data.

    This will return a void pointer to the internally allocated data array. One should
    use the setSample and getSample routines if at all possible instead of this one.
    Possible exceptions are the start of an audio chain such as a device or file. In
    that case we determine the data layout so we can just get a pointer to the data and
    copy the data. getTotalLength will tell the total size of the allocated memory. One
    should not read or write beyond this value.
*/
void *
aflibData::getDataPointer() const
{
   return _data;
}


/*! \brief Sets a sample of audio data.

    This stores a single sample value into the data array based on the audio configuration
    data. First position begins at 0.  First channel begins at 0.
*/
void
aflibData::setSample(
   int sample,
   long position,
   int channel)
{
   // This function needs a much more efficient algorithm and needs to be
   // an inline function if at all possible.


   // TBD only AFLIB_INTERLEAVE supported currently
   if (_config.getDataEndian() == _endian)
   {
      long data_position = _config.getChannels() * position +
                           channel;
#if 0
      if ((data_position * _byte_inc + _byte_inc) > _total_length)
      {
         cerr << "Buffer Overrun! ERROR!" << endl;
         return;
      }
#endif

      if (_config.getSampleSize() == AFLIB_DATA_16S)
         ((signed short int *)_data)[data_position] = (signed short int)sample;
      else if (_config.getSampleSize() == AFLIB_DATA_8U)
         ((unsigned char *)_data)[data_position] = (unsigned char)sample;
      else if (_config.getSampleSize() == AFLIB_DATA_8S)
         ((signed char *)_data)[data_position] = (signed char)sample;
      else if (_config.getSampleSize() == AFLIB_DATA_16U)
         ((unsigned short int *)_data)[data_position] = (unsigned short int)sample;
      else if (_config.getSampleSize() == AFLIB_DATA_32S)
         ((signed int *)_data)[data_position] = (signed int)sample;
   }
   else
   {
      long data_position = _config.getChannels() * position +
                           channel;

      if (_config.getSampleSize() == AFLIB_DATA_16S)
      {
          sample = (0xff & sample) << 8 | (0xff00 & sample) >> 8;
         ((signed short int *)_data)[data_position] = (signed short int)sample;
      }
      else if (_config.getSampleSize() == AFLIB_DATA_8U)
         ((unsigned char *)_data)[data_position] = (unsigned char)sample;
      else if (_config.getSampleSize() == AFLIB_DATA_8S)
         ((signed char *)_data)[data_position] = (signed char)sample;
      else if (_config.getSampleSize() == AFLIB_DATA_16U)
      {
          sample = (0xff & sample) << 8 | (0xff00 & sample) >> 8;
         ((unsigned short int *)_data)[data_position] = (unsigned short int)sample;
      }
      else if (_config.getSampleSize() == AFLIB_DATA_32S)
      {
          sample = (0xff & sample) << 24 | (0xff00 & sample) << 8 |
                   (0xff0000 & sample) >> 8 | (0xff000000 & sample) >> 24;
         ((signed int *)_data)[data_position] = (signed int)sample;
      }
   }
}


/*! \brief Gets a sample of audio data.

    This will retrieve a single data value from interval memory and return it as an int.
    First position begins at 0.  First channel begins at 0.
*/
int
aflibData::getSample(
   long position,
   int channel)
{
   int ret_value = 0;


   // TBD only AFLIB_INTERLEAVE supported currently
   if (_config.getDataEndian() == _endian)
   {
      long data_position = _config.getChannels() * position +
                           channel;
#if 0
      if ((data_position * _byte_inc + _byte_inc) > _total_length)
      {
         cerr << "Buffer Overrun! ERROR!" << endl;
         return 0;
      }
#endif

      if (_config.getSampleSize() == AFLIB_DATA_16S)
         ret_value = (int) ((signed short int *)_data)[data_position];
      else if (_config.getSampleSize() == AFLIB_DATA_8U)
         ret_value = (int) ((unsigned char *)_data)[data_position];
      else if (_config.getSampleSize() == AFLIB_DATA_8S)
         ret_value = (int) ((signed char *)_data)[data_position];
      else if (_config.getSampleSize() == AFLIB_DATA_16U)
         ret_value = (int)((unsigned short int *)_data)[data_position];
      else if (_config.getSampleSize() == AFLIB_DATA_32S)
         ret_value = (int)((signed int *)_data)[data_position];
   }
   else
   {
      long data_position = _config.getChannels() * position +
                           channel;

      if (_config.getSampleSize() == AFLIB_DATA_16S)
      {
         ret_value = (int) ((signed short int *)_data)[data_position];
	/*FIXME use swab_short */
         ret_value = (int)((signed short int)((0xff & ret_value) << 8 | (0xff00 & ret_value) >> 8));
      }
      else if (_config.getSampleSize() == AFLIB_DATA_8U)
         ret_value = (int) ((unsigned char *)_data)[data_position];
      else if (_config.getSampleSize() == AFLIB_DATA_8S)
         ret_value = (int) ((signed char *)_data)[data_position];
      else if (_config.getSampleSize() == AFLIB_DATA_16U)
      {
         ret_value = (int)((unsigned short int *)_data)[data_position];
	/*FIXME use swab_short */
         ret_value = (0xff & ret_value) << 8 | (0xff00 & ret_value) >> 8;
      }
      else if (_config.getSampleSize() == AFLIB_DATA_32S)
      {
         ret_value = (int)((signed int *)_data)[data_position];
	/*FIXME use swab_int */
         ret_value = (int)((signed int)
                   ((0xff & ret_value) << 24 | (0xff00 & ret_value) << 8 |
                   (0xff0000 & ret_value) >> 8 | (0xff000000 & ret_value) >> 24));
      }
   }

   return (ret_value);
}


/*! \brief Returns absolute min and max values of data type selected.
*/
void
aflibData::getMinMax(
   int& min_value,
   int& max_value) const
{
   switch (_config.getSampleSize())
   {
      case AFLIB_DATA_8S:
         min_value = -127;
         max_value = 128;
      break;

      case AFLIB_DATA_8U:
         min_value = 0;
         max_value = 255;
      break;

      case AFLIB_DATA_16S:
         min_value = -32767;
         max_value = 32768;
      break;

      case AFLIB_DATA_16U:
         min_value = 0;
         max_value = 65536;
      break;

      case AFLIB_DATA_32S:
         min_value = -2147483646;
         max_value = 2147483647;
      break;

      default:
         min_value = 0;
         max_value = 0;
      break;
   }
}


void
aflibData::setHostEndian()
{
   // This private function is called from constructors and gets the endian of
   // the platform this this object is currently running on.

   unsigned short seven = 7;
   unsigned char * end_test;

   end_test = (unsigned char *)&seven;
   if (end_test[0] == seven)
      _endian = AFLIB_ENDIAN_LITTLE;
   else
      _endian = AFLIB_ENDIAN_BIG;
}


/*! \brief Returns endian state of this computer platform.
*/
aflib_data_endian
aflibData::getHostEndian() const
{
   return (_endian);
}


void
aflibData::init()
{
   delete [] (unsigned char *)_data;
   _data = NULL;

   allocate();
}

void
aflibData::allocate()
{
   // This will allocate a new _data array without deleting any previous _data.

   _total_length = _config.getChannels() * _length * _byte_inc;
   _data = new unsigned char [_total_length];
   if (_data == NULL)
   {
      cerr << "Error. Out of memory" << endl;
   }
   else
   {
      _initialized = TRUE;
   }
}


/*! \brief This will zero all audio data in this object.
*/
void
aflibData::zeroData()
{
   // This function will cause the data to be zeroed.


   if (_data)
      memset(_data, 0, _total_length);
}


/*! \brief Convert data from one data size to another.

    This function will convert the data in this object to a different
    size. It will rescale the data. If the user passes the data size
    that the data is currently in then nothing will be done.
*/
void
aflibData::convertToSize(aflib_data_size data_size)
{
   register long long data_length, i;


   // IF already in correct format then just return
   if (_config.getSampleSize() == data_size)
      return;

   data_length =  getLength() * _config.getChannels() ;

   // Convert to new format
   switch(_config.getSampleSize())
   {
      case AFLIB_DATA_8S:
      {
         switch (data_size)
         {
            // Convert from 8S to 8U
            case AFLIB_DATA_8U:
            {
               _config.setSampleSize(AFLIB_DATA_8U);
               {
                  char* orig_ptr = (char *)_data;
                  for (i = 0; i < data_length; i++)
                     *orig_ptr++ ^= 0x80;
               }
            }
            break;

            // Convert from 8S to 16S
            case AFLIB_DATA_16S:
            {
               char* old_data = (char  *)_data;

               // Configure new 16 bit data
               _config.setSampleSize(AFLIB_DATA_16S);
               _byte_inc = _config.getBitsPerSample() / 8;

               // allocate new array without destroying old data
               allocate();

               // IF different endians then swap bytes
               if (_config.getDataEndian() != getHostEndian())
               {
                  short* dest_ptr = (short *)_data;
                  char* orig_ptr = old_data;

                  for (i = 0; i < data_length; i++)
                  {
                     *dest_ptr = LEFT(*orig_ptr++, 8);
                     *dest_ptr++ = swab_short(*dest_ptr);
                  }
               }
               else
               {
                  short* dest_ptr = (short *)_data;
                  char* orig_ptr = old_data;

                  for (i = 0; i < data_length; i++)
                     *dest_ptr++ = LEFT(*orig_ptr++, 8);
               }

               // delete the old orig 8 bit data array
               delete [] old_data;
            }
            break;

            // Convert from 8S to 16U
            case AFLIB_DATA_16U:
            {
               char* old_data = (char  *)_data;

               // Configure new 16 bit data
               _config.setSampleSize(AFLIB_DATA_16U);
               _byte_inc = _config.getBitsPerSample() / 8;

               // allocate new array without destroying old data
               allocate();
               // IF different endians then swap bytes
               if (_config.getDataEndian() != getHostEndian())
               {
                  short* dest_ptr = (short *)_data;
                  char* orig_ptr = old_data;

                  for (i = 0; i < data_length; i++)
                  {
                     // Scale then toggle sign bit
                     *dest_ptr = LEFT(*orig_ptr++, 8)^0x8000;
                     *dest_ptr++ = swab_short(*dest_ptr);
                  }
               }
               else
               {
                  short* dest_ptr = (short *)_data;
                  char* orig_ptr = old_data;

                  for (i = 0; i < data_length; i++)
                     // Scale then toggle sign bit
                     *dest_ptr++ = LEFT(*orig_ptr++, 8)^0x8000;
               }

               // delete the old orig 8 bit data array
               delete [] old_data;
            }
            break;

            // Convert from 8S to 32S
            case AFLIB_DATA_32S:
            {
               char* old_data = (char  *)_data;

               // Configure new 32 bit data
               _config.setSampleSize(AFLIB_DATA_32S);
               _byte_inc = _config.getBitsPerSample() / 8;

               // allocate new array without destroying old data
               allocate();

               // IF different endians then swap bytes
               if (_config.getDataEndian() != getHostEndian())
               {
                  int* dest_ptr = (int *)_data;
                  char* orig_ptr = old_data;

                  for (i = 0; i < data_length; i++)
                  {
                     *dest_ptr = LEFT(*orig_ptr++, 24);
                     *dest_ptr++ = swab_int(*dest_ptr);
                  }
               }
               else
               {
                  int* dest_ptr = (int *)_data;
                  char* orig_ptr = old_data;

                  for (i = 0; i < data_length; i++)
                     *dest_ptr++ = LEFT(*orig_ptr++, 24);
               }

               // delete the old orig 8 bit data array
               delete [] old_data;
            }
            break;

            default:
            {
               cerr << "Software Error. Ilegal data size!" << endl;
            }
            break;
         }
      }
      break;
 
      case AFLIB_DATA_8U:
      {
         switch (data_size)
         {
            // Convert from 8U to 8S
            case AFLIB_DATA_8S:
            {
               _config.setSampleSize(AFLIB_DATA_8S);
               {
                  char* orig_ptr = (char *)_data;

                  for (i = 0; i < data_length; i++)
                     // Toggle sign bit
                     *orig_ptr++ ^= 0x80;
               }
            }
            break;

            // Convert from 8U to 16S
            case AFLIB_DATA_16S:
            {
               char* old_data = ( char  *)_data;

               // Configure new 16 bit data
               _config.setSampleSize(AFLIB_DATA_16S);
               _byte_inc = _config.getBitsPerSample() / 8;

               // allocate new array without destroying old data
               allocate();

               // IF different endians then swap bytes
               if (_config.getDataEndian() != getHostEndian())
               {
                  char* orig_ptr = old_data;
                  short* dest_ptr = (short *)_data;

                  for (i = 0; i < data_length; i++)
                  {
                     *dest_ptr = LEFT(*orig_ptr++, 8)^0x8000 ;
                     *dest_ptr++ = swab_short(*dest_ptr);
                  }
               }
               else
               {
                  char* orig_ptr = old_data;
                  short* dest_ptr = (short *)_data;

                  for (i = 0; i < data_length; i++)
                     *dest_ptr++ = LEFT(*orig_ptr++, 8)^0x8000 ;
               }

               // delete the old orig 8 bit data array
               delete [] old_data;
            }
            break;

            // Convert from 8U to 16U
            case AFLIB_DATA_16U:
            {
               unsigned char* old_data = (unsigned char  *)_data;

               // Configure new 16 bit data
               _config.setSampleSize(AFLIB_DATA_16U);
               _byte_inc = _config.getBitsPerSample() / 8;

               // allocate new array without destroying old data
               allocate();

               {
                  // IF different endians then swap bytes
                  if (_config.getDataEndian() != getHostEndian())
                  {
                     unsigned short* dest_ptr = ( unsigned short *)_data;
                     unsigned char* orig_ptr = old_data;

                     for (i = 0; i < data_length; i++)
                     {
                        *dest_ptr = LEFT(*orig_ptr++, 8);
                        *dest_ptr++ = swab_short(*dest_ptr);
                     }
                  }
                  else
                  {
                     unsigned short* dest_ptr = ( unsigned short *)_data;
                     unsigned char* orig_ptr = old_data;

                     for (i = 0; i < data_length; i++)
                        *dest_ptr++ = LEFT(*orig_ptr++, 8);
                  }
               }

               // delete the old orig 8 bit data array
               delete [] old_data;
            }
            break;

            // Convert from 8U to 32S
            case AFLIB_DATA_32S:
            {
               char* old_data = ( char  *)_data;

               // Configure new 32 bit data
               _config.setSampleSize(AFLIB_DATA_32S);
               _byte_inc = _config.getBitsPerSample() / 8;

               // allocate new array without destroying old data
               allocate();

               // IF different endians then swap bytes
               if (_config.getDataEndian() != getHostEndian())
               {
                  char* orig_ptr = old_data;
                  int* dest_ptr = (int*)_data;

                  for (i = 0; i < data_length; i++)
                  {
                     *dest_ptr = LEFT(*orig_ptr++, 24)^0x80000000 ;
                     *dest_ptr++ = swab_int(*dest_ptr);
                  }
               }
               else
               {
                  char* orig_ptr = old_data;
                  int* dest_ptr = (int*)_data;

                  for (i = 0; i < data_length; i++)
                     *dest_ptr++ = LEFT(*orig_ptr++, 24)^0x80000000 ;
               }

               // delete the old orig 8 bit data array
               delete [] old_data;
            }
            break;

            default:
            {
               cerr << "Software Error. Ilegal data size!" << endl;
            }
            break;
         }
      }
      break;
 
      case AFLIB_DATA_16S:
      {
         switch (data_size)
         {
            // Convert from 16S to 8S
            // Scale to 8-bit
            case AFLIB_DATA_8S:
            {
               // IF different endians then swap bytes
         	   if (_config.getDataEndian() != getHostEndian())
               {
                  {
   	           	     short* orig_ptr = ( short *)_data;
                     char* dest_ptr = ( char *)orig_ptr;

                     for (i = 0; i < data_length; i++)
                     {
                        *orig_ptr = swab_short(*orig_ptr);
                        *dest_ptr++ = RIGHT(*orig_ptr++, 8);
                     }
                  }
               }
               else
               {
                  short* orig_ptr = ( short *)_data;
                  char* dest_ptr = ( char *)orig_ptr;

                  for (i = 0; i < data_length; i++)
                     *dest_ptr++ = RIGHT(*orig_ptr++, 8);
               }

               _config.setSampleSize(AFLIB_DATA_8S);
               _byte_inc = _config.getBitsPerSample() / 8;
               _total_length = _config.getChannels() * _length * _byte_inc;
            }
            break;

            // Convert from 16S to 8U
            // Scale to 8-bit and toggle sign bit
            case AFLIB_DATA_8U:
            {
      	       // IF different endians then swap bytes
         	   if (_config.getDataEndian() != getHostEndian())
               {
                  short* orig_ptr = ( short *)_data;
                  char* dest_ptr = ( char *)orig_ptr;

                  for (i = 0; i < data_length; i++)
                  {
                     *orig_ptr = swab_short(*orig_ptr);
                     *dest_ptr++ = RIGHT(*orig_ptr++, 8)^0x80;
                  }
               }
               else
               {
                  short* orig_ptr = ( short *)_data;
                  char* dest_ptr = ( char *)orig_ptr;

                  for (i = 0; i < data_length; i++)
                     *dest_ptr++ = RIGHT(*orig_ptr++, 8)^0x80;
               }

               _config.setSampleSize(AFLIB_DATA_8U);
               _byte_inc = _config.getBitsPerSample() / 8;
               _total_length = _config.getChannels() * _length * _byte_inc;
            }
            break;

            // Convert 16S to 16U
            // Toggle sign bit
            case AFLIB_DATA_16U:
            {
               // IF different endians then swap bytes
               if (_config.getDataEndian() != getHostEndian())
               {
                  short* orig_ptr = ( short *)_data;

                  for (i = 0; i < data_length; i++)
                  {
                     *orig_ptr = swab_short(*orig_ptr);
                     *orig_ptr ^= 0x8000;
                     *orig_ptr = swab_short(*orig_ptr);
                     orig_ptr++;
                     // Maybe just this one will work faster
                     // *orig_ptr++ = ^ 0x0080;
                  }
               }
               else
               {
                  short* orig_ptr = ( short *)_data;

                  for (i = 0; i < data_length; i++)
                     *orig_ptr++ ^= 0x8000;
               }

               _config.setSampleSize(AFLIB_DATA_16U);
            }
            break;

            // Convert from 16S to 32S
            // Scale to 32-bit
            case AFLIB_DATA_32S:
            {
               short* old_data = (short  *)_data;

               // Configure new 32 bit data
               _config.setSampleSize(AFLIB_DATA_32S);
               _byte_inc = _config.getBitsPerSample() / 8;

               // allocate new array without destroying old data
               allocate();

               // IF different endians then swap bytes
               if (_config.getDataEndian() != getHostEndian())
               {
                  int* dest_ptr = (int *)_data;
                  short* orig_ptr = old_data;

                  for (i = 0; i < data_length; i++)
                  {
                     *dest_ptr = LEFT(*orig_ptr++, 16);
                     *dest_ptr++ = swab_int(*dest_ptr);
                  }
               }
               else
               {
                  int* dest_ptr = (int *)_data;
                  short* orig_ptr = old_data;

                  for (i = 0; i < data_length; i++)
                     *dest_ptr++ = LEFT(*orig_ptr++, 16);
               }

               // delete the old orig 16 bit data array
               delete [] old_data;
            }
            break;

            default:
            {
               cerr << "Software Error. Ilegal data size!" << endl;
            }
            break;
         }
      }
      break;
 
      case AFLIB_DATA_16U:
      {
         switch (data_size)
         {
            // Convert from 16U to 8S
            // Scale to 8-bit and toggle sign bit
            case AFLIB_DATA_8S:
            {
               if (_config.getDataEndian() != getHostEndian())
               {
                  short* orig_ptr = ( short *)_data;
                  char* dest_ptr = ( char *)orig_ptr;

                  for (i = 0; i < data_length; i++)
                  {
                     *orig_ptr = swab_short(*orig_ptr);
                     *dest_ptr++ = RIGHT(*orig_ptr++,8)^0x80;
                  }
               }
               else
               {
                  short* orig_ptr = ( short *)_data;
                  char* dest_ptr = ( char *)orig_ptr;

                  for (i = 0; i < data_length; i++)
                     *dest_ptr++ = RIGHT(*orig_ptr++,8)^0x80;
               }

               _config.setSampleSize(AFLIB_DATA_8S);
               _byte_inc = _config.getBitsPerSample() / 8;
               _total_length = _config.getChannels() * _length * _byte_inc;
            }
            break;

            // Convert from 16U to 8U
            // Scale to 8-bit
            case AFLIB_DATA_8U:
            {
               if (_config.getDataEndian() != getHostEndian())
               {
                  short* orig_ptr = ( short *)_data;
                  char* dest_ptr = ( char *)orig_ptr;

                  for (i = 0; i < data_length; i++)
                  {
                     *orig_ptr = swab_short(*orig_ptr);
                     *dest_ptr++ = RIGHT(*orig_ptr++,8);
                  }
               }
               else
               {
                  short* orig_ptr = ( short *)_data;
                  char* dest_ptr = ( char *)orig_ptr;

                  for (i = 0; i < data_length; i++)
                     *dest_ptr++ = RIGHT(*orig_ptr++,8);
               }

               _config.setSampleSize(AFLIB_DATA_8U);
               _byte_inc = _config.getBitsPerSample() / 8;
               _total_length = _config.getChannels() * _length * _byte_inc;
            }
            break;

            // Convert 16U to 16S
            // Toggle sign bit
            case AFLIB_DATA_16S:
            {
               if (_config.getDataEndian() != getHostEndian())
               {
                  short* orig_ptr = (short *)_data;

                  for (i = 0; i < data_length; i++)
                  {
                     *orig_ptr = swab_short(*orig_ptr);
                     *orig_ptr ^= 0x8000;
                     *orig_ptr = swab_short(*orig_ptr);
                     orig_ptr++;
                     // Maybe just this one will work faster
                     // *orig_ptr++ = ^ 0x0080;
                  }
               }
               else
               {
                  short* orig_ptr = (short *)_data;

                  for (i = 0; i < data_length; i++)
                     *orig_ptr++ ^= 0x8000;
               }

               _config.setSampleSize(AFLIB_DATA_16S);
            }
            break;

            // Convert 16U to 32S
            // Toggle sign bit
            case AFLIB_DATA_32S:
            {
               unsigned short * old_data = (unsigned short *)_data;

               // Configure new 32 bit data
               _config.setSampleSize(AFLIB_DATA_32S);
               _byte_inc = _config.getBitsPerSample() / 8;

               // allocate new array without destroying old data
               allocate();

               if (_config.getDataEndian() != getHostEndian())
               {
                  unsigned short * orig_ptr = old_data;
                  int * dest_ptr = (int *)_data;

                  for (i = 0; i < data_length; i++)
                  {
                     /* TBD no I need to swab orig_ptr too ? */
                     *dest_ptr = LEFT(*orig_ptr++, 16)^0x80000000 ;
                     *dest_ptr++ = swab_int(*dest_ptr);
                  }
               }
               else
               {
                  unsigned short * orig_ptr = old_data;
                  int * dest_ptr = (int *)_data;

                  for (i = 0; i < data_length; i++)
                     *dest_ptr++ = LEFT(*orig_ptr++, 16)^0x80000000 ;
               }

               // delete the old orig 16 bit data array
               delete [] old_data;
            }
            break;

            default:
            {
               cerr << "Software Error. Ilegal data size!" << endl;
            }
            break;
         }
      }
      break;

      case AFLIB_DATA_32S:
      {
         switch (data_size)
         {
            case AFLIB_DATA_8S:
            {
               // IF different endians then swap bytes
               if (_config.getDataEndian() != getHostEndian())
               {
                  {
                     int * orig_ptr = ( int *)_data;
                     char* dest_ptr = ( char *)orig_ptr;

                     for (i = 0; i < data_length; i++)
                     {
                        *orig_ptr = swab_int(*orig_ptr);
                        *dest_ptr++ = RIGHT(*orig_ptr++, 24);
                     }
                  }
               }
               else
               {
                  int* orig_ptr = ( int *)_data;
                  char* dest_ptr = ( char *)orig_ptr;

                  for (i = 0; i < data_length; i++)
                     *dest_ptr++ = RIGHT(*orig_ptr++, 24);
               }

               _config.setSampleSize(AFLIB_DATA_8S);
               _byte_inc = _config.getBitsPerSample() / 8;
               _total_length = _config.getChannels() * _length * _byte_inc;
            }
            break;

            case AFLIB_DATA_8U:
            {
               // IF different endians then swap bytes
               if (_config.getDataEndian() != getHostEndian())
               {
                  int* orig_ptr = ( int *)_data;
                  char* dest_ptr = ( char *)orig_ptr;

                  for (i = 0; i < data_length; i++)
                  {
                     *orig_ptr = swab_int(*orig_ptr);
                     *dest_ptr++ = RIGHT(*orig_ptr++, 24)^0x80;
                  }
               }
               else
               {
                  int* orig_ptr = ( int *)_data;
                  char* dest_ptr = ( char *)orig_ptr;

                  for (i = 0; i < data_length; i++)
                     *dest_ptr++ = RIGHT(*orig_ptr++, 24)^0x80;
               }

               _config.setSampleSize(AFLIB_DATA_8U);
               _byte_inc = _config.getBitsPerSample() / 8;
               _total_length = _config.getChannels() * _length * _byte_inc;
            }
            break;

            case AFLIB_DATA_16S:
            {
               // IF different endians then swap bytes
               if (_config.getDataEndian() != getHostEndian())
               {
                  {
                     int * orig_ptr = ( int *)_data;
                     short* dest_ptr = ( short *)orig_ptr;

                     for (i = 0; i < data_length; i++)
                     {
                        *orig_ptr = swab_int(*orig_ptr);
                        *dest_ptr++ = RIGHT(*orig_ptr++, 16);
                     }
                  }
               }
               else
               {
                  int* orig_ptr = ( int *)_data;
                  short* dest_ptr = ( short *)orig_ptr;

                  for (i = 0; i < data_length; i++)
                     *dest_ptr++ = RIGHT(*orig_ptr++, 16);
               }

               _config.setSampleSize(AFLIB_DATA_16S);
               _byte_inc = _config.getBitsPerSample() / 8;
               _total_length = _config.getChannels() * _length * _byte_inc;
            }
            break;

            case AFLIB_DATA_16U:
            {
               // IF different endians then swap bytes
               if (_config.getDataEndian() != getHostEndian())
               {
                  int* orig_ptr = ( int *)_data;
                  short* dest_ptr = ( short *)orig_ptr;

                  for (i = 0; i < data_length; i++)
                  {
                     *orig_ptr = swab_int(*orig_ptr);
                     *dest_ptr++ = RIGHT(*orig_ptr++, 16)^0x8000;
                  }
               }
               else
               {
                  int* orig_ptr = ( int *)_data;
                  short* dest_ptr = ( short *)orig_ptr;

                  for (i = 0; i < data_length; i++)
                     *dest_ptr++ = RIGHT(*orig_ptr++, 16)^0x8000;
               }

               _config.setSampleSize(AFLIB_DATA_16U);
               _byte_inc = _config.getBitsPerSample() / 8;
               _total_length = _config.getChannels() * _length * _byte_inc;
            }
            break;

            default:
            {
               cerr << "Software Error. Ilegal data size!" << endl;
            }
            break;
         }
      }
      break;
 
      default:
      {
         cerr << "Software Error. Ilegal data size!" << endl;
      }
      break;
   }
}


/*! \brief Converts data from one endian format to another.

    This function will convert the data in this object to a different
    endian layout. If the user passes the endian layout
    that the data is currently in then nothing will be done.
*/
void
aflibData::convertToEndian(aflib_data_endian endian)
{
   // IF already in correct format then just return
   if (_config.getDataEndian() == endian)
      return;

   switch(_config.getSampleSize())
   {
      // Endian does not affect this size
      case AFLIB_DATA_8S:
      case AFLIB_DATA_8U:
      {
      }
      break;

      case AFLIB_DATA_16S:
      case AFLIB_DATA_16U:
      {
         long long data_length, i;
         short* orig_ptr = (short *)_data;

         data_length = (long long)getLength() * (long long)_config.getChannels();

         for (i = 0; i < data_length; i++)
            *orig_ptr++ = swab_short(*orig_ptr);
      }
      break;

      case AFLIB_DATA_32S:
      {
         long long data_length, i;
         int * orig_ptr = (int *)_data;

         data_length = (long long)getLength() * (long long)_config.getChannels();

         for (i = 0; i < data_length; i++)
            *orig_ptr++ = swab_int(*orig_ptr);
      }

      default:
      {
         cerr << "Software Error. Ilegal data size!" << endl;
      }
      break;
   }

   if (endian == AFLIB_ENDIAN_LITTLE)
      _config.setDataEndian(AFLIB_ENDIAN_BIG);
   else
      _config.setDataEndian(AFLIB_ENDIAN_LITTLE);
}
