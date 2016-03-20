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


#ifndef _AFLIBCHAINNODE_H
#define _AFLIBCHAINNODE_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif



#include <map>
#include "aflib.h"
#include "aflibAudio.h"

/*! \class aflibChainNode
    \brief Data class used by aflibChain base class

    This class contains all of the information about a node in the Chain. It
    keeps track of all the parents of a node in the chain. It also keeps track if
    a node has been processed in a chain. All nodes in a chain must be processed
    before data can flow thru the chain. This class is designed to be used by the
    aflibChain class only.
*/

class aflibChainNode {

public:

   aflibChainNode(aflibAudio& audio_item);
   ~aflibChainNode();

   int
   addParent(aflibAudio& parent_item);

   void
   removeParent(int parent_id);

   void
   replaceParent(
      aflibAudio& parent_item,
      int  parent_id);

   aflibAudio&
   getAudioItem();

   map<int, aflibAudio *, less<int> >&
   getParents() ;

   bool
   getNodeProcessed() const;

   void
   setNodeProcessed(bool node);


protected:

private:

   aflibChainNode();


   aflibAudio&  _audio_item;
   map<int, aflibAudio *, less<int> > _audio_parents;
   int  _id_counter;
   bool _node_processed;

};


#endif
