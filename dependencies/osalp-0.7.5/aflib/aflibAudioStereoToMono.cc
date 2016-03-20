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

#include "aflibAudioStereoToMono.h"

#include "aflibDebug.h"

/*! \brief Constructor for type of mixing operation.

   This constructor allows one to mix any input to a mono signal. 
	It will mix both channels (aflibAudioStereoToMono::AFLIB_MIX_BOTH), 
	or channel 1 (aflibAudioStereoToMono::AFLIB_MIX_CHAN1), 
	or channel 2 (aflibAudioStereoToMono::AFLIB_MIX_CHAN2).
*/
aflibAudioStereoToMono::aflibAudioStereoToMono(
   aflibAudio& audio,
   aflibAudioStereoToMono::aflib_mix_type mix,
	int amplitude) 
		: aflibAudioMixer(audio),_mix(AFLIB_MIX_UNDEF),_amplitude(0),_id(0)
{
	/* parent audio object was already added by aflibAudioMixer constructor so 
	 * this will just return its id. */
	_id = addParent(audio);

	/* this function will try to add the mixs and fail since mix is not set. */
	setAmplitude(amplitude);
	
	/* this function will try to add the mixs and succeed. */
   setMixType(mix);
}


/*! \brief Destructor.
*/
aflibAudioStereoToMono::~aflibAudioStereoToMono()
{
}

/*! \brief Sets the amplitude. 

   This function allows one to change the amplitude. 
	values between 1 and 100 are supported.

*/
void
aflibAudioStereoToMono::setAmplitude( int amplitude )
{
	_amplitude = amplitude;

	/* redo all the mixes */
	setMix();
}	

/*! \brief Sets the type of mixing to perform.

   This function allows one to change the type of mixing performed. 

	One can select both channels (aflibAudioStereoToMono::AFLIB_MIX_BOTH), 
	or channel 1 (aflibAudioStereoToMono::AFLIB_MIX_CHAN1), 
	or channel 2 (aflibAudioStereoToMono::AFLIB_MIX_CHAN2).
*/
void
aflibAudioStereoToMono::setMixType( aflibAudioStereoToMono::aflib_mix_type mix )
{
	_mix = mix;
	
	/* redo all the mixes */
	setMix();
}

void
aflibAudioStereoToMono::setMix()
{
	/* add the mixes based on the parent, mix type and amplitude */
	
   map<int, aflibAudio *, less<int> > parents;
   int   id;
	aflibConfig parent_cfg;

   // Get the ID number of the first input. We will only use this input
   parents = getParents();
	
	if(_mix == AFLIB_MIX_UNDEF)
	{
		aflib_debug("mix type hasn't been set yet");
		return;
	}

	if(_amplitude < 1)
	{
		aflib_debug("amplitude hasn't been set yet");
		return;
	}

	if(parents.size() == 1)
	{
	   id = (*parents.begin()).first;
		parent_cfg = ((*parents.begin()).second)->getOutputConfig();
	}
	else
	{
		aflib_debug("either no parents or too many parents");
		return;
	}

   // Delete all current mixing info.
   delAllMix();

   // Program mixing depending on state of input
   if (parent_cfg.getChannels() >= 2)
   {
      if (_mix == AFLIB_MIX_BOTH)
      { 
         addMix(id, 0, 0, _amplitude);
         addMix(id, 1, 0, _amplitude);
      }
      else if (_mix == AFLIB_MIX_CHAN1)
      {
         addMix(id, 0, 0, _amplitude);
      }
      else if (_mix == AFLIB_MIX_CHAN2)
      {
         addMix(id, 1, 0, _amplitude);
      }
   }
   else
   {
      // If only a single channel input then just output that channel
      addMix(id, 0, 0, _amplitude);
   }
	setNodeProcessed(FALSE);
}

void
aflibAudioStereoToMono::parentWasAdded(int parent_id)
{
	/* This class only supports one parent.  So we need to delete the old
	 * parent. 
	 *
	 * This allows a programmer to change the input to this class without 
	 * rebuilding it.
	 * 
	 * */
	
	if(_id != parent_id)
		removeParent(_id);
	
	/* Save our new parent_id */
	_id = parent_id;

	/* We'll have to create new mixes since the id has changed */
	setMix();
}
