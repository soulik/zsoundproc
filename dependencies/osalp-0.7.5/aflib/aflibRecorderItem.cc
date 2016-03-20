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

#include "aflib.h"
#include "aflibRecorderItem.h"


aflibRecorderItem::aflibRecorderItem() :
   _max_limit(-1),
   _each_limit(-1),
   _audio_file(NULL),
   _file_segment(0)
{
}

aflibRecorderItem::aflibRecorderItem(
   const aflibDateTime start_date,
   const aflibDateTime stop_date,
   const string& file,
   const string& file_type,
   long long max_limit,
   long long each_limit,
   aflibConfig& config) :
   _start_date(start_date),
   _stop_date(stop_date),
   _file(file),
   _base_file(file),
   _file_type(file_type),
   _max_limit(max_limit),
   _each_limit(each_limit),
   _audio_file(NULL),
   _file_segment(0),
   _config(config)
{
}

aflibRecorderItem::~aflibRecorderItem()
{
}

void
aflibRecorderItem::setStartDate(const aflibDateTime& date1)
{
   // Set the start date and time.

   _start_date = date1;
}

const aflibDateTime&
aflibRecorderItem::getStartDate() const
{
   return (_start_date);
}

void
aflibRecorderItem::setStopDate(const aflibDateTime& date1)
{
   // Set the stop date and time.

   _stop_date = date1;
}

const aflibDateTime&
aflibRecorderItem::getStopDate() const
{
   return (_stop_date);
}

void
aflibRecorderItem::setAudioFile(const string& file)
{
   // Set the file name of the file to store.

   _file = file;
}

const string&
aflibRecorderItem::getAudioFile() const
{
   return (_file);
}

void
aflibRecorderItem::setAudioFileType(const string& file_type)
{
   // Set the format of the file.

   _file_type = file_type;
}

const string&
aflibRecorderItem::getAudioFileType() const
{
   return (_file_type);
}

void
aflibRecorderItem::setMaxFileLimit(long long size)
{
   // Set the total file size. For instance if you only had 1GB of file
   // space available then set something less than 1,000,000,000. Then
   // recording will stop when the file size reaches this value. It will
   // stop even if the stop time is not reached. If you don't care about
   // file size then enter -1. If each file limit is set then this limit
   // will be the size of all files added together.

   _max_limit = size;
}

long long
aflibRecorderItem::getMaxFileLimit() const
{
   return(_max_limit);
}

void
aflibRecorderItem::setEachFileLimit(long long size)
{
   // Set the file size limit for each file. When the limit is reached then that
   // audio file will be closed and a new one will be created. This will continue
   // until the setMaxFileLimit is reached (see setMaxFileLimit) or the stop time
   // occurs. If you do not want multiple files then enter -1.

   _each_limit = size;
}

long long
aflibRecorderItem::getEachFileLimit() const
{
   return (_each_limit);
}

void
aflibRecorderItem::setFileObject(aflibAudioFile * audio)
{
   // This stores the audio file object in which the audio data is being written to.

   _audio_file = audio;
}

aflibAudioFile *
aflibRecorderItem::getFileObject() const
{
   return (_audio_file);
}

void
aflibRecorderItem::setConfig(const aflibConfig& cfg)
{
   // This function stores the audio data configuration.

   _config = cfg;
}

const aflibConfig&
aflibRecorderItem::getConfig() const
{
   return (_config);
}

void
aflibRecorderItem::processNextFile()
{
   // When the each file size limit has been reached for a file then this function
   // should be called so that the next file name can be processed. After this 
   // function is called then getAudioFile can be called to get the new name. The
   // new file names will have _<number> before the extension. 

   size_t insert_position;
   char buf[100];

   _file_segment++;

   sprintf(buf, "_%d", _file_segment);
   insert_position = _base_file.rfind('.');
   _file = _base_file;
   _file.insert(insert_position, buf, strlen(buf));
}

int
aflibRecorderItem::getNumSegmentsSoFar()
{
   // This returns the total number of file segments that have completed
   // writing so far.

   return(_file_segment);
}


