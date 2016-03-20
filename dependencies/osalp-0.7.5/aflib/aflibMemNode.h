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


#ifndef _AFLIBMEMNODE_H_
#define _AFLIBMEMNODE_H_


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <vector>
using std::vector;

/*! \class aflibMemNode
    \brief Data class used by aflibMemCache class.
*/

class aflibMemNode {

public:

   aflibMemNode();

   ~aflibMemNode();

   vector<int>&
   getData() {return _data;};

   void
   setChannels(int chan) {_chan = chan;};

   int
   getChannels() const {return _chan;};

   int
   getSize() const;

private:

   aflibMemNode(const aflibMemNode& op);
 
   const aflibMemNode&
   operator=(const aflibMemNode& op);

vector<int> _data;
int         _chan;

};


#endif
