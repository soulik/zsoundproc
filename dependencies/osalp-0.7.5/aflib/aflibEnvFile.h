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

#ifndef _AFLIBENVFILE_H_
#define _AFLIBENVFILE_H_


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

/*! \class aflibEnvFile
    \brief Manages settings for each individual of a program.

 This class will create a .<name> directory in the users home directory or
 a directory for the users choice. It will then manage a set of key values
 that the programmer can search for. This allows programmers to save a users
 settings and then retrieve then at a latter time. It currently supports
 upto 1024 different settings.
 Two constructors are provided. They will dertermine what directory to look
 for the settings file and what the name of the file is. If the directory is
 not specified then a default directory of .<DEFAULT_DIR> will be created in
 the users home directory. A file will be created in this directory. The name
 will be the one specified by the caller. If none is specified then
 DEFAULT_FILE will be created.
 To retrieve and store strings into the file readValueFromFile and writeValueToFile
 should be called.
*/


#include <string>

using std::string;

class aflibEnvFile {

public:

   aflibEnvFile(
      char * env_file,
      char * env_dir = NULL);

   aflibEnvFile();

   ~aflibEnvFile();

   bool
   readValueFromFile(
      string& key_str,
      string& results);

   void
   writeValueToFile(
      string& key_str,
      string& value);


private:

string _env_file;

};


#endif
