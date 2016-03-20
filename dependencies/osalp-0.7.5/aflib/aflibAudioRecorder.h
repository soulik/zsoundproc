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


#ifndef _AFLIBAUDIORECORDER_H_
#define _AFLIBAUDIORECORDER_H_

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


#include "aflibAudio.h"
#include "aflibDateTime.h"
#include "aflibRecorderItem.h"
#include <string>
#include <list>
using std::string;
using std::list;


/*! \class aflibAudioRecorder
    \brief Audio timer recording class.

 This is a class that performs audio timer recording. This is derived from the aflibAudio
 base class so it can be used in an audio chain. This is a complex class that aids the 
 user in developing functionality to their audio application that will perform like a
 VCR does for video. One creates audio record items. You supply start times, stop times,
 size limits, and info. Then you form this as part of a chain and loop on the process 
 function of this objects base class. When the start time is reached for an item it will
 start recording. It will stop on several criteria. One can specify multiple items and in
 fact they can even overlap in time.

 ADDING RECORD ITEMS
 One can add record items by calling addRecordItem. One specifies all the necessary
 information and then make it part of the audio chain and it is ready to go. If this audio
 object is destroyed then the record information is lost. If one wants to start recording
 right away then make the start time before the current time. The recording will stop based
 upon when one of two criteria is reached. If the stop time is reached then recording will
 stop. There are also two file size limits that can be set. If these are reached before the
 stop time then recording will be stopped. There is a max file size limit. This will allow
 one to set an upper limit on the disk space that will be used by the recording. This is
 useful when you only have a certain amount of disk space available. There is also an
 each file size limit. This will allow you to set a limit on how big each audio file will
 be. When that limit is reached then that file will be closed and a new one will be created.
 The new name will be called the original name plus _<number> before the extension. As many
 files will be created until either the stop time is reached or the max file size limit
 is reached. For each file mode the max file size limit means the total size of all audio
 files created. Each file mode is useful for portable MP3 players that have a memory limit.
 If you want to record a long show just set a each file size limit of 30M then the show
 will be stored to sperate files of 30M each. Just big enough to store on a MP3 player.

 REMOVING RECORD ITEMS
 One can delete already stored items by calling removeRecordItem. Just pass the item
 number. The item number can be obtained from calls to getNumberOfRecordItems, getRecordItem.
 getNumberOfRecordItems will return the total number of items stored so far. getRecordItem
 will return all the information for an item. The numbering of items start at one.
*/


class aflibConfig;


class aflibAudioRecorder : public aflibAudio {

public:

   // Available contructors and destructors
   aflibAudioRecorder(aflibAudio& audio);
   aflibAudioRecorder();
   ~aflibAudioRecorder();

   void
   addRecordItem(
      const aflibDateTime& start_date,
      const aflibDateTime& stop_date,
      const string& file,
      const string& file_type,
      long long max_limit,
      long long each_limit,
      aflibConfig& config);

   void
   removeRecordItem(
      int item);

   int
   getNumberOfRecordItems() const;

   void
   getRecordItem(
      int item,
      aflibDateTime& start_date,
      aflibDateTime& stop_date,
      string& file,
      string& file_type,
      long long& max_limit,
      long long& each_limit,
      aflibConfig& config) const;

   aflibStatus
   compute_segment(
      list<aflibData *>& data,
      long long position = -1) ;

   //! Returns the name of the derived class.
   const char *
   getName() const { return "aflibAudioRecorder";};

private:


   aflibAudioRecorder(const aflibAudioRecorder& op);

   const aflibAudioRecorder&
   operator=(const aflibAudioRecorder& op);

   bool
   audioFileSizeCheck(aflibRecorderItem& item);

list<aflibRecorderItem>   _item_list;

};


#endif
