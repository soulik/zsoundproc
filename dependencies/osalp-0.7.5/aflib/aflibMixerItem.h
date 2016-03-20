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


#ifndef _AFLIBMIXERITEM_H_
#define _AFLIBMIXERITEM_H_


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


/*! \class aflibMixerItem
    \brief Data class used by aflibAudioMixer class.
*/

class aflibMixerItem {

public:

   aflibMixerItem();

   aflibMixerItem(
      int  input,
      int  in_chan,
      int  out_chan,
      int  amp);

   ~aflibMixerItem();

   void
   setInput(int input);

   int
   getInput() const;

   void
   setInChannel(int in_chan);

   int
   getInChannel() const;

   void
   setOutChannel(int out_chan);

   int
   getOutChannel() const;

   void
   setAmplitude(int amp);

   int
   getAmplitude() const;

   bool
   operator < (const aflibMixerItem& item) const;

private:

int   _input;
int   _in_chan;
int   _out_chan;
int   _amp;

};


#endif
