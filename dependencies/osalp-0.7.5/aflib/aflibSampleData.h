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


#ifndef _AFLIBSAMPLEDATA_H_
#define _AFLIBSAMPLEDATA_H_


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

class aflibAudio;

class aflibSampleData {

public:

   aflibSampleData(const aflibAudio * file);

   ~aflibSampleData();

   void
   setOutputRange(
      int  min,
      int  max);

   void
   setMaxMinMode(bool mode);

   void
   setNumSamplePoints(
      unsigned int points);

   void
   setParametersSamples(
      long long start_sample,
      long long stop_sample);

   void
   setParametersSeconds(
      double start_seconds,
      double stop_seconds);

   void
   setChannels(
      int num_channels,
      int * array);

   int
   getDelta() const;

   bool
   process(
      int * array);

private:

   aflibSampleData();

   aflibSampleData(const aflibSampleData& op);

   const aflibSampleData&
   operator=(const aflibSampleData& op);


const aflibAudio * _file;
int *         _channel_array;

bool          _minmax_mode;
int           _output_max;
int           _output_min;
int           _a0;
int           _a1;
int           _delta;
unsigned int  _num_channels;
unsigned int  _num_points;
long long     _start_sample;
long long     _stop_sample;

};


#endif
