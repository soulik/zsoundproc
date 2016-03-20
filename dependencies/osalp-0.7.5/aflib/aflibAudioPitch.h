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


#ifndef _AFLIBAUDIOPITCH_H_
#define _AFLIBAUDIOPITCH_H_

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "aflibAudioSampleRateCvt.h"


/*! \class aflibAudioPitch
    \brief Controls the pitch of an audio source.

   This class is derived from the aflibAudioSampleRateCvt class. It only overrides the
   setInputConfig and getName functions. This is so that it can keep the output and
   input sample rates the same even thought they are changing. This gives the
   illusion of pitch changing.
*/


class aflibAudioPitch : public aflibAudioSampleRateCvt {

public:

   // Available contructors and destructors
   aflibAudioPitch(
      aflibAudio& audio, 
      double factor,
      bool  linear_interpolation = FALSE,
      bool  high_quality = FALSE,
      bool  filter_interpolation = FALSE);

   ~aflibAudioPitch();

   void
   setInputConfig(const aflibConfig& cfg);

   //! Returns the name of the derived class.
   const char *
   getName() const { return "aflibAudioPitch";};

private:

   aflibAudioPitch();

   aflibAudioPitch(const aflibAudioPitch& op);

   const aflibAudioPitch&
   operator=(const aflibAudioPitch& op);

};

#endif
