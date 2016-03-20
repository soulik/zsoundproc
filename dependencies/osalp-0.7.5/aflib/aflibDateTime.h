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

#ifndef _AFLIBDATETIME_H_
#define _AFLIBDATETIME_H_


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <iostream>

#include <string>
using std::string;
using std::ostream;


/*! \class aflibDateTime
    \brief Date Time class.

    This class simply holds a date and time. It can be used by other classes such as the
    aflibAudioRecorder class that requires a date and time. It provides for storing a 
    date and time, printing a date and time, and comparing two dates and times.
*/

class aflibDateTime {

public:

   aflibDateTime ();

   aflibDateTime (
      int  year,
      int  month,
      int  day,
      int  hour,
      int  minute,
      int  second);

   aflibDateTime (
      string date,
      string time);

   ~aflibDateTime();

   bool
   operator < (const aflibDateTime& date) const;

   bool
   operator <= (const aflibDateTime& date) const;

   bool
   operator == (const aflibDateTime& date) const;

   friend ostream&
   operator << (
      ostream& o,
      const aflibDateTime& date);

   void
   setYear(int year);

   int
   getYear() const;

   void
   setMonth(int month);

   int
   getMonth() const;

   void
   setDay(int day);

   int
   getDay() const;

   void
   setHour(int hour);

   int
   getHour() const;

   void
   setMinute(int minute);

   int
   getMinute() const;

   void
   setSecond(int second);

   int
   getSecond() const;

   void
   setCurrentTime();


private:

int  _year;
int  _month;
int  _day;
int  _hour;
int  _minute;
int  _second;

};


#endif
