#ifndef _AUDIO_H_
#define _AUDIO_H_

#include "aflibConfig.h"
#include "aflibAudio.h"
#include <string>

extern Boolean
work_app(XtPointer client_data);

void
audio_play(char * _play);

void
audio_record(
   const char * _play,
   aflibConfig& config,
   const string& format);

void
audio_timer_record(
   aflibAudio * base_ptr);

void
audio_pause();

void
audio_pitch();

void
audio_stop();

void
audio_device( const char * device );

void
audio_set_position(int pos);

void
audio_set_pitch(double factor);

#endif
