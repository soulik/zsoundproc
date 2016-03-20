/*
 * Copyright: (C) 2000 Bruce W. Forsberg
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


// class for MP3 audio file reading 
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream.h>
#include <string>

#include "aflibMpgFile.h"
#include "aflibFileItem.h"
#include "aflibData.h"
#include "mpegsound.h"
	
#define MODULE_NAME "aflibMpgFile"

extern "C"
{
   aflibFile *
   getAFileObject() { return ((aflibFile *)new aflibMpgFile()); }

	void
	query(list<aflibFileItem*>& support_list)
	{

		aflibFileItem* item;
		item	= new aflibFileItem();
	
		item->setFormat("MPEG");
		item->setDescription("MPEG 1.0/2.0 Layer I/II/III");
		item->setExtension(".mpg");
		item->setExtension(".mp3");
		item->setExtension(".mp2");
		item->setName(MODULE_NAME);
		item->setMagic("0(R), 1(I), 2(F), 3(F), 8(W), 9(A), 10(V), 11(E), -1(d), 1(a), 2(t), 3(a), -2(a), 8(ÿ)");
		item->setMagic("0(I), 1(D), 2(3)");
		item->setMagic("0(ÿ)");
		support_list.push_back(item);

	}

} 


aflibMpgFile::aflibMpgFile() : _bufpos(0),_buflen(0),_current_frame(0)
{
}

aflibMpgFile::~aflibMpgFile()
{
	if(_decoder) delete _decoder;
}

aflibStatus
aflibMpgFile::afopen(
   const char * file,
   aflibConfig* cfg)
{
   // This function needs to be redone to find the header info correctly
   // This function will open an existing MP3 file.

   aflibStatus  status = AFLIB_SUCCESS;
   aflibConfig  input_cfg;
   aflibData    data(1);

	Soundinputstream* loader;
	int err;

	if((loader = Soundinputstream::hopen((char*)file,&err)) == NULL)
	{
      status = AFLIB_ERROR_OPEN;
      return (status);
   }

	_buffer = new short[RAWDATASIZE*sizeof(short)];

	_decoder = new Mpegtoraw(loader);

	if(_decoder == NULL){
		delete loader;
		return (AFLIB_ERROR_OPEN);
        }

	_decoder->initialize((char*)file);

	if(_decoder->isstereo())
		_channels = 2;
	else
		_channels = 1;

  	input_cfg.setChannels(_channels);
	_samples_per_frame = _decoder->run(_buffer,1);
	if(_samples_per_frame == 0){
		delete loader;
		delete _decoder;
		_decoder = NULL;
		return (AFLIB_ERROR_OPEN);
	}

	_samples_per_frame /= _decoder->getcurrentframe();
//	_samples_per_frame /= _channels;
if (getenv("AFLIB_DEBUG"))
	cerr << "frames done: " << _decoder->getcurrentframe();

 
   // Need to set this better
//   if (bits == 8)
//   {
//      input_cfg.setSampleSize(AFLIB_DATA_8U);
//   }
//   else if (bits == 16)
//   {
   input_cfg.setSampleSize(AFLIB_DATA_16S);
//   }
   input_cfg.setSamplesPerSecond(_decoder->getfrequency());
	_total_frames = _decoder->gettotalframe();
	input_cfg.setTotalSamples(_samples_per_frame*_total_frames/_channels);
   input_cfg.setDataOrientation(AFLIB_INTERLEAVE);
   input_cfg.setDataEndian(data.getHostEndian());
if (getenv("AFLIB_DEBUG"))
{
	cerr << "_samples_per_frame: " << _samples_per_frame << endl;
	cerr << "_channels: " << _channels << endl;
	cerr << "_total_frames: " << _total_frames << endl;
}

   // Set input and output audio configuration data
   setInputConfig(input_cfg);
   setOutputConfig(input_cfg);

    return (status);
}

aflibStatus
aflibMpgFile::afcreate(
   const char * file,
   const aflibConfig& cfg)
{
   aflibStatus  status = AFLIB_SUCCESS;

   status = AFLIB_ERROR_UNSUPPORTED;
   return(status);
}

aflibStatus
aflibMpgFile::afread(
   aflibData& data,
   long long position )
{
   // Read data from a wave file. A -1 for position means read from the
   // current position.

   short * p_data;
   long   total_length;
   long   new_length = 0;
   int    frame_no = 0;
   aflibStatus  status = AFLIB_SUCCESS;

   data.setConfig(getInputConfig());
   total_length = data.getLength()*_channels;
   p_data = (short*)data.getDataPointer();

	if(position != -1){
		// get remainder
		_bufpos = (_channels*position) % _samples_per_frame;
		// give me a nice whole number - no rounding errors here buddy
		frame_no = (position*_channels - _bufpos)/_samples_per_frame;

                // IF we are beyond last frame then we are done
                if (frame_no > _decoder->gettotalframe())
            {
                   status = AFLIB_END_OF_FILE;
            }
                else if(_current_frame != frame_no)
                {
			// load and decode frame if not yet done. 
			_decoder->setframe(frame_no);
			_current_frame = _decoder->getcurrentframe();
			_buflen = _decoder->run(_buffer,1); 
		}
	}

	while(new_length < total_length){
		if (_bufpos == _buflen){
			_current_frame = _decoder->getcurrentframe();
			_buflen = _decoder->run(_buffer,1);
			_bufpos = 0;
			if(_buflen == 0 )
			{
if (getenv("AFLIB_DEBUG"))
{
				cerr << "Ouch!: failed read on frame: "<< frame_no << endl;
}
				break;
			}
		}
		*(p_data + new_length++) = _buffer[_bufpos++];
	}
		
   if (new_length != total_length)
   {
		data.adjustLength(new_length/_channels);

		if(!new_length){ 
			// premature EOF so reset total length
			_total_frames = _current_frame - 1;

			// Ugh - Does _input_cfg have to be private in aflibAudio ?
			aflibConfig input = getInputConfig();
			input.setTotalSamples(_samples_per_frame*_total_frames/_channels);
   		setInputConfig(input);
   		setOutputConfig(input);
			// End Ugh

      	status = AFLIB_END_OF_FILE;
		}

   }

   return(status);
}

aflibStatus
aflibMpgFile::afwrite(
   aflibData& data,
   long long position )
{
   aflibStatus  status = AFLIB_SUCCESS;


   status = AFLIB_ERROR_UNSUPPORTED;
   return(status);
}

bool
aflibMpgFile::isDataSizeSupported(aflib_data_size size)
{
   return (size == AFLIB_DATA_16S);
}

bool
aflibMpgFile::isEndianSupported(aflib_data_endian end)
{
   return (end == AFLIB_ENDIAN_LITTLE);
}

bool
aflibMpgFile::isSampleRateSupported(int& rate)
{
   bool ret_value = FALSE;
 
 
   // If not yet allocated then 
  // if (_fd == 0)
      return (rate == 22050 || rate == 44100 || rate == 16000);

   // IF same rate then TRUE else return desired rate
//   if (rate == _sample_rate)
      ret_value = TRUE;
 //  else
// rate = _sample_rate;
 
   return (ret_value);
}

