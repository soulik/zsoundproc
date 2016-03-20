/*
 * Copyright: (C) 2001 Bruce W. Forsberg
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


#include "aflibMemCache.h"
#include "aflibMemNode.h"
#include "aflib.h"
#include "aflibData.h"

#define CACHE_SIZE_MAX 1 * 1024 * 1024  // Max cache size total

long long aflibMemCache::_cache_size_total = 0;


/*! \brief Constructor.
*/
aflibMemCache::aflibMemCache()
{
   _cache_counter = 0;
   _cache_size_local = 0;
   _enable = FALSE;
   if (getenv("AFLIB_CACHE_SIZE"))
      _cache_size_max = atoi(getenv("AFLIB_CACHE_SIZE"));
   else
      _cache_size_max = CACHE_SIZE_MAX;
}


/*! \brief Destructor.
*/
aflibMemCache::~aflibMemCache()
{
   clearCache();
}


/*! \brief Returns the current state of caching for this object.

    Return the state of the cache. TRUE means caching is enabled.
*/
bool
aflibMemCache::getCacheEnable()
{
   return(_enable);
}


/*! \brief Set the state of caching for this object.
*/
void
aflibMemCache::setCacheEnable(bool enable)
{
   _enable = enable;
}


/*! \brief Returns the maximum allowed size of the cache.
*/
long long
aflibMemCache::getCacheMax() const
{
   return (_cache_size_max);
}


/*! \brief Gets the current size of the cache for this object only.
*/ 
long long
aflibMemCache::getCacheLocal() const
{
   return (_cache_size_local);
}


/*! \brief Gets the total size of all caches added together.
*/
long long
aflibMemCache::getCacheTotal() const
{
   return (_cache_size_total);
}


void
aflibMemCache::cacheData(
   long long position,
   aflibData& data)
{
   // This function will receive a block of data to be cached. This function will store the
   // data into the cache so that it can be read later. It will first of all check the
   // cache size. If the cache limit has been exceeded then old data will be discarded.


   // IF we have exceeded the max cache size then reduce the cache size
   if (_cache_size_local > _cache_size_max)
   {
      reduceCache();
   }

   // Check if the data can replace an existing nodes data
   if (!checkExistingNode(position, data))
   {
      // Create new node and insert into array
      createNewNode(0, data.getLength(), position, data);
   }
}

 
void
aflibMemCache::lookupData(
   long long& position,
   int& num_samples)
{
   // This function will determine if there is data available in the cache that can be
   // used. It will modify the position and num_samples passed in to indicate the data
   // that is still needed that is not in the cache. If all the needed data is in the
   // cache then this function will return num_samples equal to 0.

	if(position == -1) return;
   map< long long, aflibMemNode* >::iterator it;
   map< long long, aflibMemNode* >::iterator it_prev;
   bool processed = FALSE;


   // loop thru all memory nodes
   for (it = _node_array.begin(); it != _node_array.end(); it++)
   {
      // Loop until we find tbe memory node past the start of this memory location
      if ((*it).first > position)
      {
         processed = TRUE;

         // If no node before this position then no data in cache for this position
         if (it == _node_array.begin())
         {
            break;
         }

         // IF data is not in node then done
         if (position > (*it_prev).first + (long long)(*it_prev).second->getSize())
         {
            break;
         }

         // Use the previous node to modify data
         calcPosition(position, num_samples, it_prev);
         break;
      }
      it_prev = it;
   }

   // Check the last node if none others were found
   if ((_node_array.size() != 0) && processed == FALSE)
   {
      calcPosition(position, num_samples, it_prev);
   }
}


void
aflibMemCache::calcPosition(
   long long& position,
   int&       num_samples,
   map< long long, aflibMemNode* >::iterator it)
{
   // Using the memory node passed in modify the position and num_samples based on
   // how much data can be retrieved from this node


   int  diff;

   // Find the amount of data that might be available
   diff = (int) ( (*it).first + (*it).second->getSize() - position );
 
   // IF there is data that we can use
   if (diff > 0)
   {
      // IF all data is available
      if (diff >= num_samples)
      {
         position += num_samples;
         num_samples = 0;
      }
      else
      {
         position += diff;
         num_samples -= diff;
      }
   }
}


