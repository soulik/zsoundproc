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


#ifndef _AFLIBAUDIOSTEREOTOMONO_H
#define _AFLIBAUDIOSTEREOTOMONO_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "aflib.h"
#include "aflibAudioMixer.h"


/*! \class aflibAudioStereoToMono
    \brief Implements mixing a stereo or mono signal to a mono signal.

  This class implements converting a stereo or mono signal into a mono signal. 
  It will output either a mix of both channels 1 and 2 inputs, channel 1, or 
  channel 2. 
  
  This class is a convience class derived from aflibAudioMixer since this is a 
  commonly used operation. It takes either a mono or stereo signal so that the 
  programmer does not need to worry about the source.
*/


class aflibAudioStereoToMono : public aflibAudioMixer {

public:

   enum aflib_mix_type
   {
      AFLIB_MIX_UNDEF,
      AFLIB_MIX_BOTH,
      AFLIB_MIX_CHAN1,
      AFLIB_MIX_CHAN2
   };

   // Constructor for a node with a parent
   aflibAudioStereoToMono(
      aflibAudio& audio,
      aflibAudioStereoToMono::aflib_mix_type mix = AFLIB_MIX_BOTH,
		int amplitude = 100);

   ~aflibAudioStereoToMono();

   void
   setMixType(aflibAudioStereoToMono::aflib_mix_type mix);
	
   void
   setAmplitude(int amplitude);

   const char *
   getName() const { return "aflibAudioStereoToMono";};

private:

   aflibAudioStereoToMono();

   aflibAudioStereoToMono(const aflibAudioStereoToMono& op);

   const aflibAudioStereoToMono&
   operator=(const aflibAudioStereoToMono& op);

	void
	setMix();

   void
   parentWasAdded(int parent_id);
	
   aflib_mix_type  _mix;
	int _amplitude;
	int _id;
};

#endif
