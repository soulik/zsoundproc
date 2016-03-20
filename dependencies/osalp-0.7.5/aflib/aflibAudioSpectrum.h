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


#ifndef _AFLIBAUDIOSPECTRUM_H_
#define _AFLIBAUDIOSPECTRUM_H_

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "aflibAudio.h"
#include "aflibFFT.h"

#define MAX_ARRAY_SIZE 1024

typedef void (*powermeter_callback)(double *);
typedef void (*spectrum_callback)(int, double *);

/*! \class aflibAudioSpectrum
    \brief Audio spectrum display and power meter class.

 This is a class that is derived from aflibAudio so that it can be put in a chain.
 It allows the user to display a power meter (ie signal strength) or a spectrum
 display. The number of elements in the spectrum display is changeable. The user
 can have either or both at the same time. It also provides the ability to change
 the number of updates that occur. It also supports more than one channel.
*/

class aflibAudioSpectrum : public aflibAudio {

public:

   // Available contructors and destructors
   aflibAudioSpectrum(aflibAudio& audio);
   ~aflibAudioSpectrum();

   bool
   setParameters(
      int num_samples,
      int resp_per_sec,
      int channels);

   void
   setPowerMeterCallback( powermeter_callback func_ptr);

   void
   setAudioSpectrumCallback( spectrum_callback func_ptr);

   void
   setInputConfig(const aflibConfig& cfg);

   aflibStatus
   compute_segment(
      list<aflibData *>& data,
      long long position = -1) ;

   //! Returns the name of the derived class.
   const char *
   getName() const { return "aflibAudioSpectrum";};

protected:
	
   virtual void
   setPowerMeter(double*);

   virtual void
   setAudioSpectrum(int, double*);

	bool _pm;
	bool _spectrum;
private:

   aflibAudioSpectrum();

   aflibAudioSpectrum(const aflibAudioSpectrum& op);

   const aflibAudioSpectrum&
   operator=(const aflibAudioSpectrum& op);

   void
   allocateMemory();

aflibFFT  _fft;
int    _num_samples;
int    _responses;
int    _channels;
int    _samples_between_responses;
int    _samples_counter;
double  * _in_real;
double  * _power_out;
double  * _spec_out;
powermeter_callback  _pm_func_ptr;
spectrum_callback  _spectrum_func_ptr;

};


#endif
