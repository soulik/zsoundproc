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


#include <stdio.h>
#include <iostream>
#include <sys/stat.h>


#include "aflibAudioRecorder.h"
#include "aflibAudioFile.h"
#include "aflibData.h"
#include "aflibConfig.h"


/*! \brief Constructor that requires an audio parent.
*/
aflibAudioRecorder::aflibAudioRecorder(aflibAudio& audio) :
   aflibAudio(audio)
{
}


/*! \brief Empty constructor.

    A empty constructor is provided for the case where this object will serve as
    a data collector keeping record segments stored. When it is ready to be used
    as part of a chain then the addParent in the base class should be called with the
    audio parent object.
*/
aflibAudioRecorder::aflibAudioRecorder()
{
}


/*! \brief Destructor.
*/
aflibAudioRecorder::~aflibAudioRecorder()
{
}


/*! \brief Adds an item to the record list.

    This function will add a record item to the list of record elements. Enter a start and
    stop date and time. A file and file type. The file_type argument must be a supported
    format. The list of formats can be returned from the call returnSupportedFormats from
    the aflibFile base class. This is a static function so it can be called at anytime. The
    max_limit is the total maximum disk space that can be used. Recording will stop when
    this value has been reached. To bypass the max file limit use -1. There is also an
    each_limit that can be bypassed with a -1 as well. This will split the audio into
    multiple files. It will start recording to a new file when this limit has been reached.
    Finally the config parameter will tell this class how to setup your audio file.
*/
void
aflibAudioRecorder::addRecordItem(
   const aflibDateTime& start_date,
   const aflibDateTime& stop_date,
   const string& file,
   const string& file_type,
   long long max_limit,
   long long each_limit,
   aflibConfig& config)
{
   aflibRecorderItem audio(
      start_date, stop_date, file, file_type, max_limit, each_limit, config);

   // Add new record item to list
   _item_list.push_back(audio);
}


/*! \brief Removes a audio record item.

    This will remove the specified item. The item number is obtained from calling
    getNumberOfRecordItems and getRecordItem. Item numbers start at 1.
*/
void
aflibAudioRecorder::removeRecordItem(
   int item)
{
   list<aflibRecorderItem>::iterator it;
   int i = 1;

   // Iterate thru list and till we find the item
   for (it = _item_list.begin(); it != _item_list.end(); it++, i++)
   {
      // When we find the item remove
      if (i == item)
      {
         _item_list.erase(it);
         break;
      }
   }
}


/*! \brief Gets number of current record items.
*/
int
aflibAudioRecorder::getNumberOfRecordItems() const
{
   int total_size;

   // Return size of list as number of items
   total_size = _item_list.size();

   return(total_size);
}


/*! \brief Gets the data for a record item.
*/
void
aflibAudioRecorder::getRecordItem(
   int item,
   aflibDateTime& start_date,
   aflibDateTime& stop_date,
   string& file, 
   string& file_type, 
   long long& max_limit,
   long long& each_limit,
   aflibConfig& config) const
{
   list<aflibRecorderItem>::const_iterator it;
   int i = 1;

   // Iterate until we get to the correct item
   for (it = _item_list.begin(); it != _item_list.end(); it++, i++)
   {
      // When we find the item return the results
      if (i == item)
      {
         start_date = (*it).getStartDate();
         stop_date = (*it).getStopDate();
         file = (*it).getAudioFile();
         file_type = (*it).getAudioFileType();
         max_limit = (*it).getMaxFileLimit();
         each_limit = (*it).getEachFileLimit();
         config = (*it).getConfig();
         break;
      }
   }
}


aflibStatus
aflibAudioRecorder::compute_segment(
   list<aflibData *>& data,
   long long position) 
{
   // This is the standard work function that does the work.

   aflibDateTime current_time;
   list<aflibRecorderItem>::iterator it;
   aflibStatus status = AFLIB_SUCCESS;
   aflibAudioFile * audio_file = NULL;
   aflibConfig config;

   current_time.setCurrentTime();

   // Check each item in the list
   for (it = _item_list.begin(); it != _item_list.end(); it++)
   {
      // We are done
      if ((*it).getStopDate() <= current_time)
      {
         // If we are currently recording then stop
         if ((*it).getFileObject())
         {
            delete (*it).getFileObject();
            (*it).setFileObject(NULL);

         }
      }
      // We have not started yet
      else if (current_time < (*it).getStartDate())
      {
         // Nothing to do
      }
      // We are currently recording
      else
      {
         // Are we currently writing to a file
         if ((*it).getFileObject())
         {
            // Are we done? Have we reached the file limit.
            if (audioFileSizeCheck((*it)) == FALSE)
            {
               // Set stop date to indicate stop time
               (*it).setStopDate(current_time);
            }
         }
         // ELSE create a file
         else
         {
            config = (*it).getConfig();
            audio_file = new aflibAudioFile(*this, (*it).getAudioFileType(),
               (*it).getAudioFile(), &config, &status);

            (*it).setFileObject(audio_file);
         }
         // Write out a data segment
         ((aflibAudio *)(*it).getFileObject())->compute_segment(data, -1);
      }
   }

   return (AFLIB_SUCCESS);
}


bool
aflibAudioRecorder::audioFileSizeCheck(aflibRecorderItem& item)
{
   // This function will check the file sizes to make sure we can still store
   // data. If all is still OK them TRUE is returned. If we are done then a
   // FALSE is returned.

   const char * file = NULL;
   struct stat  statbuf;
   bool ret_result = TRUE;
   aflibConfig config;
   aflibStatus status;
   aflibAudioFile * audio_file = NULL;
   int  stat_status = 0;

   file = item.getAudioFile().c_str();

   stat_status = stat(file, &statbuf);
   // In some cases the new file may not be created just yet. This occurs with
   // mp3 when we create a pipe to another process. Thus if file is not found
   // then assume 0 size
   if (stat_status == -1)
   {
      statbuf.st_size = 0;
   }

   // Is each file size implemented
   if (item.getEachFileLimit() != -1)
   {
      //  Has the total length of an audio file been exceeded
      if (statbuf.st_size > item.getEachFileLimit())
      {
         config = item.getConfig();

         // close old file and free memory
         delete item.getFileObject();

         // Get new file name and create new file object and store
         item.processNextFile();

         audio_file = new aflibAudioFile(*this, item.getAudioFileType(),
            item.getAudioFile(), &config, &status);
         item.setFileObject(audio_file);
        
         // If a new file was created then get new file size
         stat_status = stat(item.getAudioFile().c_str(), &statbuf);
         // In some cases the new file may not be created just yet. This occurs with
         // mp3 when we create a pipe to another process. Thus if file is not found
         // then assume 0 size
         if (stat_status == -1)
         {
            statbuf.st_size = 0;
         }
      }
   }

   // Is total file size implemented
   if (item.getMaxFileLimit() != -1)
   {
      // Get size of all file segments so far
      long long total = item.getNumSegmentsSoFar() * item.getEachFileLimit()
         + statbuf.st_size;

      //  Has the total length of an audio file been exceeded
      if (total > item.getMaxFileLimit())
      {
         ret_result = FALSE;
      }
   }
   return (ret_result);
}


