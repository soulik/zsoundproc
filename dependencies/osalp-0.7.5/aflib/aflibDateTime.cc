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
#include <time.h>

#include "aflib.h"
#include "aflibDateTime.h"


/*! \brief Constructor with no parameters.
*/
aflibDateTime::aflibDateTime() :
   _year(-1),
   _month(-1),
   _day(-1),
   _hour(-1),
   _minute(-1),
   _second(-1)
{
}


/*! \brief Constructor with parameters as integer values.
*/
aflibDateTime::aflibDateTime (
   int  year,
   int  month,
   int  day,
   int  hour,
   int  minute,
   int  second) :
   _year(year),
   _month(month),
   _day(day),
   _hour(hour),
   _minute(month),
   _second(second)
{
}


/*! \brief Constructor with parameters as strings.

    This constructor takes a date and a time as a string. The date must
    be in the format MM/DD/YYYY. The time must be in the format HH:MM:SS.
*/
aflibDateTime::aflibDateTime (
   string date,
   string time)
{
   string temp_str;

   temp_str = date.substr(0, 2);
   _month = atoi(temp_str.c_str());

   temp_str = date.substr(3, 2);
   _day = atoi(temp_str.c_str());

   temp_str = date.substr(6, 4);
   _year = atoi(temp_str.c_str());

   temp_str = time.substr(0, 2);
   _hour = atoi(temp_str.c_str());

   temp_str = time.substr(3, 2);
   _minute = atoi(temp_str.c_str());

   temp_str = time.substr(6, 2);
   _second = atoi(temp_str.c_str());
}


/*! \brief Destructor.
*/
aflibDateTime::~aflibDateTime()
{
}


/*! \brief Comparision operator.

    This operator will determine if one date and time is less than another date and
    time.
*/
bool
aflibDateTime::operator < (const aflibDateTime& date) const
{
   if (getYear() < date.getYear())
      return (TRUE);
   else if (getYear() > date.getYear())
      return (FALSE);

   if (getMonth() < date.getMonth())
      return (TRUE);
   else if (getMonth() > date.getMonth())
      return (FALSE);

   if (getDay() < date.getDay())
      return (TRUE);
   else if (getDay() > date.getDay())
      return (FALSE);

   if (getHour() < date.getHour())
      return (TRUE);
   else if (getHour() > date.getHour())
      return (FALSE);

   if (getMinute() < date.getMinute())
      return (TRUE);
   else if (getMinute() > date.getMinute())
      return (FALSE);

   if (getSecond() < date.getSecond())
      return (TRUE);
   else if (getSecond() > date.getSecond())
      return (FALSE);

   return (FALSE);
   
}


bool
aflibDateTime::operator == (const aflibDateTime& date) const
{
  if (getYear() == date.getYear() && getMonth() == date.getMonth() &&
      getDay() == date.getDay() && getHour() == date.getHour() &&
      getMinute() == date.getMinute() && getSecond() == date.getSecond())
    return (TRUE);
  else
    return (FALSE);
}


bool
aflibDateTime::operator <= (const aflibDateTime& date) const
{
  if (*this < date || *this == date)
    return (TRUE);
  else
    return (FALSE);
}


/*! \brief Outputs data and time to a stream.

     This will output the date and time to a stream in the format MM/DD/YYYY and
     HH:MM:SS.
*/
ostream&
operator << (
   ostream& o,
   const aflibDateTime& date)
{
   o << date.getMonth() << "/" << date.getDay() << "/" << date.getYear() << " ";
   o << date.getHour() << ":" << date.getMinute() << ":" << date.getSecond();

   return (o);
}


/*! \brief Sets the year.
*/
void
aflibDateTime::setYear(int year)
{
   _year = year;
}


/*! \brief Gets the year.
*/
int
aflibDateTime::getYear() const
{
   return(_year);
}


/*! \brief Sets the month.
*/
void
aflibDateTime::setMonth(int month)
{
   _month = month;
}


/*! \brief Gets the month.
*/
int
aflibDateTime::getMonth() const
{
   return(_month);
}


/*! \brief Sets the day.
*/
void
aflibDateTime::setDay(int day)
{
   _day = day;
}


/*! \brief Gets the day.
*/
int
aflibDateTime::getDay() const
{
   return(_day);
}


/*! \brief Sets the hour.
*/
void
aflibDateTime::setHour(int hour)
{
   _hour = hour;
}


/*! \brief Gets the hour.
*/
int
aflibDateTime::getHour() const
{
   return(_hour);
}


/*! \brief Sets the minute.
*/
void
aflibDateTime::setMinute(int minute)
{
   _minute = minute;
}


/*! \brief Gets the minute.
*/
int
aflibDateTime::getMinute() const
{
   return(_minute);
}


/*! \brief Sets the second.
*/
void
aflibDateTime::setSecond(int second)
{
   _second = second;
}


/*! \brief Gets the second.
*/
int
aflibDateTime::getSecond() const
{
   return(_second);
}


/*! \brief Sets the date and time to the current date and time.
*/
void
aflibDateTime::setCurrentTime()
{
   time_t unix_time;
   struct tm * tm_time;

   // Get the current date and time
   time(&unix_time);
   tm_time = localtime(&unix_time);

   // Set all the fields in our date time structure
   setYear(tm_time->tm_year + 1900);
   setMonth(tm_time->tm_mon + 1);
   setDay(tm_time->tm_mday);
   setHour(tm_time->tm_hour);
   setMinute(tm_time->tm_min);
   setSecond(tm_time->tm_sec);

}

