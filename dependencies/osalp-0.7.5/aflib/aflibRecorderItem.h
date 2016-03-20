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


#ifndef _AFLIBRECORDERITEM_H_
#define _AFLIBRECORDERITEM_H_


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "aflibDateTime.h"
#include "aflibData.h"
#include "aflibConfig.h"
#include <string>

class aflibAudioFile;


/*! \class aflibRecorderItem
    \brief Data class used by class aflibAudioRecorder.
*/

class aflibRecorderItem {

public:

   aflibRecorderItem ();

   aflibRecorderItem(
      const aflibDateTime start_date,
      const aflibDateTime stop_date,
      const string& file,
      const string& file_type,
      long long max_limit,
      long long each_limit,
      aflibConfig& config);

   ~aflibRecorderItem();

   void
   setStartDate(const aflibDateTime& date1);

   const aflibDateTime&
   getStartDate() const;

   void
   setStopDate(const aflibDateTime& date1);

   const aflibDateTime&
   getStopDate() const;

   void
   setAudioFile(const string& file);

   const string&
   getAudioFile() const;

   void
   setAudioFileType(const string& file);

   const string&
   getAudioFileType() const;

   void
   setMaxFileLimit(long long size);

   long long
   getMaxFileLimit() const;

   void
   setEachFileLimit(long long size);

   long long
   getEachFileLimit() const;

   void
   setFileObject(aflibAudioFile * audio);

   aflibAudioFile *
   getFileObject() const;

   void
   setConfig(const aflibConfig& cfg);

   const aflibConfig&
   getConfig() const;

   void
   processNextFile();

   int
   getNumSegmentsSoFar();

private:


aflibDateTime  _start_date;
aflibDateTime  _stop_date;
string         _file;
string         _base_file;
string         _file_type;
long long      _max_limit;
long long      _each_limit;
aflibAudioFile * _audio_file;
int              _file_segment;
aflibConfig      _config;
};

#endif
