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

#include "aflibAudioFile.h"
#include "aflibFile.h"

#include <stdio.h>
#include <stdlib.h>


/*! \brief Constructor used to read a device or file.
*/
aflibAudioFile::aflibAudioFile(
   aflibFileType type_enum,
   const string& file,
   aflibConfig* cfg,
   aflibStatus* status) : aflibAudio()
{
   aflibConfig  config;

   if (cfg != NULL)
   {
      config = *cfg;
   }

   _has_parent = FALSE;
   _file_object = aflibFile::open(type_enum, file, &config, status);
   if (_file_object != NULL)
   {
      setOutputConfig(config);

      if (cfg != NULL)
      {
         *cfg = config;
      }
      }
}

/*! \brief Constructor used to read a device or file.
*/
aflibAudioFile::aflibAudioFile(
   const string& format,
   const string& file,
   aflibConfig* cfg,
   aflibStatus* status) : aflibAudio()
{
   aflibConfig  config;

   if (cfg != NULL)
   {
        config = *cfg;
   }

   _has_parent = FALSE;
   _file_object = aflibFile::open(format, file, &config, status);
   if (_file_object != NULL)
   {
      setOutputConfig(config);

      if (cfg != NULL)
      {
         *cfg = config;
      }
   }
}


/*! \brief Constructor used to write to a device or file.
*/
aflibAudioFile::aflibAudioFile(
   aflibAudio& audio,
   aflibFileType type_enum,
   const string& file,
   aflibConfig* cfg,
   aflibStatus* status) : aflibAudio(audio)
{
   _has_parent = TRUE;
   _file_object = aflibFile::create(type_enum, file, *cfg, status);

   if (_file_object != NULL)
   {
		if (cfg != NULL)
		{
			setInputConfig(*cfg);
		}
	}
}


/*! \brief Constructor used to write to a device or file.
*/
aflibAudioFile::aflibAudioFile(
   aflibAudio& audio,
   const string& format,
   const string& file,
        aflibConfig* cfg,
   aflibStatus* status) : aflibAudio(audio)
{
   _has_parent = TRUE;
   _file_object = aflibFile::create(format, file, *cfg, status);

   if (_file_object != NULL)
   {
		if (cfg != NULL)
		{
			setInputConfig(*cfg);
		}
	}
}


/*! \brief Destructor.
*/
aflibAudioFile::~aflibAudioFile()
{
   delete _file_object;
}


aflibStatus
aflibAudioFile::compute_segment(
   list<aflibData *>& data,
   long long position)
{
   aflibStatus ret_status;

   if (_has_parent == TRUE)
   {
      ret_status = _file_object->afwrite(**(data.begin()), position);
        }
   else
   {
      ret_status = _file_object->afread(**(data.begin()), position);
    }
   return (ret_status);
}


bool
aflibAudioFile::setItem(
   const char * item,
   const void * value)
{
   return (_file_object->setItem(item, value)); 
}


bool
aflibAudioFile::getItem(
   const char * item,
   void * value)
{
   return (_file_object->getItem(item, value)); 
}


/*! \brief Gets the format of the audio file.

     This will retrieve the format of the current file being written to. This is
     usefull when reading a file in AUTO mode. This will tel you the format that
     is being used.
*/
const string&
aflibAudioFile::getFormat() const
{
   return (_file_object->getFormat());
}

void
aflibAudioFile::setInputConfig(const aflibConfig& cfg)
{
   // This function overrides the virtual function in the aflibAudio base class.
   // We do this so that we can read the audio configuration data. We then
   // recalculate any data that is dependant on the audio configuration
 

   _file_object->setInputConfig(cfg); 
}

const aflibConfig&
aflibAudioFile::getInputConfig() const
{
   return (_file_object->getInputConfig());
}

void
aflibAudioFile::setOutputConfig(const aflibConfig& cfg)
{
   _file_object->setOutputConfig(cfg);
}
 
const aflibConfig&
aflibAudioFile::getOutputConfig() const
{
   return (_file_object->getOutputConfig());
}

bool
aflibAudioFile::isDataSizeSupported(aflib_data_size size)
{
   return(_file_object->isDataSizeSupported(size));
}
 
bool
aflibAudioFile::isEndianSupported(aflib_data_endian end)
{
   return(_file_object->isEndianSupported(end));
}
 
bool
aflibAudioFile::isSampleRateSupported(int& rate)
{
   return(_file_object->isSampleRateSupported(rate));
}

bool
aflibAudioFile::isChannelsSupported(int& channels)
{
   return(_file_object->isChannelsSupported(channels));
}



