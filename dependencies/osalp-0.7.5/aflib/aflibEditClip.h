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


#ifndef _AFLIBEDITCLIP_H_
#define _AFLIBEDITCLIP_H_


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


/*! \class aflibEditClip
    \brief Data class used by aflibAudioEdit.

    This stores an audio segment by the start and stop samples for the input and
    output. It will also compare two objects and determine which one is before the
    other.
*/

class aflibEditClip {

public:

   aflibEditClip();

   aflibEditClip(
      int  input,
      long long start_samples_o,
      long long stop_samples_o,
      long long start_samples_i,
      long long stop_samples_i,
      double    sample_rate_factor);

   ~aflibEditClip();

   bool
   operator < (const aflibEditClip& clip) const;

   void
   setInput(int input);

   int
   getInput() const;

   void
   setStartSamplesOutput(long long start_samples_o);

   long long
   getStartSamplesOutput() const;

   void
   setStopSamplesOutput(long long stop_samples_o);

   long long
   getStopSamplesOutput() const;

   void
   setStartSamplesInput(long long start_samples_i);

   long long
   getStartSamplesInput() const;

   void
   setStopSamplesInput(long long stop_samples_i);

   long long
   getStopSamplesInput() const;

   void
   setSampleRateFactor(double sample_rate_factor);

   double
   getSampleRateFactor() const;

   int
   compare(const aflibEditClip& clip);


private:

int       _input_id;
long long _output_start_samples;
long long _output_stop_samples;
long long _input_start_samples;
long long _input_stop_samples;
double    _sample_rate_factor;

};


#endif
