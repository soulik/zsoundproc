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


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


#include "aflibAudioPitch.h"


/*! \brief Constructor.

    This constructor overides the aflibAudioSampleRateCvt base class constructor.
    It provides the ability to set the quality of the sample rate conversion and
    the sample rate conversion factor. For more information on these see the
    documentation for the aflibAudioSampleRateCvt constructor.
*/
aflibAudioPitch::aflibAudioPitch(
   aflibAudio& audio,
   double factor,
   bool  linear_interpolation,
   bool  high_quality,
   bool  filter_interpolation) : aflibAudioSampleRateCvt(
      audio, factor, linear_interpolation, high_quality, filter_interpolation)
{
}

/*! \brief Destructor.
*/
aflibAudioPitch::~aflibAudioPitch()
{
}


/*! \brief Sets the input and output sample rates of this class.
 
   This function overrides the aflibAudioSampleRateCvt base class function.
   It will NOT change the output sample rate in the output audio configuration. By
   keeping the input and output sample rates the same even thought they are
   changing this gives the illusion of pitch changing.
*/
void
aflibAudioPitch::setInputConfig(const aflibConfig& cfg)
{
   // call the base classes setInputConfig to store input configuration
   aflibAudio::setInputConfig(cfg);
   // Store output configuration
   setOutputConfig(cfg);
}

