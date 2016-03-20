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


#ifndef _AFLIBMEMCACHE_H
#define _AFLIBMEMCACHE_H


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

/*! \class aflibMemCache
     \brief Base class that implements audio data caching.

 The purpose of this class is to provide a memory caching mechanism to those classes
 who whould benefit from this. It will store audio data into a buffer that it controls
 so that when audio data is reread for what ever purpose it will come from this cache
 instead of from the original source. This is beneficial for three reasons. 
 1) Some sources may not support backing up for old data. For instance an http stream
    comming over an internet connection. Some derived aflibAudio objects need to back
    up over old data. For instance the sample rate converter class. It needs to operate
    on an old history of the data for best results. In order to save the complexity from
    each derived class we will implement caching here. 
 2) Reading audio data can be expensive from some sources. Reading from a disk can be
    slow and audio data from mp3s requires decoding. This will provide the capability
    to not have to reread again from the original source. 
 3) Some audio sources do not support random access. We can provide a limited random
    access capability for these sources by caching the audio data from the start of the
    file. Of course there are limitations with this.

 Most derived classes by default have caching disabled. To enable or disable caching one
 would call setCacheEnable. The environment variable AFLIB_CACHE_SIZE can be set to set
 the maximum amount of cache to use. The default is 1MB.
*/

class aflibData;
class aflibMemNode;

#include <map>
using std::map;

class aflibMemCache {

public:

   aflibMemCache();

   virtual
   ~aflibMemCache();

   bool
   getCacheEnable();

   void
   setCacheEnable(bool enable);

   long long
   getCacheMax() const;

   long long
   getCacheTotal() const;

   long long
   getCacheLocal() const;

   void
   clearCache();

protected:

   void
   lookupData(
      long long& position,
      int& num_samples);

   void
   fillDataFromCache(
      aflibData& data,
      long long& position,
      int& num_samples,
      long long orig_position,
      int orig_num_samples);

private:

   aflibMemCache(aflibMemCache& op);

   const aflibMemCache&
   operator=(const aflibMemCache& op);

   void
   calcPosition(
      long long& position,
      int&       num_samples,
      map< long long, aflibMemNode* >::iterator it);

   void
   reduceCache();

   bool
   checkExistingNode(
      long long position,
      aflibData& data);

   void
   createNewNode(
      int start_element,
      int stop_element,
      long long position,
      aflibData& data); 

   void
   cacheData(
      long long position,
      aflibData& data);

long long _cache_counter;
long long _cache_size_max;
static long long _cache_size_total;
long long _cache_size_local;
bool      _enable;
map< long long, aflibMemNode* >  _node_array;


};

#endif


