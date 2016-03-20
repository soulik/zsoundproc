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

#include <iostream>
#include "aflibChain.h"
#include "aflibChainNode.h"

list<aflibChainNode *>  aflibChain::_total_list;

using std::cout;
using std::cerr;
using std::endl;


/*! \brief Constructor.
 
     This constructor is for derived classes that have no parent audio object.
*/
aflibChain::aflibChain()
{
   _processing_constructor = TRUE;
   _this_node = NULL;


   insertIntoChain(*((aflibAudio*)this));
   _processing_constructor = FALSE;
}


/*! \brief Constructor.
 
     This constructor is for derived classes that have a parent audio object.
*/
aflibChain::aflibChain(
   aflibAudio& parent)
{
   _processing_constructor = TRUE;
   _this_node = NULL;

   insertIntoChain(parent, *((aflibAudio*)this));
   _processing_constructor = FALSE;
}


/*! \brief Destructor.
*/
aflibChain::~aflibChain()
{
   removeFromChain(*((aflibAudio*)this));

   delete _this_node;
}


/*! \brief Returns parent objects and IDs for this audio object.

     This function will return the parents and IDs for this audio object. The order
     in the map will be in ID order.
*/
const map<int, aflibAudio *, less<int> >&
aflibChain::getParents() const
{
   if (_this_node == NULL)
   {
      cerr << "SOFTWARE ERROR: aflibChain::getParents()" << endl;
      exit(1);
   }
   return (_this_node->getParents());
}


void
aflibChain::insertIntoChain(
   aflibAudio& parent,
   aflibAudio& child)
{
   _this_node = new aflibChainNode(child);

   _this_node->addParent(parent);
   _total_list.push_back(_this_node);

   dumpChain();
}


void
aflibChain::insertIntoChain(
   aflibAudio& child)
{
  // Use this as a start node
   _this_node = new aflibChainNode(child);

   _total_list.push_back(_this_node);

   dumpChain();
}


void
aflibChain::removeFromChain(
   aflibAudio& child)
{
   // When we remove an object from a chain we need to assign the
   // parents of the deleted object to the next objects in the
   // chain so the chain will stay connected.

   list<aflibChainNode *>::iterator it;
   list<aflibChainNode *>::iterator it_to_delete =  _total_list.end();
   map<int, aflibAudio *, less<int> > new_parents;
   map<int, aflibAudio *, less<int> >::const_iterator it_parents;
   map<int, aflibAudio *, less<int> >::const_iterator it_p;

   // Find the node in the total node list and remove
   for (it = _total_list.begin(); it != _total_list.end(); it++)
   {
      if (&child == &((*it)->getAudioItem()))
      {
         it_to_delete = it;
         break;
      }
   }

   // IF we found item to delete then save its parents
   if (it_to_delete != _total_list.end())
   {
      new_parents = (*it_to_delete)->getParents();
   }

   // Find each audio object that the object that is to be deleted as a parent.
   // Add the parents of the deleted object to this object.
   for (it = _total_list.begin(); it != _total_list.end(); it++)
   {
      map<int, aflibAudio *, less<int> >& old_parents = (*it)->getParents();
      for (it_parents = old_parents.begin(); it_parents != old_parents.end(); it_parents++)
      {
         // IF object to be deleted is a parent for this object
         if ( &(*it_to_delete)->getAudioItem() == (*it_parents).second )
         {
            // notify audio object that parent was destroyed
            (((*it)->getAudioItem())).parentWasDestroyed((*it_parents).first);
            
            // Erase old entry
            (*it)->removeParent((*it_parents).first);

            // Add parents. We must use API so that a new ID gets assigned
            for (it_p = new_parents.begin(); it_p != new_parents.end(); it_p++)
            {
               (*it)->addParent(*(*it_p).second);
            }
            break;
         }
      }
   }

   if (it_to_delete != _total_list.end() )
   {
      _total_list.erase(it_to_delete);
   }
}


/*! \brief Adds a parent to an object in a chain.

    If an object wishes to add another item as a parent we will let them.
    Some objects need more than one input. This provides for it. An ID will be
    be returned to the user. This is how users should reference this input. If
    the parent is already attached to the child and in te list then its
    existing ID will be returned.
*/
int
aflibChain::addParent(aflibAudio& parent)
{
   int ret_id;

   if (_this_node == NULL)
   {
      cerr << "SOFTWARE ERROR: aflibChain::addParents()" << endl;
      exit(1);
   }

   ret_id = _this_node->addParent(parent);

   // If not called from the constructor then notify derived class that parent was added
   if (_processing_constructor == FALSE)
   {
      parentWasAdded(ret_id);
   }

   dumpChain();

   return(ret_id);
}


/*! \brief Removes a parent from an object in a chain referenced by ID.

    If an object wishes to remove a parent then we will let them. Some
    objects have more than one input and may want to remove one or more.
    NOTE: we are not talking about the node being deleted just the chain
    between this child and parent
*/
void
aflibChain::removeParent(int parent_id)
{
   if (_this_node == NULL)
   {
      cerr << "SOFTWARE ERROR: aflibChain::removeParents()" << endl;
      exit(1);
   }

   // notify audio object that parent was destroyed
   parentWasDestroyed(parent_id);
            
   _this_node->removeParent(parent_id);
   dumpChain();
}


