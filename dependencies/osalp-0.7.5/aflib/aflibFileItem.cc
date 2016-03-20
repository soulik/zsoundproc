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


#include "aflibFileItem.h"

#include <stdio.h>
#include <stdlib.h>

#include "aflib.h"


aflibFileItem::aflibFileItem()
{
}

aflibFileItem::aflibFileItem(
   const string&  mod_format,
   const string&  mod_dscr,
   const string&  mod_ext,
   const string&  mod_magic,
   const string&  mod_name,
   const string&  mod_value1,
   const string&  mod_value2,
   const string&  mod_value3)
{
   _mod_format = mod_format;
   _mod_dscr = mod_dscr;
   _mod_ext.push_back(mod_ext);
   _mod_name = mod_name;
   _mod_value1 = mod_value1;
   _mod_value2 = mod_value2;
   _mod_value3 = mod_value3;

   _mod_magic.push_back(mod_magic);
}

aflibFileItem::~aflibFileItem()
{
}


bool
aflibFileItem::operator==(const vector<unsigned char>& array) const
{
   bool recognized = FALSE;
   const list<string>& magic_list = getMagic();
   list<string>::const_iterator it;
   int num[30], ret_value;
   unsigned char buf[30];
   int i, value;
   bool  search_mode;
   int   search_ret_value = 0;
   int   search_i = 0;
   int   search_pos;
   int add_delta, search_delta, adj_num;


   for (it = magic_list.begin(); it != magic_list.end(); it++)
   {
      search_pos = 0;
      add_delta = 0;
      adj_num = 0;
      search_mode = FALSE;
      recognized = TRUE;

      // Read upto 30 pairs of numbers and characters
      ret_value = sscanf((*it).c_str(),
         "%d(%1c), %d(%1c), %d(%1c), %d(%1c), %d(%1c), "\
         "%d(%1c), %d(%1c), %d(%1c), %d(%1c), %d(%1c), "\
         "%d(%1c), %d(%1c), %d(%1c), %d(%1c), %d(%1c), "\
         "%d(%1c), %d(%1c), %d(%1c), %d(%1c), %d(%1c), "\
         "%d(%1c), %d(%1c), %d(%1c), %d(%1c), %d(%1c), "\
         "%d(%1c), %d(%1c), %d(%1c), %d(%1c), %d(%1c)",
         &num[0], &buf[0], &num[1], &buf[1], &num[2], &buf[2],
         &num[3], &buf[3], &num[4], &buf[4], &num[5], &buf[5],
         &num[6], &buf[6], &num[7], &buf[7], &num[8], &buf[8],
         &num[9], &buf[9], &num[10], &buf[10], &num[11], &buf[11],
         &num[12], &buf[12], &num[13], &buf[13], &num[14], &buf[14],
         &num[15], &buf[15], &num[16], &buf[16], &num[17], &buf[17],
         &num[18], &buf[18], &num[19], &buf[19], &num[20], &buf[20],
         &num[21], &buf[21], &num[22], &buf[22], &num[23], &buf[23],
         &num[24], &buf[24], &num[25], &buf[25], &num[26], &buf[26],
         &num[27], &buf[27], &num[28], &buf[28], &num[29], &buf[29]);

      ret_value /= 2;
      i = 0;

      // Loop thru all data pairs. When no match we exit with not recognized otherwise
      // if we make it thru then it is recognized.
      while (ret_value)
      {
         // IF -2 then end search mode
         if (num[i] == -2)
         {
            search_mode = FALSE;
         }
         // ELSE IF -1 then we search for the next character
         else if (num[i] == -1)
         {
            search_mode = TRUE;
            search_ret_value = ret_value - 1;
            search_i = i - 1;
            search_delta = 0;
            while(search_pos + search_delta < 1000)
            {
               // Did we find the character we are looking for
               if (array[search_pos + search_delta] == buf[i])
               {
                  add_delta = search_pos + search_delta;
                  break;
               }
               search_delta++;
            }

            // setup to start search at next character after this one that we found
            search_pos = add_delta + 1;
         }
         else
         {
            // compute next data element to look at
            adj_num = num[i] + add_delta;

            // IF we reached the end of the available data then we are done
            if (adj_num >= (signed int)array.size())
            {
               recognized = FALSE;
               break;
            }

            // compare value we are looking for with data from file
            value = array[adj_num];
            if (value != buf[i])
            {
               // IF we are searching for data somewhere then dont exit yet but restart
               // search again at next position
               if (search_mode == TRUE)
               {
                  ret_value = search_ret_value;
                  i = search_i;
               }
               // ELSE no match so we are done, not recognized
               else
               {
                  recognized = FALSE;
                  break;
               }
            }

            // update the current position
            if (search_mode == FALSE)
               search_pos = add_delta + num[i];
         }

         // Get next pair of data
         ret_value--;
         i++;
      }

      // IF format is recognized then break out of for loop
      if (recognized == TRUE)
      {
         break;
      }
   }

   return recognized;
}


void
aflibFileItem::setFormat(const string& mod_format)
{
   _mod_format = mod_format;
}
 
const string&
aflibFileItem::getFormat() const
{
   return (_mod_format);
}
 
void
aflibFileItem::setDescription(const string& mod_dscr)
{
   _mod_dscr = mod_dscr;
}
 
const string&
aflibFileItem::getDescription() const
{
   return (_mod_dscr);
}
 
void
aflibFileItem::setExtension(const string& mod_ext)
{
   _mod_ext.push_back(mod_ext);
}
 
const list<string>&
aflibFileItem::getExtensions() const
{
   return (_mod_ext);
}
 
void
aflibFileItem::setMagic(const string& mod_magic)
{
   _mod_magic.push_back(mod_magic);
}
 
const list<string>&
aflibFileItem::getMagic() const
{
   return (_mod_magic);
}
  
void
aflibFileItem::setName(const string& mod_name)
{
   _mod_name= mod_name;
}
 
const string&
aflibFileItem::getName() const
{
   return (_mod_name);
}

void
aflibFileItem::setValue1(const string& mod_value1)
{
   _mod_value1 = mod_value1;
}
 
const string&
aflibFileItem::getValue1() const
{
   return (_mod_value1);
}

void
aflibFileItem::setValue2(const string& mod_value2)
{
   _mod_value2= mod_value2;
}
 
const string&
aflibFileItem::getValue2() const
{
   return (_mod_value2);
}

void
aflibFileItem::setValue3(const string& mod_value3)
{
   _mod_value3 = mod_value3;
}
 
const string&
aflibFileItem::getValue3() const
{
   return (_mod_value3);
}


