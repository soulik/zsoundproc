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


#ifndef _AFLIBAUDIOEDIT_H_
#define _AFLIBAUDIOEDIT_H_

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <list>
#include <set>
using std::set;

#include "aflibAudio.h"
#include "aflibEditClip.h"

/*! \class aflibAudioEdit
    \brief Provides functionality for audio editing.

 This is a class that simplifies audio editing. This is derived from the aflibAudio base
 class so it can be used in an audio chain. This is a complex class that aids the 
 application programmer in writing an audio editing program. It can accept multiple
 inputs and allows the user to add audio clip segments, remove audio clip segments, 
 and add or remove inputs. One can use this object in a chain so that the output of
 this object can be output to an audio file or device for output. Once the clip
 information is stored in this class it will automatically get the audio input from
 the correct input and route it to the output. 
 We also provide our own process function instead of using the base classes since
 we need to retrieve data from different inputs. One other current limitiation is that
 if a data segment that is passed to the process function spans clips from different
 inputs then only the input in the furst clip will be used. This will be changed in the
 furture as well.

 ADDING AND REMOVING INPUTS
 One can add or remove inputs to this class. The constructor does require one input.
 An ID number is returned when additional inputs are added. The first input specified
 should be assumed to have an ID of 1. When an input is removed any audio clips that
 are obtained from this input will be removed. If all inputs are removed then FALSE
 will be sent to the enable function of the aflibAudio base class to disable this
 object from the chain. When an input is added this object will be enabled again.
 The functions to add and remove inputs are provided by the aflibChain base class:\n
   int  addParent();\n
   void removeParent();\n
\n
 ADDING AND REMOVING AUDIO SEGMENTS
 Adding and removing segments is the heart of this object. This in effect allows one
 to map inputs to the output of this object. A typical application would be to have
 only one input and then add a segment that contains the entire audio segment of the
 input. Then the user can remove certain audio segments of this input. This makes
 the output look like one continuous audio stream without the segments that have
 been removed. The APIs here allow one to specify segments in either samples or
 segments. If one uses doubles then the specifcation is assumed to be seconds and if
 long long then samples. One can also remove a segment by the segment number. One
 should verify the segment number first though as segment numbers can change after
 segments are added, removed, or inputs removed. One can also remove all segments from
 a specific input. The functions to add and remove segments are: \n
   void addSegment( \n
      int  input, \n
      long long input_start_position, \n
      long long input_stop_position, \n
      long long output_insert_position); \n
   void addSegment( \n
      int  input, \n
      double input_start_seconds, \n
      double input_stop_seconds, \n
      double output_insert_seconds); \n
   void removeSegment(int seg_num); \n
   void removeSegment( \n
      long long output_start_position, \n
      long long output_stop_position); \n
   void removeSegment( \n
      double output_start_seconds, \n
      double output_stop_seconds); \n
   void removeSegmentsFromInput(int input); \n
 \n
 OBTAINING INFORMATION
 Several functions exist to get information on the audio segment data. One can get the
 total number of current segments and then one can obtain the data for any particular
 segment in either seconds or samples. Segments start at number 1. \n
   int getNumberOfSegments(); \n
   void getSegment( \n
      int segment_number, \n
      int&  input, \n
      long long& input_start_position, \n
      long long& input_stop_position, \n
      long long& output_start_position, \n
      long long& output_stop_position); \n
   void getSegment( \n
      int segment_number, \n
      int&  input, \n
      double& input_start_seconds, \n
      double& input_stop_seconds, \n
      double& output_start_seconds, \n
      double& output_stop_seconds); \n
 \n
 UNDO / REDO
 In the future Undo / Redo capability will be added to undo the last audio segment
 addition or deletion.
*/


class aflibAudioEdit : public aflibAudio {

public:

   // Available contructors and destructors
   aflibAudioEdit(aflibAudio& audio);
   ~aflibAudioEdit();

   // Add and remove segments
   void
   addSegment(
      int  input,
      long long input_start_position,
      long long input_stop_position,
      long long output_insert_position,
      double    factor = 1.0);

   void
   addSegment(
      int  input,
      double input_start_seconds,
      double input_stop_seconds,
      double output_insert_seconds,
      double factor = 1.0);

   void
   removeSegment(int seg_num);

   void
   removeSegment(
      long long output_start_position,
      long long output_stop_position);

   void
   removeSegment(
      double output_start_seconds,
      double output_stop_seconds);

   void
   removeSegmentsFromInput(int input);

   // Obtain information
   int
   getNumberOfSegments();

   void
   getSegment(
      int segment_number,
      int&  input,
      long long& input_start_position,
      long long& input_stop_position,
      long long& output_start_position,
      long long& output_stop_position,
      double&    factor);

   void
   getSegment(
      int segment_number,
      int&  input,
      double& input_start_seconds,
      double& input_stop_seconds,
      double& output_start_seconds,
      double& output_stop_seconds,
      double& factor);

   // Perform Undo / Redo of an audio edit
   aflibUndoRedo
   getUndoRedoStatus() const;

   void
   performUndoRedo();

   aflibStatus
   compute_segment(
      list<aflibData *>& data,
      long long position = -1) ;

   aflibData *
   process(
      aflibStatus& ret_status,
      long long position,
      int& num_samples,
      bool free_memory = TRUE) ;

   //! Returns the name of the derived class.
   const char *
   getName() const { return "aflibAudioEdit";};

   // Callbacks from afChain
   void
   parentWasDestroyed(int parent_id);

   void
   parentWasAdded(int parent_id) ;

   void
   setInputConfig(const aflibConfig& cfg);

   bool
   isDataSizeSupported(aflib_data_size size);
 
   bool
   isEndianSupported(aflib_data_endian end);
 
   bool
   isSampleRateSupported(int& rate);

   bool
   isChannelsSupported(int& channels);

private:

   aflibAudioEdit();

   aflibAudioEdit(const aflibAudioEdit& op);

   const aflibAudioEdit&
   operator=(const aflibAudioEdit& op);

   void
   printClips();

   void
   recomputeConfig();

   // Add and remove inputs
   void
   addInput(int input);

   void
   removeInput(int input);

list<int>  _input_array;
set< aflibEditClip, less < aflibEditClip > > _clip_array;

};


#endif
