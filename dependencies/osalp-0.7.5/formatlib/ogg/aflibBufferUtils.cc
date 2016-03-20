/*
 * Copyright: (C) 2000-2002 Bruce W. Forsberg
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
#include <iostream.h>


#include "aflibBufferUtils.h"
#include "aflib.h"


aflibBufferUtils::aflibBufferUtils()
{
   _buf_ptr = NULL;
   _buf_size = 0;
}

aflibBufferUtils::~aflibBufferUtils()
{
   if (_buf_ptr != NULL)
   {
      delete [] _buf_ptr;
   }
}

/*! \brief Initializes the buffer class.

   This will initialize the buffer class with a size of buf_size. Subsequent calls to
   this function will free any previous memory allocated with a call to init.
*/
bool
aflibBufferUtils::init(
   int buf_size)
{
   bool init_flag = TRUE;


   if (_buf_ptr)
   {
      delete [] _buf_ptr;
      _buf_ptr = NULL;
   }

   _buf_size = buf_size;
   _buf_ptr = new unsigned char[buf_size];
   _w_ptr = _buf_ptr;
   _r_ptr = _buf_ptr;
   _end_ptr = _buf_ptr + buf_size;

   if (_buf_ptr == NULL)
      init_flag = FALSE;

   return (init_flag);
}


void
aflibBufferUtils::write(
   unsigned char * buf,
   int             num_bytes)
{
   int bytes_till_end;



   bytes_till_end = _end_ptr - _w_ptr;

   // TBD what about data overruns
   // Do we need to wrap data around the start
   if (num_bytes > bytes_till_end)
   {
      memcpy(_w_ptr, buf, bytes_till_end);
      memcpy(_buf_ptr, buf + bytes_till_end, num_bytes - bytes_till_end);
      _w_ptr = _buf_ptr + num_bytes - bytes_till_end;
   }
   else
   {
      memcpy(_w_ptr, buf, num_bytes);
      _w_ptr += num_bytes;
   }
}


void
aflibBufferUtils::read(
   unsigned char * buf,
   int             num_bytes)
{
   int bytes_till_end;


   bytes_till_end = _end_ptr - _r_ptr;

   if (num_bytes > bytes_till_end)
   {
      memcpy(buf, _r_ptr, bytes_till_end);
      memcpy(buf + bytes_till_end, _buf_ptr, num_bytes - bytes_till_end);
      _r_ptr = _buf_ptr + num_bytes - bytes_till_end;
   }
   else
   {
      memcpy(buf, _r_ptr, num_bytes);
      _r_ptr += num_bytes;
   }
}


