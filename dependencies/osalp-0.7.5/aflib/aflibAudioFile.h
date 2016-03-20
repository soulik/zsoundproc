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


#ifndef _AFLIBAUDIOFILE_H
#define _AFLIBAUDIOFILE_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "aflib.h"
#include "aflibAudio.h"
#include <string>

using std::string;


/*! \class aflibAudioFile
    \brief Wrapper class that allows aflibFile object to be used in a chain.

 This class is the object wrapper that can make an aflibFile object useable
 as an audio object and to be used in a chain. For using file and device
 objects in a chain this is the API that one should use. The constructors
 are the same as the base class.

 Function compute_segment is implemented so that the base aflibAudio class can
 process data throught this class. 

 There are three constructors with this class. One with no aflibAudio parent and the
 other two with parents. When starting a chain with an audio source the constructor
 without a parent should be used. When at the end of a chain one of the two constructors
 that require an audio aflibAudio object should be used.
*/


class aflibConfig;
class aflibFile;

class aflibAudioFile : public aflibAudio {

public:

   // Constructor for a node with no parent (ie start of a chain)
   aflibAudioFile(
      aflibFileType type_enum,
      const string& file,
      aflibConfig* cfg = NULL,
      aflibStatus* status = NULL);

   // Constructor for a node with no parent (ie start of a chain)
   aflibAudioFile(
      const string& format,
      const string& file,
      aflibConfig* cfg = NULL,
      aflibStatus* status = NULL);

   // Constructor for a node with a parent
   aflibAudioFile(
      aflibAudio& audio,
      aflibFileType type_enum,
      const string& file,
      aflibConfig* cfg = NULL,
      aflibStatus* status = NULL);

   // Constructor for a node with a parent
   aflibAudioFile(
      aflibAudio& audio,
      const string& format,
      const string& file,
      aflibConfig* cfg = NULL,
      aflibStatus* status = NULL);

   ~aflibAudioFile();

   aflibStatus
   compute_segment(
      list<aflibData *>& data,
      long long position = -1) ;

   bool
   setItem(
      const char * item,
      const void * value);

   bool
   getItem(
      const char * item,
      void * value);

   //! Returns the name of the derived class.
   const char *
   getName() const { return "aflibAudioFile";};

   void
   setInputConfig(const aflibConfig& cfg);
 
   const aflibConfig&
   getInputConfig() const;
 
   void
   setOutputConfig(const aflibConfig& cfg);
 
   const aflibConfig&
   getOutputConfig() const;

   const string&
   getFormat() const;

   bool
   isDataSizeSupported(aflib_data_size size);
 
   bool
   isEndianSupported(aflib_data_endian end);
 
   bool
   isSampleRateSupported(int& rate);

   bool
   isChannelsSupported(int& channels);

protected:

private:

aflibFile  *  _file_object;
bool       _has_parent;

};


#endif
