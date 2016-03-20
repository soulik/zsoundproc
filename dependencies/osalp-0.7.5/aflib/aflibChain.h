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


#ifndef _AFLIBCHAIN_H
#define _AFLIBCHAIN_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <list>
#include <map>
#include <functional>
#include "aflib.h"
using std::list;
using std::map;
using std::less;

/*! \class aflibChain
    \brief Base class that implements chaining for audio objects.

     This is the base class that provides the mechanism to chain audio objects
     together. It maintains several static lists that contain all of the 
     chain information for this application. Derived class can override two
     functions that will inform them when a parent has been added or destroyed.
     Also provided in this base class are member functions to add and remove
     parents from an audio object. The ID for a parent that is added by using
     the constructor is 1. All others added with the addParent call will be 
     returned with this call.

     This class provides a default contructor for the start of a chain and
     a constructor that takes an aflibAudio object as the parent of this object. This
     class is also designed to be a base class for aflibAudio. This is so that all
     aflibAudio derived classes will inherit its functionality.
*/

class aflibAudio;
class aflibChainNode;


class aflibChain {

public:

   virtual
   ~aflibChain();

   const map<int, aflibAudio *, less<int> >&
   getParents() const;

   int
   addParent(aflibAudio& parent);

   void
   removeParent(int parent_id);

   void
   removeParent(aflibAudio& parent);

   bool
   getNodeProcessed() const;
 
   void
   setNodeProcessed(bool node);

   void
   dumpChain(bool check_env = TRUE);

protected:

   aflibChain();
   aflibChain(aflibAudio& parent);

   void
   replaceParent(
      aflibAudio& old_p,
      aflibAudio& new_p);

   // These are callback functions that derived classes can over ride to be
   // notified when a parent is destroyed or added. An Audio Editor object
   // for instance may have many inputs and if one is destroyed then it needs to
   // be notified so that it can rebuild its lists.
   virtual void
   parentWasDestroyed(int parent_id);

   virtual void
   parentWasAdded(int parent_id);

private:

   aflibChain(aflibChain& op);

   const aflibChain&
   operator=(const aflibChain& op);

   void
   insertIntoChain(
      aflibAudio& parent,
      aflibAudio& child);

   void
   insertIntoChain(
      aflibAudio& child);

   void
   removeFromChain(
      aflibAudio& child);


static list<aflibChainNode *>  _total_list;  
aflibChainNode *   _this_node;
bool               _processing_constructor;

};


#endif


