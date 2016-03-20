/*
 * Copyright: (C) 2001 Bruce W. Forsberg
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


#ifndef _AFLIBAUDIOMEMORYINPUT_H_
#define _AFLIBAUDIOMEMORYINPUT_H_

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "aflibAudio.h"


typedef long (*samples_callback)(aflibAudio *, void *, long, long, long long);


/*! \class aflibAudioMemoryInput
    \brief Provides ability to insert raw audio data into audio chain.

   This class allows one to insert raw data into an audio chain at the
   start of the chain. This class can't be used anywhere in a chain other
   than at the start. Once an audio chain is formed then the caller can call
   the process member function of the last item in the chain and retrieve the
   aflibData object to get the raw audio data out of the chain. 
   Users will call the process member function at the end of an audio chain.
   This will result in data being requested down the chain until this class
   is reached. Then the samples_callback will be called requesting audio data
   from the user. Users of this class should call the setAudioMemoryInputCallback
   function to register a callback function that will provide data to the chain.
   This callback function shall have 5 parameters and shall return the actual
   number of data stored. Since this object is at the beginning of the audio
   chain then in order for data to be inserted into the chain the caller must
   register a callback function before calling the process member function
   of the chain. The 5 parameters are: \n
     aflibAudio * - the aflibAudioMemoryInput object that is making this call. \n
     void *       - pointer to the audio data memory location. \n
     long         - number of samples to read. \n
     long         - total length of samples (ie samples above * size of each sample). \n
     long long    - position in data stream that is requested. \n

   The last parameter is the position of the data to read from. The user can request
   to start reading anywhere they wish. Thus they can start 10000 samples from the
   beginning of the audio data. If the user is streaming data then the library will
   make every attempt to request data sequentially from this class, but this is not
   guaranteed.

*/


class aflibAudioMemoryInput: public aflibAudio {

public:

   // Available contructors and destructors
   aflibAudioMemoryInput(const aflibConfig& config);

   ~aflibAudioMemoryInput();

   void
   setAudioMemoryInputCallback( samples_callback func_ptr);

   aflibStatus
   compute_segment(
      list<aflibData *>& data,
      long long position = -1) ;

   //! Returns the name of the derived class.
   const char *
   getName() const { return "aflibAudioMemoryInput";};

private:

   aflibAudioMemoryInput();
   aflibAudioMemoryInput(const aflibAudioMemoryInput& op);

   const aflibAudioMemoryInput&
   operator=(const aflibAudioMemoryInput& op);

samples_callback  _samples_func_ptr;

};

#endif
