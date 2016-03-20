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


// class for MP3 audio file reading 
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream.h>
#include <string>
#include <signal.h>


#include "aflibMpg123File.h"
#include "aflibData.h"

#include "aflibFileItem.h"
	
#define MODULE_NAME "aflibMpgFile"

#define LAYER1_FRAME_SIZE	384
#define LAYER2_FRAME_SIZE	1152
#define TEMP_DIR			"/usr/tmp/"
#define IN_PIPE			"mpg123-in-XXXXXX"
#define OUT_PIPE			"mpg123-out-XXXXXX"
#define MPG123				"mpg123"


extern "C"
{
   aflibFile *
   getAFileObject() { return ((aflibFile *)new aflibMpg123File()); }

	void
	query(list<aflibFileItem*>& support_list)
	{

		aflibFileItem* item;
		item	= new aflibFileItem();

		item->setFormat("MPEG");
		item->setDescription("mpg123 Wrapper");
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


aflibMpg123File::aflibMpg123File():
_fd(0),
_sample_rate(0),
_depth(0),
_channels(0),
_current_sample(0),
_last_frame(0),
_samples_per_frame(0),
_version(0),
_blkalign(0),
_mpeg_cmd("mpg123 -q -s -k")
{ }

aflibMpg123File::~aflibMpg123File()
{
   // Should be moved to a close function
	if(_fd != NULL) pclose(_fd);
}

aflibStatus
aflibMpg123File::afopen(
   const char * file,
   aflibConfig* cfg)
{
   // This function needs to be redone to find the header info correctly
   // This function will open an existing MP3 file.

  	string cmd_str, temppath;
  	aflibConfig  input_cfg(*cfg);
  	char  buf[1024];
	int b_itt = 0;
	char temp[] = "mpeg-XXXXXX";
	int this_frame = 0;
	long info_size = 0;
   aflibData  data(1);

   if ( mktemp(temp) == NULL ) return (AFLIB_ERROR_OPEN);
	temppath = TEMP_DIR; 
	temppath += temp;	
   /* Route stderr to the temp file as 2>&1 doesn't seem to work with mpg123 */
	
	/* Patch by Jeroen Versteeg (j.m.versteeg@student.utwente.nl):
	 * enclose the filename in quotes, because otherwise it can't handle "strange" filenames (i.e. with spaces, quotes, etc.)
	 * Most probably other modules need to be patched in the same way, but for me this is not a core-activity ;)
	 */
	 
	cmd_str = MPG123;
	cmd_str += " -n1 -v -t ";
	cmd_str += "\"";
	cmd_str += file;
	cmd_str += "\"";
	cmd_str += " 2> ";
	cmd_str += temppath ;

	system(cmd_str.c_str());

   if ((_fd = fopen(temppath.c_str(), "r")) == NULL ) return (AFLIB_ERROR_OPEN);

   /* Read info from temp file */
	info_size = fread(buf,1,1024,_fd);	
	/* Close and delete temp file */
	fclose(_fd);
	_fd = NULL;
	unlink(temppath.c_str());

	/* Skip past copyright header */
	for(int i=0; i< info_size; i++ ){
		if(buf[i] == '\n' && buf[i+1] == '\n'){ b_itt = i+2; break;}
	}

	for(int i=b_itt; i< info_size; i++ ){
		if(( !strncmp(&buf[i],"MPEG 1",6) )&&( !_version )){
			_version = 1;
		} else if(( !strncmp(&buf[i],"MPEG 2",6) )&&( !_version )){
			_version = 2;
		} else if(( !strncmp(&buf[i],"Frame#",6) )&&( !_last_frame )){
			while( !isdigit(buf[i])&&(i < info_size)) i++ ;
			this_frame = strtol(&buf[i],NULL,10);
			while( isdigit(buf[i])&&(i < info_size)) i++ ;
			while( !isdigit(buf[i])&&(i < info_size)) i++ ;
			_last_frame = strtol(&buf[i],NULL,10) + this_frame;
		} else if(( !strncmp(&buf[i],"encoding: signed 16 bit",23) )&&
																( !_depth )){
			_depth = 16;
			input_cfg.setSampleSize(AFLIB_DATA_16S);
		} else if(( !strncmp(&buf[i],"encoding: unsigned 8 bit",24) )&&
																( !_depth )){
			 _depth = 8;
			input_cfg.setSampleSize(AFLIB_DATA_8U);
		} else if(( !strncmp(&buf[i],"rate:",5) )&&( !_sample_rate )){
			while( !isdigit(buf[i])&&(i < info_size)) i++ ;
			_sample_rate = strtol(&buf[i],NULL,10);
			/* No we don't want the Bitrate: */
			if( _sample_rate < 8000 ) _sample_rate = 0;
			else input_cfg.setSamplesPerSecond(_sample_rate);
		} else if(( !strncmp(&buf[i],"channels:",9) )&&(!_channels )){
			while( !isdigit(buf[i])&&(i < info_size)) i++ ;
			_channels = strtol(&buf[i],NULL,10);
			input_cfg.setChannels(_channels);
		} else if(( !strncmp(&buf[i],"Layer: I,",9) )&&( !_samples_per_frame )){
			_samples_per_frame = LAYER1_FRAME_SIZE;
		} else if(( !strncmp(&buf[i],"Layer: II,",10) )&&( !_samples_per_frame )){
				_samples_per_frame = LAYER2_FRAME_SIZE;
		} else if(( !strncmp(&buf[i],"Layer: III,",11) )&&(!_samples_per_frame )){
				_samples_per_frame = LAYER2_FRAME_SIZE;
		}
	}

   input_cfg.setDataOrientation(AFLIB_INTERLEAVE);
   input_cfg.setDataEndian(data.getHostEndian());

   if (getenv("AFLIB_DEBUG"))
   {
	printf("_version = %i, _samples_per_frame = %i, _last_frame = %i\n _depth = %i, rate = %i, ch= %i |n",
			_version,(int)_samples_per_frame,(int)_last_frame,_depth,_sample_rate,_channels);
   }

	/* If everything was found then we're cool */
	if (_version && _samples_per_frame && _last_frame && _depth && _sample_rate && _channels ){

		_blkalign = _depth == 16 ? 2 : 1;
		_blkalign *= _channels;
	/* I'm still trying to figure this one out */
		_samples_per_frame /= _version;
		input_cfg.setTotalSamples(_last_frame*_samples_per_frame);

      // Set input and output audio configuration data
      setInputConfig(input_cfg);
  	  	setOutputConfig(input_cfg);
      if (getenv("AFLIB_DEBUG"))
      {
		   printf("_version = %i, _samples_per_frame = %i, _last_frame = %i\n",
			_version,(int)_samples_per_frame,(int)_last_frame);
      }

	/* Patch by Jeroen Versteeg (j.m.versteeg@student.utwente.nl):
	 * enclose the filename in quotes, because otherwise it can't handle "strange" filenames (i.e. with spaces, quotes, etc.)
	 */
		_filename = "\"";
		_filename += file;
		_filename += "\"";
  	} else {
     	return(AFLIB_ERROR_OPEN);
  	}

  	return (AFLIB_SUCCESS);
}

aflibStatus
aflibMpg123File::afcreate(
   const char * file,
   const aflibConfig& cfg)
{
   return( AFLIB_ERROR_UNSUPPORTED );
}

aflibStatus
aflibMpg123File::afread(
   aflibData& data,
   long long position )
{

   void * p_data;
   long   total_length;
   long   new_length = 0;
   aflibStatus  status = AFLIB_SUCCESS;

   data.setConfig(getInputConfig());
   total_length = data.getTotalAdjustLength();
   p_data = data.getDataPointer();

	/* Only reopen pipe if position changes from last read*/
	if(position != -1){
		if(_current_sample != position){
			_current_sample = position;
			if(_fd != NULL){
				//fflush(_fd);
				pclose(_fd);
				_fd = NULL;
			}
		}
	} 
	/*This is slow.  Mainly when aflibSampleData calls it. But for the mp3
		files I have editor doesn't open them at all so... */
	if(_fd == NULL){
		/* Find out what frame the position is at */
		long skip_frames = _current_sample/_samples_per_frame;

		/* Setup the command string */
		char myframe[20];
		sprintf(myframe,"%li ",skip_frames);
   	string cmd_str = _mpeg_cmd;
		cmd_str += myframe;
		cmd_str += _filename;
		cmd_str += " 2>/dev/null";

		if((_fd = popen(cmd_str.c_str(),"r")) == NULL )
      		return (AFLIB_ERROR_OPEN);
	/* Skip past the samples we don't want */
		long long unwanted = (_current_sample % _samples_per_frame)*_blkalign; 
		if (unwanted) for(long long i = 0 ; i < unwanted; i++) fgetc(_fd);
	}

	new_length = fread(p_data, 1,total_length, _fd);

	_current_sample += new_length/_blkalign;

   if (new_length != total_length)
   {
		data.adjustLength(new_length/_blkalign);
   	if (new_length == 0)
   	{
			if( _fd ) pclose( _fd ); 
			_fd = NULL; 
			_current_sample = 0;
      	status = AFLIB_END_OF_FILE;
   	}
   }

   return(status);
}

aflibStatus
aflibMpg123File::afwrite(
   aflibData& data,
   long long position )
{
   return( AFLIB_ERROR_UNSUPPORTED );
}

bool
aflibMpg123File::isDataSizeSupported(aflib_data_size size)
{
   return (size == AFLIB_DATA_16S);
}

bool
aflibMpg123File::isEndianSupported(aflib_data_endian end)
{
   return (end == AFLIB_ENDIAN_LITTLE);
}

bool
aflibMpg123File::isSampleRateSupported(int& rate)
{
   bool ret_value = FALSE;
 
 
   // If not yet allocated then 
   if (_sample_rate == 0)
      return (rate == 22050 || rate == 44100 || rate == 16000);

   // IF same rate then TRUE else return desired rate
   if (rate == _sample_rate)
      ret_value = TRUE;
   else
      rate = _sample_rate;
 
   return (ret_value);
}

/*I had to put this in here.  When the pipe gets opened from a thread
then the thread gets killed or dies the stdout goes to the soundcard
 */
bool
aflibMpg123File::setItem(const char* item, const void* value)
{
	if(!strcmp(item,"close") && ((bool)&value == TRUE)) {
		if(_fd){
			pclose(_fd); 
			_fd= NULL;
		}
	} 
	return TRUE;
}
