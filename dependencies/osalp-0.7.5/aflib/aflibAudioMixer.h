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


#ifndef _AFLIBAUDIOMIXER_H_
#define _AFLIBAUDIOMIXER_H_

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "aflibAudio.h"
#include "aflibMixerItem.h"
#include <set>
using std::set;

/*! \class aflibAudioMixer
    \brief Audio Mixing class.

 This class provides for mixing 1 or more inputs together to a single output. 
 
 It is derived from aflibAudio and thus can be used in a chain. It will mix any 
 number of channels, inputs to one output. The output can have multiple 
 channels. It will also allow one to adjust the amplitude of each input.  This 
 class will force each input to have the same characteristics such as sample 
 rate, data size and endian. If they are not then it will rely on the aflibAudio
 chain to automatically convert the data. The sample rate will be converted to 
 the highest value for all inputs. The data size will also be converted to the 
 largest size amoungst the inputs. The output will then be this selection of 
 values. The mixing is performed by mapping inputs to outputs. This is done with
 the addMix function.  Additional parents can be added by using the addParent 
 call in the aflibChain base class. The ID returned from this call should be 
 used for the addMix call. The parent passed in the constructor has an ID of 1. 
 If a parent is removed then all mixes that reference this input will be 
 removed.
*/ 


class aflibAudioMixer : public aflibAudio {

public:

   // Available contructors and destructors
   aflibAudioMixer(
      aflibAudio& audio);

   aflibAudioMixer();

   ~aflibAudioMixer();

   aflibStatus
   addMix(
      int  input,
      int  in_chan,
      int  out_chan,
      int  amp);

   aflibStatus
   delMix(
      int  input,
      int  in_chan,
      int  out_chan);

   void
   delAllMix();

   int
   getNumOfMixs();

   aflibStatus
   getMix(
      int   mix_num,
      int&  input,
      int&  in_chan,
      int&  out_chan,
      int&  amp);

   void
   setInputConfig(const aflibConfig& cfg);

   aflibStatus
   compute_segment(
      list<aflibData *>& data,
      long long position = -1) ;

   //! Returns the name of the derived class.
   const char *
   getName() const { return "aflibAudioMixer";};

   bool
   isDataSizeSupported(aflib_data_size size);
 
   bool
   isEndianSupported(aflib_data_endian end);
 
   bool
   isSampleRateSupported(int& rate);


private:

   aflibAudioMixer(const aflibAudioMixer& op);

   const aflibAudioMixer&
   operator=(const aflibAudioMixer& op);

   void
   parentWasDestroyed(int parent_id);
 

set < aflibMixerItem, less<aflibMixerItem> > _mix_item;

};

#endif
