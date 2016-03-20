/*
 * Copyright: (C) 2002 Jeroen Versteeg
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
 *   Jeroen Versteeg            j.m.versteeg@student.utwente.nl
 *
 */

/*! \class aflibOggFile
    \brief Derived class to read Ogg Vorbis audio files.
 
  This class will read audio files of the Ogg (Ogg Vorbis
  File Format) format.
*/


#ifndef _AFLIBOGGFILE_H
#define _AFLIBOGGFILE_H

#include "aflibFile.h"

class aflibBufferUtils;

class aflibOggFile : public aflibFile {

public:
   aflibOggFile();
   ~aflibOggFile();
   aflibStatus afopen(const char * file, aflibConfig* cfg);
   aflibStatus afcreate(const char * file, const aflibConfig& cfg);
   aflibStatus afread(aflibData& data, long long position = -1);
   aflibStatus afwrite( aflibData& data, long long position = -1);
   bool isDataSizeSupported(aflib_data_size size);
   bool isEndianSupported(aflib_data_endian end);
   bool isSampleRateSupported(int& rate);

private:
	int            _handle;
	FILE	  	*_stream;
	OggVorbis_File _ogg_handle;
        vorbis_info    *_vi;
	long long     	_total_samples;
	long long     	_cur_position;
        aflibBufferUtils *_buf;

};


#endif
