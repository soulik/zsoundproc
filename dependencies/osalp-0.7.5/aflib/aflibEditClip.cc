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


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


#include <stdio.h>
#include <iostream>

#include "aflibEditClip.h"


/*! \brief Constructor.
*/
aflibEditClip::aflibEditClip()
{
   _input_id = 0;
   _output_start_samples = 0;
   _output_stop_samples = 0;
   _input_start_samples = 0;
   _input_stop_samples = 0;
   _sample_rate_factor = 1.0;
}


/*! \brief Constructor with parameters.
*/
aflibEditClip::aflibEditClip(
      int  input,
      long long start_samples_o,
      long long stop_samples_o,
      long long start_samples_i,
      long long stop_samples_i,
      double    sample_rate_factor) :
   _input_id(input),
   _output_start_samples(start_samples_o),
   _output_stop_samples(stop_samples_o),
   _input_start_samples(start_samples_i),
   _input_stop_samples(stop_samples_i),
   _sample_rate_factor(sample_rate_factor)
{
}


/*! \brief Destructor.
*/
aflibEditClip::~aflibEditClip()
{
}


/*! \brief Operator to determine what clip is before another in output space.
*/
bool
aflibEditClip::operator < (const aflibEditClip& clip) const
{
   return (getStartSamplesOutput() < clip.getStartSamplesOutput());
}


/*! \brief Set the input that a clip uses.
*/
void
aflibEditClip::setInput(int input)
{
   _input_id = input;
}

   
/*! \brief Get the input that a clip uses.
*/
int
aflibEditClip::getInput() const
{
   return(_input_id);
}


/*! \brief Sets the start samples location for the output.
*/
void
aflibEditClip::setStartSamplesOutput(long long start_samples_o)
{
   _output_start_samples = start_samples_o;
}

   
/*! \brief Gets the start samples location for the output.
*/
long long
aflibEditClip::getStartSamplesOutput() const
{
   return (_output_start_samples);
}

   
/*! \brief Sets the stop samples location for the output.
*/
void
aflibEditClip::setStopSamplesOutput(long long stop_samples_o)
{
   _output_stop_samples = stop_samples_o;
}

   
/*! \brief Gets the stop samples location for the output.
*/
long long
aflibEditClip::getStopSamplesOutput() const
{
   return (_output_stop_samples);
}

   
/*! \brief Sets the start samples location for the input.
*/
void
aflibEditClip::setStartSamplesInput(long long start_samples_i)
{
   _input_start_samples = start_samples_i;
}

   
/*! \brief Gets the start samples location for the input.
*/
long long
aflibEditClip::getStartSamplesInput() const
{
   return (_input_start_samples);
}


/*! \brief Sets the stop samples location for the input.
*/
void
aflibEditClip::setStopSamplesInput(long long stop_samples_i)
{
   _input_stop_samples = stop_samples_i;
}


/*! \brief Gets the stop samples location for the input.
*/
long long
aflibEditClip::getStopSamplesInput() const
{
   return (_input_stop_samples);
}


/*! \brief Sets the sample rate conversion factor. 

    If an input is at 22050 Hz and the output will be 44100 then the
    sample rate conversion factor will be 2.0. Thus there will be twice
    as many samples in the output as are in the input.
*/
void
aflibEditClip::setSampleRateFactor(double sample_rate_factor)
{
   _sample_rate_factor = sample_rate_factor;
}


/*! \brief Gets the sample rate conversion factor.
*/
double
aflibEditClip::getSampleRateFactor() const
{
   return (_sample_rate_factor);
}


/*! \brief Compares two date / time objects.

    This function will compare an audio clip with this one. It will return
    one of three possible values. 
    -1 : "this" clip is before "clip" clip sequentially. 
     0 : "this" starts within "clip" clip
     1 : "this" clip is after "clip" clip
*/
int
aflibEditClip::compare(const aflibEditClip& clip)
{
   int ret_value;

   if (getStartSamplesOutput() <= clip.getStartSamplesOutput())
      ret_value = -1;
   else if (getStartSamplesOutput() < clip.getStopSamplesOutput())
      ret_value = 0;
   else
      ret_value = 1;

   return(ret_value);
}


