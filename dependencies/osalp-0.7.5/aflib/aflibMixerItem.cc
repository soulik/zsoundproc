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


#include "aflibMixerItem.h"

aflibMixerItem::aflibMixerItem()
{
}

aflibMixerItem::aflibMixerItem(
   int  input,
   int  in_chan,
   int  out_chan,
   int  amp)
{
   _input = input;
   _in_chan = in_chan;
   _out_chan = out_chan;
   _amp = amp;
}

bool
aflibMixerItem::operator < (const aflibMixerItem& item) const
{
   // This will sort the set by output channel, input, and input channel. 
	// This will make it easier to process the list.

   bool less_flag = false;

   if (getOutChannel() < item.getOutChannel())
   {
      less_flag = true;
   }
   else if (getOutChannel() == item.getOutChannel())
   {
      if (getInput() < item.getInput())
      {
         less_flag = true;
      }
      else if (getInput() == item.getInput())
      {
         if (getInChannel() < item.getInChannel())
         {
            less_flag = true;
         }
      }
   }

   return (less_flag);
}

aflibMixerItem::~aflibMixerItem()
{
}

void
aflibMixerItem::setInput(int input)
{
   _input = input;
}

int
aflibMixerItem::getInput() const
{
   return(_input);
}
 
void
aflibMixerItem::setInChannel(int in_chan)
{
   _in_chan = in_chan;
}

int
aflibMixerItem::getInChannel() const
{
   return(_in_chan);
}
 
void
aflibMixerItem::setOutChannel(int out_chan)
{
   _out_chan = out_chan;
}
 
int
aflibMixerItem::getOutChannel() const
{
   return(_out_chan);
}
 
void
aflibMixerItem::setAmplitude(int amp)
{
   _amp = amp;
}
 
int
aflibMixerItem::getAmplitude() const
{
   return (_amp);
}