void
aflibMemCache::reduceCache()
{
   // One or more nodes should be removed to keep memory usage in check.
   // Currently this uses a very simple algorithm. It deletes the nodes with
   // the smallest positions. This needs to change in the future.


   long long size;
   map< long long, aflibMemNode* >::iterator it;

   // Delete cache nodes until local cache is less than max
   while (_cache_size_local > _cache_size_max)
   {
      it = _node_array.begin();
      // reduce total cache size of node
      size = ((*it).second->getSize() * (*it).second->getChannels() * 4);
      _cache_size_local -= size;
      _cache_size_total -= size;

      // Remove node
      delete (*it).second;
      _node_array.erase(it);
   }
}


bool
aflibMemCache::checkExistingNode(
   long long position,
   aflibData& data)
{
   // This will check if the data can replace an existing node. If it does then TRUE
   // will be returned. There are three possible cases that are covered in this function.
   // They are cased on the following diagram.
   //
   //             |---New Data---|
   //
   // Case 1  |----------|
   // Case 2  |---------------------|
   // Case 3              |---->
   //
   // Case 1 is the existing node starts before the new data node and ends before the
   // new data node ends.
   // Case 2 is the existing node starts before the new data node and ends after the
   // new data node ends.
   // Case 3 is the existing node starts after the new data node and ends either before
   // or after the new node ends.
   //
   // node_position and node_size are the position and sizes of the current node that we
   // are iterating on. size and position is the size and position of the new data that
   // is to be added. size and position can change as we add data to an existing node.
   // orig_position and orig size is the original size and position of the new data and
   // does not change.


   map< long long, aflibMemNode* >::iterator it;
   bool processed = FALSE;
   long long node_position;
   long long node_size;
   long long size;
   long long orig_position;
   long long orig_size;
   int  i, j;
   int  chans;


   chans = data.getConfig().getChannels();
   size = data.getLength();
   orig_position = position;
   orig_size = size;
 
   // Loop thru all nodes 
   for (it = _node_array.begin(); it != _node_array.end(); it++)
   {
      node_position = (*it).first;
      node_size = (*it).second->getSize();

      // IF end point of node is beyond new data start then
      if (node_position + node_size > position)
      {
         // IF beginning point of node is before or equal new data start
         if (node_position <= position)
         {
            // IF end point of node is before or equal new data end point
            if (node_position + node_size <= position + size)
            {
               // Case 1
               processed = TRUE;

               vector<int>& array = (*it).second->getData();
               for (i = 0; i < node_size + node_position - position; i++)
               {
                  for (j = 0; j < chans; j++)
                  {
                     array[i * chans + j + position - node_position] =
                        data.getSample(i + position - orig_position, j);
                  }
               }

               // Update node information
               size -= node_size + node_position - position;
               position = node_size + node_position;

            }
            else
            {
               // Case 2
               processed = TRUE;

               vector<int>& array = (*it).second->getData();
               for (i = 0; i < size; i++)
               {
                  for (j = 0; j < chans; j++)
                  {
                     array[i * chans + j + position - node_position] =
                        data.getSample(i + position - orig_position, j);
                  }
               }
               break;
            }
         }
         // ELSE beginning point of node is after new data start AND
         // before end of new data
         else if (node_position < position + size)
         {
            // Case 3
            processed = TRUE;

            // IF there is data before this node then add it as new node
            if (node_position > position)
            {
               createNewNode(0, node_position - position, position, data);

               // Update node information
               size -= (node_position - position);
               position = node_position;
            }

            vector<int>& array = (*it).second->getData();

            // Replace the rest of data in existing node
            // Check to see if new data ends before node data ends
            if (size + position <= node_position + node_size)
            {
               for (i = 0; i < size; i++)
               {
                  for (j = 0; j < chans; j++)
                  {
                     array[i * chans + j] =
                        data.getSample(i + position - orig_position, j);
                  }
               }
               break;
            }
            else
            {
               for (i = 0; i < node_size; i++)
               {
                  for (j = 0; j < chans; j++)
                  {
                     array[i * chans + j] =
                        data.getSample(i + position - orig_position, j);
                  }
               }

               // Update node information
               size -= node_size;
               position += node_size;
            }
         }
         else
         {
            // IF we get here the node is completely past the new data and we are done
            break;
         }
      }
   }

   return (processed);
}


void
aflibMemCache::createNewNode(
   int start_element,
   int stop_element,
   long long position,
   aflibData& data)
{
   // This private function adds a new node to the node array that stores memory data

   aflibMemNode  * node;
   int i, j;
   long long  size;
   int  chans;


   chans = data.getConfig().getChannels();
   // Create new memory node 
   node = new aflibMemNode();
   node->setChannels(chans);

   vector<int>& array = node->getData();

   // Loop over samples then channels
   for (i = start_element; i < stop_element; i++)
   {
      for (j = 0; j < chans; j++)
      {
         array.push_back(data.getSample(i, j));
      }
   }

 
   // Since everything stored as ints use 4 bytes
   size = ((stop_element - start_element) * node->getChannels() * 4);
   _cache_size_local += size;
   _cache_size_total += size;
 
   // Store new memory node into set
   _node_array[position] = node;
}