/*! \brief Removes a parent from an object in a chain referenced by object.

    This is a convience function that will perform the same function as the
    removeParent function that takes a parent id. It will simply lookup the
    parent id and call that function.
*/
void
aflibChain::removeParent(aflibAudio& parent)
{
   const map<int, aflibAudio *, less<int> >& parent_list = getParents();
   map<int, aflibAudio *, less<int> >::const_iterator it_p;


   // Step thru all parents for this object
   for (it_p = parent_list.begin(); it_p != parent_list.end(); it_p++)
   {
      // IF object is a parent for this object
      if ( &parent == (*it_p).second )
      {
         // Erase entry
         removeParent((*it_p).first);
      }
   }
}


/*! \brief Replaces a parent with another parent keeping the same ID.

    This allows one to replace one parent with another parent while keeping the
    same ID. This is useful when inserting an object between two other objects but
    one does not want the audio object to consider this a new input. The aflibAudio
    base class uses this to insert a aflibAudioSampleRateCvt object in a chain
    without the child knowing it.
*/
void
aflibChain::replaceParent(
   aflibAudio& old_p,
   aflibAudio& new_p)
{
   const map<int, aflibAudio *, less<int> >& parent_list = getParents();
   map<int, aflibAudio *, less<int> >::const_iterator it_p;
 
 
   // Step thru all parents for this object
   for (it_p = parent_list.begin(); it_p != parent_list.end(); it_p++)
   {
      // IF object is a parent for this object
      if ( &old_p == (*it_p).second )
      {
         _this_node->replaceParent(new_p, (*it_p).first);
         break;
      }
   }
}


/*! \brief Notifies if this object has been fully processed in the chain.

    This notifies the caller if the chain has been fully processed.
    This is useful so that the base classes can modify the chain if
    it needs to after it has been changed or modified and before any
    data it passed thru the chain. This is needed for things such as
    the sample rate converter class that can be inserted automattically
    if needed. If TRUE is returned then this node has not changed since
    it was processed last. If FALSE then this node has changed.
*/
bool
aflibChain::getNodeProcessed() const
{
   return (_this_node->getNodeProcessed());
}

 
/*! \brief Sets the processed state for this object in the chain.

    This allows the caller to set the state of the node. The state is set in
    proper node class. It is initially set to FALSE in the node class.
    The caller should set it to TRUE when the chain has been fully setup
    and processed. Each node in the chain must be processed before data can
    be puled thru the chain.
*/
void
aflibChain::setNodeProcessed(bool node)
{
   _this_node->setNodeProcessed(node); 
}


/*! \brief Callback to notify derived class when parent was destroyed.

    This is a callback that derived classes override to get
    notified when a parent was removed from an object
*/
void
aflibChain::parentWasDestroyed(int parent_id) 
{
}


/*! \brief Callback to notify derived class when parent was added.

    This is a callback that derived classes override to get
    notified when a parent is added. It will not get called when
    a parent is set in the constructor.
*/
void
aflibChain::parentWasAdded(int parent_id)
{
}


/*! \brief Dumps the contents of audio chain.

    This function will dump the contents of the current chain.
    If TRUE or no input parameter then the env AFLIB_DUMP_CHAIN will
    be checked to see if it is set. If it is not then no output. If
    FALSE is passed in then the chain will be dumped to stdout if
    the env is set or not.
*/
void
aflibChain::dumpChain(bool check_env)
{
   list<aflibChainNode *>::iterator it;
   map<int, aflibAudio *, less<int> >::const_iterator it_p;

   // Don't output chain if env not set
   if ( check_env && (!getenv("AFLIB_DUMP_CHAIN")) )
      return;

   cout << endl << "------------ Start of Chain Dump ------------" << endl;
   // iterate thru each node
   for (it = _total_list.begin(); it != _total_list.end(); it++)
   {
      cout << "Audio object : " << &((*it)->getAudioItem());

      // We cant call getName here since this can get called from the constructor
      // which means the derived constructor has not been processed yet and that
      // means we cant access virtual functions yet such as getName.
      if ((&((*it)->getAudioItem()) == this) && (_processing_constructor == TRUE))
          cout << "  Name " << "Not Yet Constructed";
      else
          cout << "  Name " << (&((*it)->getAudioItem()))->getName();

      cout << "  State: " << (((*it)->getAudioItem()).getEnable() ? "Enabled":"Disabled") ;
      cout << endl;

      const map<int, aflibAudio *, less<int> >& o_p=(*it)->getParents();

      cout << "Number of parents are : " << o_p.size() << endl;
      for (it_p = o_p.begin(); it_p != o_p.end(); it_p++)
      {
          cout << "...Parent is " << (*it_p).second;
          if (((*it_p).second  == this) && (_processing_constructor == TRUE))
             cout << " Name " << "Not Yet Constructed";
          else
             cout << " Name " << (*it_p).second->getName();
          cout << " ID: " << (*it_p).first;
          cout << endl;
      }
   }

   cout << "------------ End of Chain Dump ------------" << endl << endl;
}


