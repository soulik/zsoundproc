/*
This file was derived from the following work listed below. It
was modified for this class by Bruce W. Forsberg (forsberg@tns.net).

Modified from spkit software. Files
butterwo
bwbandpa
bwbandre
bwhighpa
bwlowpas


    Sound Processing Kit - A C++ Class Library for Audio Signal Processing
    Copyright (C) 1995-1998 Kai Lassfolk
 
    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.
 
    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.
 
    You should have received a copy of the GNU Library General Public
    License along with this library; if not, write to the Free
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/


#ifndef _AFLIBAUDIOBWFILTER_H_
#define _AFLIBAUDIOBWFILTER_H_

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "aflibAudio.h"


/*! \class aflibAudioBWFilter
    \brief Audio Butterworth filter class.

    This class provides support for band pass, band reject, low pass, and
    high pass filters using Butterworth filters. 
*/

class aflibAudioBWFilter: public aflibAudio {

public:

   enum AFLIB_BWFILTER_MODE
   {
      LOW_PASS,
      HIGH_PASS,
      BAND_PASS,
      BAND_REJECT
   };

   // Available contructors and destructors
   aflibAudioBWFilter(
      aflibAudio& audio,
      aflibAudioBWFilter::AFLIB_BWFILTER_MODE mode,
      double              freq,
      double              bw);

   ~aflibAudioBWFilter();

   void
   setParameters(
      aflibAudioBWFilter::AFLIB_BWFILTER_MODE mode,
      double              freq,
      double              bw);

   void
   setInputConfig(const aflibConfig& cfg);

   aflibStatus
   compute_segment(
      list<aflibData *>& data,
      long long position = -1) ;

   //! Returns the name of the derived class.
   const char *
   getName() const { return "aflibAudioBWFilter";};

   bool
   isDataSizeSupported(aflib_data_size size);

private:

   aflibAudioBWFilter();

   aflibAudioBWFilter(const aflibAudioBWFilter& op);

   const aflibAudioBWFilter&
   operator=(const aflibAudioBWFilter& op);

   void
   allocateMemory();

aflibAudioBWFilter::AFLIB_BWFILTER_MODE   _mode;
double                                    _freq;
double                                    _bw;
double                                    _C, _D;
double                                    _a[3], _b[2];
double                                   *_buf_x0;
double                                   *_buf_x1;
double                                   *_buf_y0;
double                                   *_buf_y1;
int                                       _chan;

};


#endif