void
aflibMemCache::fillDataFromCache(
   aflibData& data,
   long long& position,
   int& num_samples,
   long long orig_position,
   int orig_num_samples)
{
   // This function will goto the cache fill in any necessary data that is missing from the
   // the start of the data. It does not support filling in data from any other
   // location. If we fail to fill in any data it is an error in another function in this
   // class that said we could. Parameters position and num_samples indicate new data that
   // was not cached but was obtained from the audio chain. Parameters orig_position and
   // orig_num_samples indicate the original data that was asked for whether data was
   // cached or not. Thus position - orig_position indicates data that was cached and will
   // be retrieved by this function.
   //
   //    ---------------------------------------------------------------------------
   //   |                       |                                                   |
   //   |                       |                                                   |
   //   |<----------A---------->|<-----------------------B------------------------->|
   //   |                       |<-----------------num_samples--------------------->|
   //   |<---------------orig_num_samples------------------------------------------>|
   //    ---------------------------------------------------------------------------
   //   ^orig_position          ^position
   //
   //   Above shows the data layout. data when this function is called contains B. The
   //   A data will be obtained from the cache. When this function exits it will contain
   //   A + B.

   aflibData  * new_data = NULL;
   aflibData  * data_ptr = &data;
   map< long long, aflibMemNode* >::iterator it;
   long long n_position;
   long long n_size;
   int  i, j;
   int  chans;

	if(position == -1) return;

   // Cache audio data if needed
   cacheData(position, data);

   chans = data.getConfig().getChannels();

   // IF we need more memory allocate a new data array
   if (orig_num_samples > data.getOrigLength())
   {
      new_data = new aflibData(data.getConfig(), orig_num_samples);
      data_ptr = new_data;
   }

   // IF we need data from the start then get the data
   if (position != orig_position)
   {
      // move data from current position to make room for new data at start
      for (i = 0; i < num_samples; i++)
      {
         for (j = 0; j < chans; j++)
         {
            data_ptr->setSample(data.getSample(i, j), i + position - orig_position, j);
         }
      }

      // Now insert the data at the start
      // Loop thru all nodes 
      for (it = _node_array.begin(); it != _node_array.end(); it++)
      {
         n_position = (*it).first;
         n_size = (*it).second->getSize();

         // Are we done
         if (n_position >= position)
         {
            break;
         }

         // Does this node contain data for us
         // TBD may need to check if next node has data for us as well
         if ((orig_position >= n_position) && (orig_position <= n_position + n_size))
         {
            int local_size;
            int local_offset;

            // Find offset into data
            local_offset = (int)(orig_position - n_position);

            // We need to find the size of the data that is available for us
            if ((n_position + n_size - orig_position) >= (position - orig_position))
            {
               // All data is in node use it
               local_size = position - orig_position;
            }
            else
            {
               // Only use amount available from node
               local_size = n_position + n_size - orig_position;
            }

            // Fill in the data
            for (i = 0; i < local_size; i++)
            {
               for (j = 0; j < chans; j++)
               {
                  data_ptr->setSample(
                    (*it).second->getData()[(i + local_offset) * chans + j],
                    i, j);
               }
            }
            num_samples += local_size;
            position -= local_size;
         }
      }
   }

#if 0
   // We should never need this case for this algorithm

   // IF we need data at the end then get the data
   if (num_samples != orig_num_samples)
   {
      cerr << "NOT YET IMPLEMENTED!" << endl;
   }
#endif

   // IF we allocated a new data object then copy it to the one returned
   if (new_data)
   {
      data = *new_data;
      delete new_data;
   }
}


/*! \brief Empties the cache for this object only.
*/
void
aflibMemCache::clearCache()
{
   map< long long, aflibMemNode* >::iterator it;

   // Delete all allocated objects
   for (it = _node_array.begin(); it != _node_array.end(); it++)
   {
      delete (*it).second;
   }

   // Erase all nodes
   if (_node_array.size() != 0)
   {
      _node_array.erase( _node_array.begin(), _node_array.end());
   }
   _cache_size_total -= _cache_size_local;
   _cache_size_local = 0;
}


