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

/*! \class aflibBufferDev
    \brief Class that implements a buffer for devices that need one.

 Some audio devices do not provide buffering for audio data. The Solaris
audio device in record mode is one. For record mode one must read data in
blocks exactly. If the user requests a size different than the audio device
block size then we must read the audio device block size and buffer the
data and return the requested size from the buffer. This class provides
this buffer.

*/

#ifndef _AFLIBBUFFERDEV_H
#define _AFLIBBUFFERDEV_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


class aflibBufferDev {

public:

   aflibBufferDev();

   ~aflibBufferDev();

   bool
   init(
      int buf_size);

   //! Gets the total size of the buffer.
   inline
   int
   getBufferSize() const {return(_buf_size);};

   //! Gets the number bytes available to read from the buffer.
   inline
   int
   bytes_available()
   {
      int bytes_avail;

      if (_w_ptr >= _r_ptr)
         bytes_avail = _w_ptr - _r_ptr;
      else
         bytes_avail = _buf_size - (_r_ptr - _w_ptr);

      return(bytes_avail);
   };

   void
   write(
      unsigned char * buf,
      int             num_bytes);

   void
   read(
      unsigned char * buf,
      int             num_bytes);

private:

unsigned char *  _buf_ptr;
unsigned char *  _end_ptr;
unsigned char *  _w_ptr;
unsigned char *  _r_ptr;
int              _buf_size;


};


#endif
