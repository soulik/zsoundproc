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


#include "aflibChainNode.h"
#include "aflibAudio.h"

#include <stdio.h>
#include <stdlib.h>


/*! \brief This is the only constructor available and requires node it represents.
*/
aflibChainNode::aflibChainNode(aflibAudio& audio_item): _audio_item(audio_item)
{
   _id_counter = 0;
   _node_processed = FALSE;
}


/*! \brief Destructor.
*/
aflibChainNode::~aflibChainNode()
{
}


/*! \brief Adds a parent audio object to the list of parents for this node.

   Add a parent to this node . If the parent already exists then
   its ID will be returned. If it is not found then it will be added
   to the parent list and a unique ID returned.
*/
int
aflibChainNode::addParent(aflibAudio& parent_item)
{
   map<int, aflibAudio *, less<int> >::iterator it;


   _node_processed = FALSE;

   // Look to see if parent has already been added.
   for (it = _audio_parents.begin(); it != _audio_parents.end(); it++)
   {
      // IF parent is already in map then return ID
      if (&parent_item == (*it).second)
      {
         return((*it).first);
      }
   }

   _id_counter++;
   _audio_parents[_id_counter] = &parent_item;

   return(_id_counter);
}


/*! \brief Remove a parent from this node.
*/
void
aflibChainNode::removeParent(int parent_id)
{
   _audio_parents.erase(parent_id);
   _node_processed = FALSE;
}


/*! \brief Replaces an existing parent with a new parent.

    This will replace an existing parent with a new parent keeping the ID number
    the same for this node.
*/
void
aflibChainNode::replaceParent(
   aflibAudio& parent_item,
   int  parent_id)
{
   _audio_parents.erase(parent_id);
   _audio_parents[parent_id] = &parent_item;
   _node_processed = FALSE;
}


/*! \breify Returns the audio object that this node represents.
*/
aflibAudio&
aflibChainNode::getAudioItem()
{
   return (_audio_item);
}


/*! \breify Returns the parents of this audio object.

    This returns the parents for this audio object. It will return them in a map
    so that the caller will have the IDs and objects. They will be in ID assending
    order.
*/
map<int, aflibAudio *, less<int> >&
aflibChainNode::getParents() 
{
   return _audio_parents;
}


/*! \brief Returns if this node has been processed.

    This notifies the caller if the chain has been fully processed.
    This is useful so that the base classes can modify the chain if
    it needs to after it has been changed or modified and before any
    data it passed thru the chain. This is needed for things such as
    the sample rate converter class that can be inserted automattically
    if needed. If TRUE is returned then this node has not changed since
    it was processed last. If FALSE then this node has changed.
*/
bool
aflibChainNode::getNodeProcessed() const
{
   return (_node_processed);
}


/*! \breify Sets the processed state of this node.

    This allows the caller to set the state of the node. The state is set to
    FALSE internally in this class when the state is changed. The caller should
    set it to TRUE when the chain has been fully setup and processed.
*/
void
aflibChainNode::setNodeProcessed(bool node)
{
   _node_processed = node;
}


