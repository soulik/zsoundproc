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

#ifndef _AFLIBAUDIO_H
#define _AFLIBAUDIO_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "aflib.h"
#include "aflibChain.h"
#include "aflibMemCache.h"
#include "aflibConfig.h"

#include <list>


/*! \class aflibAudio
    \brief Main base class for all audio objects to be placed in a chain.
 
     This is the Base class that all objects will use that want to be used
     as audio objects with this library. It inherts functionality from the
     aflibChain class so that these objects can be used in a chain. It is also
     derived from aflibMemCache class. This provides memory caching for an
     object.

     Two constructors are provided. A constructor with no parameters is
     provided that derived classes should call that can be used as the
     start of a chain. A mixing class for instance makes no sense to
     have a default contructor since it can't be used as the start of a
     chain. Most often the start of a chain will be a file or device
     object. The second constructor takes an aflibAudio object. This should
     be called by a derived class that could have a parent audio object
     feeding it. This will more than likely be almost all objects.

     Function compute_segment is pure virtual so that all derived classes will
     have to implement it. There is no implementation in this base class.
     This is the function that derived classes implement that will do the
     work. This function will be passed a list of aflibData objects. One
     for each parent of the object. The order in the list is the same as
     the order from the getParents function. It will also be passed the position
     of the data. When done the function should return AFLIB_SUCCESS if
     success.

     Function process is defined in this base class and should not be overriden except
     in certain circumstances. Such as an audio class with more than 1 input
     that needs special requirements. It provides the functionality to push
     data throught the chain. The user application can call process from any
     element in the chain. It will work its way back through the chain and
     when it gets to the start will start an aflibData object through the
     chain until arriving at and processing this object. This function might
     be the ideal place in the future to enable threading. 
*/

class aflibData;
class aflibAudioSampleRateCvt;
class aflibAudioMixer;


class aflibAudio : public aflibChain, public aflibMemCache {

public:

   virtual
   ~aflibAudio();

   void
   enable(bool enable);

   bool
   getEnable() const;

   void
   enableDataSizeConversion(bool enable);

   void
   enableEndianConversion(bool enable);

   void
   enableSampleRateConversion(bool enable);

   void
   enableChannelsConversion(bool enable);

   virtual void
   setInputConfig(const aflibConfig& cfg);

   virtual const aflibConfig&
   getInputConfig() const;

   virtual void
   setOutputConfig(const aflibConfig& cfg);

   virtual const aflibConfig&
   getOutputConfig() const;

   //! Returns the name of the derived class, must be implemented by derived class.
   virtual const char *
   getName() const { return "aflibAudio";};

   //! Work function, must be implemented by derived class.
   virtual aflibStatus
   compute_segment(
      list<aflibData *>& ,
      long long ){ return AFLIB_SUCCESS;};

   virtual aflibData *
   process(
      aflibStatus& ret_status,
      long long position,
      int& num_samples,
      bool free_memory = TRUE);

protected:

      aflibAudio();
      aflibAudio(aflibAudio& audio);

   aflibAudio* 
   convertSampleRate(aflibAudio&  parent);

   aflibAudio* 
   convertChannels(aflibAudio&  parent);

   void
   convertData(aflibData& data);

   void
   examineChain();

      bool
   checkChain(const aflibAudio * audio) const;

   void
   preprocessChain(
      aflibAudio * audio,
      aflibAudio * child);

   // "is" functions that indicate what formats an object supports
   virtual bool
   isDataSizeSupported(aflib_data_size size);

   virtual bool
   isEndianSupported(aflib_data_endian end);

   virtual bool
   isSampleRateSupported(int& rate);

   virtual bool
   isChannelsSupported(int& channels);

   inline void
   incrementLevel() {_level++;};

   inline void
   decrementLevel() {_level--;};

private:

bool     _enable;
bool     _enable_data_size;
bool     _enable_endian;
bool     _enable_sample_rate;
bool     _enable_channels;
aflibConfig _cfg_input;
aflibConfig _cfg_output;
aflibAudioSampleRateCvt  * _cvt;
aflibAudioMixer  * _mix;
static int  _level;

};


#endif
