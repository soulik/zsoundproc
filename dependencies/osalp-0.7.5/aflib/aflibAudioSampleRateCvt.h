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


#ifndef _AFLIBAUDIOSAMPLERATECVT_H_
#define _AFLIBAUDIOSAMPLERATECVT_H_

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


#include "aflibAudio.h"
#include "aflibConverter.h"


/*! \class aflibAudioSampleRateCvt
    \brief Allows the aflibConverter class to be used in an audio chain.
 
   This class is a wrapper class around the audio rate converter class (ie
   aflibConverter). This class provides data to the converter class and keeps
   the audio data streaming. This is needed for file formats that don't have random
   access and for reading from devices. This class can use the aflibMemCache base
   class to perform audio caching. This class will provide sample rate conversion
   by virtually any factor as well as set the quality of the conversion. 
*/


class aflibAudioSampleRateCvt : public aflibAudio {

public:

   // Available contructors and destructors
   aflibAudioSampleRateCvt(
      aflibAudio& audio, 
      double factor,
      bool  linear_interpolation = FALSE,
      bool  high_quality = FALSE,
      bool  filter_interpolation = FALSE);

   ~aflibAudioSampleRateCvt();

   void
   setFactor(
      double factor,
      double volume = 1.0);

   void
   setInputConfig(const aflibConfig& cfg);

   //! Returns the name of the derived class.
   const char *
   getName() const { return "aflibAudioSampleRateCvt";};

   aflibStatus
   compute_segment(
      list<aflibData*>& data,
      long long position = -1) ;

   aflibData *
   process(
      aflibStatus& ret_status,
      long long position,
      int& num_samples,
      bool free_memory = TRUE) ;

   bool
   isDataSizeSupported(aflib_data_size size);

	bool
	isChannelsSupported(int& channels);

private:

   aflibAudioSampleRateCvt();

   aflibAudioSampleRateCvt(const aflibAudioSampleRateCvt& op);

   const aflibAudioSampleRateCvt&
   operator=(const aflibAudioSampleRateCvt& op);


int     _extra_sample;
int     _output_samples;
int     _requested_samples;
aflibConverter  * _converter;
double  _factor;
bool    _initial;
int     _init_chan;
short  *_in_array;
int     _in_array_size;
short  *_out_array;
int     _out_array_size;
double  _vol;

// These factors are used for buffering audio data so that we can get
// the audio data as a stream from the parent
long long _next_output_position;
long long _next_input_position;
int       _save_samples;
int       _prev_in_count;



};


#endif
