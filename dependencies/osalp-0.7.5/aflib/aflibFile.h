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

#ifndef _AFLIBFILE_H
#define _AFLIBFILE_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "aflib.h"
#include "aflibConfig.h"

#include <string>
#include <list>
#include <map>
using std::string;
using std::list;
using std::map;


/*! \class aflibFile
    \brief Base class for audio file/device reading and writing.

 This is the base class for all audio file and device objects. It provides
 the functionality for calling derived classes and defines the API for these
 derived classes. The main entry APIs are the open and create static
 functions. Users actually use these. This will in turn allocate an object
 of the proper derived class. There are no public constructors for this
 class. Also static is a function called 
 returnSupportedFormats. It will return all the current supported file formats.
 Once an object is allocated and initialized then one can call getFormat to get 
 the file format that is currently being used.

 The functions afopen, afcreate, afread, and afwrite are all virtual and are 
 designed to be overriden by the derived classes. Also there are three functions
 called setValue1, setValue2, and setValue3. These can be used to pass implementation
 dependant info to a file format.

 This class supports the ability to dynamically load and free these derived
 objects. These derived objects will each be made into a library that will
 be loaded only when needed.

 This class can be compiled with the flag NO_MODULES. If it is then the file
 objects will not be dynamically loaded. Instead applications will have
 to link against these libraries. Also only certain file formats will be
 supported.
*/

class aflibData;
class aflibFileItem;

class aflibFile {

public:

   virtual ~aflibFile();

   static void
   returnSupportedFormats(
      list <string>& formats,
      list <string>& descriptions);

   static const list<aflibFileItem*>&  
	returnSupportedFormats();

   static aflibFile *
   open(
      aflibFileType type_enum,
      const string& file,
      aflibConfig* cfg = NULL,
      aflibStatus* status = NULL);

   static aflibFile *
   open(
      const string& format,
      const string& file,
      aflibConfig* cfg = NULL,
      aflibStatus* status = NULL);

   static aflibFile *
   create(
      aflibFileType type_enum,
      const string& file,
      const aflibConfig& cfg,
      aflibStatus* status = NULL);

   static aflibFile *
   create(
      const string& format,
      const string& file,
      const aflibConfig& cfg,
      aflibStatus* status = NULL);

   virtual aflibStatus
   afopen(
      const char * file,
      aflibConfig* cfg);

   virtual aflibStatus
   afcreate(
      const char * file,
      const aflibConfig& cfg);

   virtual aflibStatus
   afread(
      aflibData& data,
      long long position = -1);

   virtual aflibStatus
   afwrite(
      aflibData& data,
      long long position = -1);

   virtual bool
   setItem(
      const char * item,
      const void * value);

   virtual bool
   getItem(
      const char * item,
      void * value);

   const string&
   getFormat() const;

   virtual void
   setInputConfig(const aflibConfig& cfg);
 
   const aflibConfig&
   getInputConfig() const;
 
   void
   setOutputConfig(const aflibConfig& cfg);
 
   const aflibConfig&
   getOutputConfig() const;

   void
   setValue1(const string& value);

   void
   setValue2(const string& value);

   void
   setValue3(const string& value);

   virtual bool
   isDataSizeSupported(aflib_data_size size);
 
   virtual bool
   isEndianSupported(aflib_data_endian end);
 
   virtual bool
   isSampleRateSupported(int& rate);

   virtual bool
   isChannelsSupported(int& channels);

protected:

   aflibFile();

   long       _total_size;
   long       _one_second_size;
   long       _current_place_size;
   string     _value1;
   string     _value2;
   string     _value3;


private:

	//void (*_isChannelsSupported)(int&);
   aflibConfig _cfg_input;
   aflibConfig _cfg_output;
   void *     _lib1;
   aflibFile * _file_object;
   string     _format;
   static list<aflibFileItem *>  _support_list;
   static bool _list_created;


   aflibFile(const char * module_name);

   aflibFile(const aflibFile& file);

   const aflibFile&
   operator== (const aflibFile& file);

   static aflibFile *
   allocateModuleFile(
      aflibFileType type_enum,
      const char *  module_format);

   static const char *
   findModuleFile(const string& file_name);

   static void
   parseModuleFile();

   bool
   initialized();

};

#endif
