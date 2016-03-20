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


#ifndef _AFLIBFILEITEM_H_
#define _AFLIBFILEITEM_H_

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string>
#include <vector>
#include <list>
using std::string;
using std::list;
using std::vector;



/*! \class aflibFileItem
    \brief Data class used internally by library for file module support.
*/

class aflibFileItem {

public:

   aflibFileItem ();

   aflibFileItem(
      const string&  mod_format,
      const string&  mod_dscr,
      const string&  mod_ext,
      const string&  mod_magic,
      const string&  mod_name,
      const string&  mod_value1,
      const string&  mod_value2,
      const string&  mod_value3);

   ~aflibFileItem();

   bool
   operator==(const vector<unsigned char>& array) const;

   void
   setFormat(const string& mod_format);

   const string&
   getFormat() const;

   void
   setDescription(const string& mod_dscr);

   const string&
   getDescription() const;

   void
   setExtension(const string& mod_ext);

   const list<string>&
   getExtensions() const;

   void
   setMagic(const string& mod_magic);

   const list<string>&
   getMagic() const;

   void
   setName(const string& mod_name);

   const string&
   getName() const;

   void
   setValue1(const string& mod_value1);

   const string&
   getValue1() const;

   void
   setValue2(const string& mod_value2);

   const string&
   getValue2() const;

   void
   setValue3(const string& mod_value3);

   const string&
   getValue3() const;

private:

   string  _mod_format;
   string  _mod_dscr;
   list<string>  _mod_ext;
   string  _mod_name;
   string  _mod_value1;
   string  _mod_value2;
   string  _mod_value3;

   list<string> _mod_magic;

};

#endif
