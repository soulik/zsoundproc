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


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


#include "aflibAudioMemoryInput.h"
#include "aflibData.h"

#include <list>


/*! \brief Constructor - Default constructor.
*/
aflibAudioMemoryInput::aflibAudioMemoryInput(const aflibConfig& config) : aflibAudio()
{
   _samples_func_ptr = NULL;

   // Set the configuration of the data
   setInputConfig(config);
}


/*! \brief Destructor
*/
aflibAudioMemoryInput::~aflibAudioMemoryInput()
{
}

/*! \brief Enables and disables the memory input callback.

   This function enables or disables the memory input callback. The callback
   registered must accept 5 parameters. The first is a pointer to this aflibAudio
   object. This allows the caller to know which object is generating this
   callback. This is so that if people have multiple objects all registering
   the same callback. The next parameter is the pointer to the memory location
   to store the data in. The next is the number of elements requested to be
   stored into the pointer. The next is the total number of bytes to store. The
   last parameter is the position of the audio data that is requested.

*/
void
aflibAudioMemoryInput::setAudioMemoryInputCallback( samples_callback func_ptr)
{
   _samples_func_ptr = func_ptr;
}


aflibStatus
aflibAudioMemoryInput::compute_segment(
   list<aflibData *>& data,
   long long position) 
{
   aflibStatus  status = AFLIB_SUCCESS;
   void * ptr;
   aflibData * p_data;
   long samples;

   if (_samples_func_ptr == NULL)
      status = AFLIB_END_OF_FILE;
   else
   {
      p_data = *data.begin();

      // Set the data layout of the data
      p_data->setConfig(getInputConfig());

      // Get a pointer to the data memory buffer
      ptr = p_data->getDataPointer();

      samples = _samples_func_ptr(this, ptr,
         p_data->getLength(), p_data->getTotalLength(), position);

      // IF we reached the end of the data then return error
      if (samples == 0)
      {
         status = AFLIB_END_OF_FILE;
         p_data->adjustLength(0);
      }
      // IF we read the last chunk
      else if (samples != p_data->getLength())
      {
         // When we only have a partial read then readjust the length of data
         p_data->adjustLength(samples);
      }
   }

   return (status);
}


