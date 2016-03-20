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


#ifndef _AFLIBAUDIOCONSTANTSRC_H_
#define _AFLIBAUDIOCONSTANTSRC_H_

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "aflibAudio.h"


/*! \class aflibAudioConstantSrc
    \brief Inserts a constant audio source in a chain for test purposes

 This class allows one to insert a test signal anywhere in an audio chain.
 It is mainly used for debugging other audio objects that work on audio 
 data. When it is inserted in a chain it takes on the characteristics of
 everything before it in the chain. For instance a parent that is outputting
 a 22050 sample rate mono signal will cause this object to throw away the
 data from the parent and instead insert a test signal that is 22050 mono.
 
 One can select a DC level, triangle wave, square wave, or sine wave. One can
 also adjust the frequency and amplitude. 
*/


class aflibAudioConstantSrc: public aflibAudio {

public:

enum waveform_type
{
   WAVEFORM_DCLEVEL,
   WAVEFORM_TRIANGLE,
   WAVEFORM_SQUAREWAVE,
   WAVEFORM_SINEWAVE
};

   // Available contructors and destructors
   aflibAudioConstantSrc(aflibAudio& audio);
   aflibAudioConstantSrc();

   ~aflibAudioConstantSrc();

   void
   setWaveformType(
      aflibAudioConstantSrc::waveform_type wave,
      double  max_amp,
      double  min_amp,
      int  frequency);

   aflibStatus
   compute_segment(
      list<aflibData *>& data,
      long long position = -1) ;

   //! Returns the name of the derived class.
   const char *
   getName() const { return "aflibAudioConstantSrc";};

private:


   aflibAudioConstantSrc(const aflibAudioConstantSrc& op);

   const aflibAudioConstantSrc&
   operator=(const aflibAudioConstantSrc& op);

waveform_type  _wave;
double         _amp_max;
double         _amp_min;
int            _freq;

};


#endif
