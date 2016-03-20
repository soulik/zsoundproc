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
 *   Jeroen Versteeg		j.m.versteeg@student.utwente.nl
 *
 */

// class for Ogg audio file reading and writing (latter not implemented yet)
// (modelled after aflibWavFile by Bruce Forsberg)

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <iostream.h>

#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>

#include "aflibOggFile.h"
#include "aflibBufferUtils.h"
#include "aflibFileItem.h"
#include "aflibData.h"

#define MODULE_NAME "aflibOggFile"
#define OGG_READ_SIZE 1024

extern "C" {
	aflibFile *getAFileObject() { 
		return ((aflibFile *)new aflibOggFile()); 
	}

	void query(list<aflibFileItem*>& support_list) {
		aflibFileItem* item;
		item	= new aflibFileItem();

                if (getenv("AFLIB_DEBUG"))
		   printf("aflibOggFile::query()\n");
		item->setFormat("Ogg");
		item->setDescription("Ogg Vorbis File Format");
		item->setExtension(".ogg");
		item->setName(MODULE_NAME);
		item->setMagic("0(O), 1(g), 2(g), 3(S)");
		support_list.push_back(item);
   }
}


aflibOggFile::aflibOggFile() {
	_handle = 0;
	_stream = NULL;
	_total_samples = 0;
	_cur_position = 0;
        _buf = new aflibBufferUtils();
        _buf->init(500000);
        if (getenv("AFLIB_DEBUG"))
	   printf("aflibOggFile::aflibOggFile()\n");
}


aflibOggFile::~aflibOggFile() {
   if (_handle != 0) {
		ov_clear(&_ogg_handle);
                _handle = 0;
   }
   if (_buf != NULL)
      delete _buf;
}


aflibStatus aflibOggFile::afopen(const char * file, aflibConfig* cfg) {
   aflibStatus  status = AFLIB_SUCCESS;
   aflibConfig  input_cfg;

	_stream = fopen(file, "r");
   if (_stream == NULL) {
      status = AFLIB_ERROR_OPEN;
      return (status);
   }

   // Store user specified results as defaults
   if (cfg != NULL) {
      input_cfg = *cfg;
   }

	if(ov_open(_stream, &_ogg_handle, NULL, 0) < 0) {
      status = AFLIB_ERROR_OPEN;
      return (status);
	}
        _handle = 1;
	
	// temp extra info:
	/* Throw the comments plus a few lines about the bitstream we're decoding */
	char **ptr=ov_comment(&_ogg_handle,-1)->user_comments;
	_vi=ov_info(&_ogg_handle,-1);
        if (getenv("AFLIB_DEBUG"))
        {
	   while(*ptr){
		   fprintf(stderr,"%s\n",*ptr);
		   ++ptr;
	   }
	   fprintf(stderr,
              "\nBitstream is %d channel, %ldHz\n",_vi->channels,_vi->rate);
	   fprintf(stderr,
              "\nDecoded length: %ld samples\n", (long)ov_pcm_total(&_ogg_handle,-1));
	   fprintf(stderr,
              "Encoded by: %s\n\n",ov_comment(&_ogg_handle,-1)->vendor);
        }
	
   input_cfg.setDataOrientation(AFLIB_INTERLEAVE);
   input_cfg.setDataEndian(AFLIB_ENDIAN_LITTLE);
   input_cfg.setChannels(_vi->channels);
   input_cfg.setSampleSize(AFLIB_DATA_16S);
   _total_samples = ov_pcm_total(&_ogg_handle,-1);
   input_cfg.setTotalSamples(_total_samples);
   input_cfg.setSamplesPerSecond(_vi->rate);

   // Set input and output audio configuration
   setInputConfig(input_cfg);
   setOutputConfig(input_cfg);

   return (status);
}


aflibStatus aflibOggFile::afcreate( const char * file, const aflibConfig& cfg) {
   aflibStatus  status = AFLIB_ERROR_UNSUPPORTED;
   return(status);
}


aflibStatus aflibOggFile::afread(aflibData& data, long long position ) {
   void 				*p_data;
   long  			new_length, old_length;
	int				current_section;
   aflibStatus  	status = AFLIB_SUCCESS;
   long			total_length;
   unsigned char temp_buf[OGG_READ_SIZE];
	
   data.setConfig(getInputConfig());
   p_data = data.getDataPointer();
   total_length = data.getTotalLength();

   // IF not enough buffer space then lower length of data
   if (total_length > _buf->getBufferSize() - 1)
   {
      data.adjustTotalLength(_buf->getBufferSize() - 1);
      total_length = data.getTotalAdjustLength();
   }                                                                                  

   // Seek to correct position
   if (position != -1)
   {
      old_length = _buf->bytes_available() / 2 / getInputConfig().getChannels();
      // ov_pcm_seek is an expensive call so only call if needed
      if ((position+old_length) != (long long)ov_pcm_tell(&_ogg_handle))
         ov_pcm_seek(&_ogg_handle, position+old_length);
   }

	// Wait for a buffer to have data
	while (_buf->bytes_available() < total_length)
	{
		old_length = ov_read(&_ogg_handle,
			(char*)temp_buf, OGG_READ_SIZE, 0, 2, 1, &current_section);
        // Break out if no more data
        if (old_length == 0)
            break;
		_buf->write(temp_buf, old_length);
   }

   // IF we could not read enough data to fill buffer
   if (_buf->bytes_available() < total_length)
      total_length = _buf->bytes_available();

   _buf->read((unsigned char *)p_data, (int)total_length);
   // Set exactly how many samples read
   new_length = total_length / 2 / getInputConfig().getChannels();

   // IF we reached the end of the file then return error
   if (new_length == 0) {
      status = AFLIB_END_OF_FILE;
      data.adjustLength(0);
   }
   // IF we read the last chunk
   else if (new_length != data.getLength()) {
      // When we only have a partial read then readjust the length of data
      data.adjustLength(new_length);
   }

   return(status);
}


aflibStatus aflibOggFile::afwrite(aflibData& data, long long position ) {
   aflibStatus  status = AFLIB_ERROR_UNSUPPORTED;
   return(status);
}

bool aflibOggFile::isDataSizeSupported(aflib_data_size size) {

   return (size == AFLIB_DATA_16S);
}
 

bool aflibOggFile::isEndianSupported(aflib_data_endian end) {

   return (end == AFLIB_ENDIAN_LITTLE);
}


bool aflibOggFile::isSampleRateSupported(int& rate) {
   int value;
   double double_value;
   bool ret_value = FALSE;

   // If handle not yet allocated then indicate any sample rate is supported
   if (_handle == 0)
      return (TRUE);

   // Get the rate of the data
   double_value = (double)_vi->rate;
   value = (int)(double_value + 0.5);

   // IF same rate then TRUE else return desired rate
   if (value == rate)
      ret_value = TRUE;
   else
      rate = value;

   return (ret_value);
}
