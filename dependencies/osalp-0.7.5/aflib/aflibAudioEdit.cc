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
#include <iostream>

using std::cerr;
using std::cout;
using std::endl;

#include "aflibAudioEdit.h"
#include "aflibData.h"


/*! \brief Constructor - requires an aflibAudio object.

    User must create this object with at least one input. The ID for this
    input can be assumed to be 1.
*/
aflibAudioEdit::aflibAudioEdit(aflibAudio& audio) : aflibAudio(audio)
{
}


/*! \brief Destructor.
*/
aflibAudioEdit::~aflibAudioEdit()
{
   // Erase all clips
   if (_clip_array.size() != 0)
   {
      _clip_array.erase( _clip_array.begin(), _clip_array.end());
   }
}


void
aflibAudioEdit::addInput(int input)
{
   // This function will add another input into this edit class. This will
   // allow the user to create an output from multiple inputs. 
   // If this is the first input added then this object will be enabled
   // with a call to enable.


   _input_array.push_back(input);

   if (_input_array.size() == 1)
   {
      enable(TRUE);
   }
}

void
aflibAudioEdit::removeInput(int input)
{
   // This function will remove an input from this object. If this is the last input
   // then this object will be disabled with a call to enable. First thing that will
   // be done is that all clips with this input will be removed.


   // remove all segments for this input
   removeSegmentsFromInput(input);

   // remove this input from list
   _input_array.remove(input);

   if (_input_array.size() == 0)
   {
      enable(FALSE);
   }
}


/*! \brief Add an audio clip using samples.

    This function will add a new segment at the position specified. If will push
    out all data after this insertion point in the existing audio clip array. This
    function allows one to specify the positions as samples.
*/
void
aflibAudioEdit::addSegment(
   int  input,
   long long input_start_position,
   long long input_stop_position,
   long long output_insert_position,
   double    factor)
{
   long long delta;
   set<aflibEditClip, less < aflibEditClip > >::iterator it;
   set<aflibEditClip, less < aflibEditClip > >::iterator remove_first = _clip_array.end();
   set<aflibEditClip, less < aflibEditClip > >::iterator remove_last = _clip_array.end();
   set<aflibEditClip, less < aflibEditClip > > insert_array;
   long long insertion_point;

   delta = input_stop_position - input_start_position;

   // Lets create an insertion point. We do this by breaking up the segment into
   // two at the insertion point
   removeSegment(output_insert_position, output_insert_position);

   insertion_point = 0;

   // Go through the list of clips and find the insertion point. All clips after
   // the insertion point are pushed out to make room for this clip
   for (it = _clip_array.begin(); it != _clip_array.end(); it++)
         {
      if (output_insert_position == (*it).getStopSamplesOutput())
            {
         insertion_point = output_insert_position;
   }
      else if ((*it).getStartSamplesOutput() >= output_insert_position)
            {
         aflibEditClip seg((*it).getInput(),
            (long long)((*it).getStartSamplesOutput() + delta * factor),
            (long long)((*it).getStopSamplesOutput() + delta * factor),
            (*it).getStartSamplesInput(), (*it).getStopSamplesInput(),
            (*it).getSampleRateFactor());

         // Change current clip by marking it for removal and adding modified clip to
         // insert array. Then add new segment
         if (remove_first == _clip_array.end())
         {
            remove_first = it;
         }
         remove_last = it;

         insert_array.insert(seg);
            }
        }

   // Create the new clip to be inserted
   aflibEditClip seg1(input,
      insertion_point, (long long)(insertion_point + delta * factor),
      input_start_position, input_stop_position, factor);

   insert_array.insert(seg1);

   // Remove the elements now
   if ((_clip_array.size() != 0) && (remove_first != _clip_array.end()))
            {
      // IF we did not find the last element then use the last element
      if (remove_last == _clip_array.end())
         _clip_array.erase( remove_first, remove_last);
      else
         _clip_array.erase( remove_first, ++remove_last);
    }

   // Now add the modified and new elements
   _clip_array.insert(insert_array.begin(), insert_array.end());

   recomputeConfig();
   printClips();
}


/*! \brief Add an audio clip using seconds.

    This function will add a new segment at the position specified. If will push
    out all data after this insertion point in the existing audio clip array. This
    function allows one to specify the positions in seconds.
*/
void
aflibAudioEdit::addSegment(
   int  input,
   double input_start_seconds,
   double input_stop_seconds,
   double output_insert_seconds,
   double factor)
{
   const aflibConfig& cfg = getInputConfig();
   long long start_samples;
   long long stop_samples;
   long long insert_samples;

   // Convert seconds to samples and call samples function
   start_samples = (long long)(input_start_seconds * cfg.getSamplesPerSecond());
   stop_samples = (long long)(input_stop_seconds * cfg.getSamplesPerSecond());
   insert_samples = (long long)(output_insert_seconds * cfg.getSamplesPerSecond());

   addSegment(input, start_samples, stop_samples, insert_samples, factor);
}

    
/*! \brief Remove an audio clip segment by segment number.

    This function allows one to remove a segment from the audio clip list by its segment
    number. One should verify the segment first with a call to getSegment. Segment numbers
    can change when ever there is a change made. 
*/
void
aflibAudioEdit::removeSegment(int seg_num)
{
   set<aflibEditClip, less < aflibEditClip > >::iterator it;
   int j;
   long long start_position = 0;
   long long stop_position = 0;

   // Find the segment to be removed start and stop positions
   if (seg_num <= (int)_clip_array.size())
   {
      for (it = _clip_array.begin(), j = 1; it != _clip_array.end(); it++, j++)
      {
         if (j == seg_num)
         {
            start_position = (*it).getStartSamplesOutput();
            stop_position = (*it).getStopSamplesOutput();
            break;
         }
      }

      // IF we found the segment then remove
      if ((start_position !=0) || (stop_position != 0))
      {
         removeSegment(start_position, stop_position);
      }
   }
}


/*! \brief Remove an audio clip segment by samples.

    This function allows one to remove an audio segment by specifing a start and stop samples
    position. This segment can span one or more audio segments. These samples are referenced
    to the current output.
*/
void
aflibAudioEdit::removeSegment(
   long long output_start_position,
   long long output_stop_position)
{
   set<aflibEditClip, less < aflibEditClip > >::iterator it;
   set<aflibEditClip, less < aflibEditClip > >::iterator remove_first = _clip_array.end();
   set<aflibEditClip, less < aflibEditClip > >::iterator remove_last = _clip_array.end();
   set<aflibEditClip, less < aflibEditClip > > insert_array;
   long long output_lost = output_stop_position - output_start_position;
   bool remove_segment = FALSE;
   

   for (it = _clip_array.begin(); it != _clip_array.end(); it++)
   {
      aflibEditClip cur_seg = (*it);

      // If we are removing part of this segment
      if ((output_start_position > cur_seg.getStartSamplesOutput()) &&
          (output_start_position < cur_seg.getStopSamplesOutput()))
      {
         // IF completly contained within this segment then break segment in two
         if (output_stop_position < cur_seg.getStopSamplesOutput())
         {
            long long lost_samples = cur_seg.getStopSamplesOutput() - output_start_position;
            cur_seg.setStopSamplesOutput(output_start_position);
            cur_seg.setStopSamplesInput( cur_seg.getStopSamplesInput() - lost_samples );

            long long delta =  (*it).getStopSamplesOutput() - output_stop_position;
            aflibEditClip seg(cur_seg.getInput(),
               output_start_position,
               (*it).getStopSamplesOutput() - (output_stop_position - output_start_position),
               (*it).getStopSamplesInput() - delta, (*it).getStopSamplesInput(),
               (*it).getSampleRateFactor());

            // Change current clip by marking it for removal and adding modified clip to
            // insert array. Then add new segment
            if (remove_first == _clip_array.end())
            {
               remove_first = it;
            }
            remove_last = it;
            remove_segment = TRUE;
            insert_array.insert(cur_seg);
            insert_array.insert(seg);
         }
         // ELSE IF segment just ends at this segment then just readjust end point
         else if (output_stop_position == cur_seg.getStopSamplesOutput())
         {
            long long lost_samples = output_stop_position - output_start_position;
            cur_seg.setStopSamplesOutput( cur_seg.getStopSamplesOutput() - lost_samples );
            cur_seg.setStopSamplesInput( cur_seg.getStopSamplesInput() - lost_samples );

            // Change current clip by marking it for removal and adding modified clip to
            // insert array.
            if (remove_first == _clip_array.end())
            {
               remove_first = it;
            }
            remove_last = it;
            remove_segment = TRUE;

            // If segment to add has 0 length then entire segment was deleted so don't add
            if (cur_seg.getStopSamplesOutput() - cur_seg.getStartSamplesOutput() != 0)
            {
               insert_array.insert(cur_seg);
            }
         }
         // ELSE this segment spans more than this segment
         else
         {
            long long lost_samples = cur_seg.getStopSamplesOutput() - output_start_position;
            cur_seg.setStopSamplesOutput( output_start_position );
            cur_seg.setStopSamplesInput( cur_seg.getStopSamplesInput() - lost_samples );

            // Change current clip by marking it for removal and adding modified clip to
            // insert array. 
            if (remove_first == _clip_array.end())
            {
               remove_first = it;
            }
            remove_last = it;
            remove_segment = TRUE;
            insert_array.insert(cur_seg);
         }
      }
      // ELSE IF segment we are removing starts with this segment or before
      else if ((output_start_position <= cur_seg.getStartSamplesOutput()) &&
               (output_stop_position > cur_seg.getStartSamplesOutput()))
      {
         // IF cut ends before this segment
         if (output_stop_position < cur_seg.getStopSamplesOutput())
         {
            long long lost_samples = output_stop_position - cur_seg.getStartSamplesOutput();
            long long lost_before_clip = cur_seg.getStartSamplesOutput() - output_start_position;
            cur_seg.setStartSamplesOutput( (*it).getStartSamplesOutput() - lost_before_clip);
            cur_seg.setStopSamplesOutput( (*it).getStopSamplesOutput() - lost_before_clip - lost_samples);
            cur_seg.setStartSamplesInput( (*it).getStartSamplesInput()  + lost_samples);
            cur_seg.setStopSamplesInput( (*it).getStopSamplesInput() );

            // Change current clip by marking it for removal and adding modified clip to
            // insert array.
            if (remove_first == _clip_array.end())
            {
               remove_first = it;
            }
            remove_last = it;
            remove_segment = TRUE;
            insert_array.insert(cur_seg);
         }
         // ELSE IF entire segment
         else if (output_stop_position == cur_seg.getStopSamplesOutput())
         {
            //  erase current clip by marking it for removal 
            if (remove_first == _clip_array.end())
            {
               remove_first = it;
            }
            remove_last = it;
            remove_segment = TRUE;
         }
         // ELSE cut ends after this segment then remove clip and continue
         else
         {
            //  erase current clip by marking it for removal 
            if (remove_first == _clip_array.end())
            {
               remove_first = it;
            }
            remove_last = it;
            remove_segment = TRUE;
         }
      }
      // ELSE IF all segments after the cut need to have output readjusted
      else if (output_stop_position <= cur_seg.getStartSamplesOutput())
      {
         cur_seg.setStartSamplesOutput((*it).getStartSamplesOutput() - output_lost);
         cur_seg.setStopSamplesOutput((*it).getStopSamplesOutput() - output_lost);

         // Change current clip by marking it for removal and adding modified clip to
         // insert array. Then add new segment
         if (remove_first == _clip_array.end())
         {
            remove_first = it;
         }
         remove_last = it;
         remove_segment = TRUE;
         insert_array.insert(cur_seg);
      }
   }

   // Remove the elements now
   // There must be data in clip array to erase _clip_array
   if ((_clip_array.size() != 0) && (remove_segment == TRUE))
   {
      _clip_array.erase( remove_first, ++remove_last);
   }

   // Now add the modified and new elements
   if (insert_array.size() != 0)
      _clip_array.insert(insert_array.begin(), insert_array.end());

   recomputeConfig();
   printClips();
}

/*! \brief Remove an audio clip segment by seconds.

    This function allows one to remove an audio segment by specifing a start and stop seconds
    position. This segment can span one or more audio segments. These seconds are referenced
    to the current output.
*/
void
aflibAudioEdit::removeSegment(
   double output_start_seconds,
   double output_stop_seconds)
{
   const aflibConfig& cfg = getInputConfig();
   long long start_samples;
   long long stop_samples;
 
   // Convert seconds to samples and call samples remove function
   start_samples = (long long)(output_start_seconds * cfg.getSamplesPerSecond());
   stop_samples = (long long)(output_stop_seconds * cfg.getSamplesPerSecond());
 
   removeSegment(start_samples, stop_samples);
}
 
 
/*! \brief Remove all audio segments for a specific input
 
    This function allows one to remove all audio segments for a specific input.
*/ 
void
aflibAudioEdit::removeSegmentsFromInput(int input)
{
   // This function will remove all audio clips from a specific input.
 
   int num_segs;
   int inp, i;
   long long start_i, stop_i, start_o, stop_o;
   double  factor;
 
 
   num_segs = getNumberOfSegments();
 
   // Count backwards so that num_segs changing does not affect us
   // Remove all clips from this input
   for (i = num_segs; i != 0; i--)
   {
      getSegment(i, inp, start_i, stop_i, start_o, stop_o, factor);
      if (inp == input)
      {
         // Remove this specifiec segment number
         removeSegment(i);
   }
   }
}


/*! \brief Gets the current number of segments that are in the audio clip list.
*/
int
aflibAudioEdit::getNumberOfSegments()
{
   return (_clip_array.size());
}


/*! \brief Retrieves information for an audio clip segment by samples.

    This function will retrieve the information for a particular audio clip segment. This will
    allow the user to determine the input and output start and stop positions in samples. The
    segment numbers start with 1.
*/
void
aflibAudioEdit::getSegment(
   int segment_number,
   int& input,
   long long& input_start_position,
   long long& input_stop_position,
   long long& output_start_position,
   long long& output_stop_position,
   double&    factor)
{
   set<aflibEditClip, less < aflibEditClip > >::iterator it;
   int j;

   input_start_position = 0;
   input_stop_position = 0;
   output_start_position = 0;
   output_stop_position = 0;
   input = 0;

   if (segment_number <= (int)_clip_array.size())
   {
      for (it = _clip_array.begin(), j = 1; it != _clip_array.end(); it++, j++)
      {
         if (j == segment_number)
         {
            input_start_position = (*it).getStartSamplesInput();
            input_stop_position = (*it).getStopSamplesInput();
            output_start_position = (*it).getStartSamplesOutput();
            output_stop_position = (*it).getStopSamplesOutput();
            input = (*it).getInput();
            factor = (*it).getSampleRateFactor();
            break;
         }
   }
   }
}


/*! \brief Retrieves information for an audio clip segment by seconds.

    This function will retrieve the information for a particular audio clip segment. This will
    allow the user to determine the input and output start and stop positions in seconds. The
    segment numbers start with 1.
*/
void
aflibAudioEdit::getSegment(
   int segment_number,
   int& input,
   double& input_start_seconds,
   double& input_stop_seconds,
   double& output_start_seconds,
   double& output_stop_seconds,
   double& factor)
{
   const aflibConfig& cfg = getInputConfig();

   long long start_samples_output;
   long long stop_samples_output;
   long long start_samples_input;
   long long stop_samples_input;

   getSegment(segment_number, input,
      start_samples_input, stop_samples_input, start_samples_output, stop_samples_output,
      factor);

   // Convert seconds to samples and call samples remove function
   input_start_seconds = (double)start_samples_input / cfg.getSamplesPerSecond();
   input_stop_seconds = (double)stop_samples_input / cfg.getSamplesPerSecond();
   output_start_seconds = (double)start_samples_output / cfg.getSamplesPerSecond();
   output_stop_seconds = (double)stop_samples_output / cfg.getSamplesPerSecond();
}


/*! \brief Sets the input and output audio data configuration of this object.
 
   This function overrides the aflibAudio base class function. It will change
   the total samples in the output audio configuration. It will also select
   the best output based on the inputs. Any conversion that needs to be done
   will be done. This allows mixing of inputs with different sample rates,
   endian layouts, channels, and data sizes.
*/
void
aflibAudioEdit::setInputConfig(const aflibConfig& cfg)
{
   aflibConfig config = cfg;
   set<aflibEditClip, less < aflibEditClip > >::iterator it_clip;
   map<int, aflibAudio *, less<int> >  parent_list = getParents();
   map<int, aflibAudio *, less<int> >::iterator it;
   int  sample_rate = 0;
   aflib_data_endian  endian = AFLIB_ENDIAN_LITTLE;
   aflib_data_size    size = AFLIB_DATA_8U;
   aflibConfig out_cfg(cfg);
   int   chan_num = 0;


   // Look at every parents data configuration
   for (it = parent_list.begin(); it != parent_list.end(); it++)
   {
      const aflibConfig& new_cfg = ((*it).second)->getOutputConfig();

      // Pick the biggest sample rate
      if (new_cfg.getSamplesPerSecond() > sample_rate)
      {
         sample_rate = new_cfg.getSamplesPerSecond();
      }

      if (new_cfg.getChannels() > chan_num)
      {
         chan_num = new_cfg.getChannels();
      }

      // Pick last endian config. It does not really matter
      endian = new_cfg.getDataEndian();

      // Pick 16S, 16U, 8S, or 8U in that order
      if (size != AFLIB_DATA_16S)
      {
         if (new_cfg.getSampleSize() == AFLIB_DATA_16S)
         {
            size = AFLIB_DATA_16S;
         }
         else if (new_cfg.getSampleSize() == AFLIB_DATA_16U)
         {
            size = AFLIB_DATA_16U;
         }
         else if (size != AFLIB_DATA_16U)
         {
            if (new_cfg.getSampleSize() == AFLIB_DATA_8S)
            {
                    size = AFLIB_DATA_8S;
            }
            else if (size != AFLIB_DATA_8S)
            {
               size = AFLIB_DATA_8U;
            }
         }
      }
   }

   // Set and Store the output configuration
   out_cfg.setSamplesPerSecond(sample_rate);
   out_cfg.setSampleSize(size);
   out_cfg.setDataEndian(endian);
   out_cfg.setChannels(chan_num);

   // IF no more clips
   if (_clip_array.size() == 0)
   {
      out_cfg.setTotalSamples(0);
   }
   // ELSE get stop segment from last clip and stop as total samples
   else
   {
      it_clip = _clip_array.end();
      it_clip--;
      out_cfg.setTotalSamples((*it_clip).getStopSamplesOutput());
   }
 
   setOutputConfig(out_cfg);

   // Set the input config to be the same as the output. This is what we need inputted
   // into each input. It will force the base classes to make the conversion.
   aflibAudio::setInputConfig(cfg);
   aflibAudio::setOutputConfig(out_cfg);
}

void
aflibAudioEdit::recomputeConfig()
{
   // Invalidate chain so that new config gets passed up chain
   setNodeProcessed(FALSE);
}


void
aflibAudioEdit::printClips()
{
   // This function is for debugging purposes. It allows one to print all the audio clip data


   if (getenv("AFLIB_DEBUG"))
   {
      set<aflibEditClip, less < aflibEditClip > >::iterator it;
      int clip_num;

      cout << endl << "---------------------------------------------------------" << endl;
      for (it = _clip_array.begin(), clip_num = 1; it != _clip_array.end(); it++, clip_num++)
      {
         cout << "Clip Number " << clip_num << endl;
         cout << "Clip Input " << (*it).getInput() << endl;
         cout << "Start Samples Input " << (*it).getStartSamplesInput() << endl;
         cout << "Stop Samples Input " << (*it).getStopSamplesInput() << endl;
         cout << "Start Samples Output " << (*it).getStartSamplesOutput() << endl;
         cout << "Stop Samples Output " << (*it).getStopSamplesOutput() << endl;
         cout << "Factor " << (*it).getSampleRateFactor() << endl;
      }
      cout << "---------------------------------------------------------" << endl;
   }
}


/*! \brief Not Yet Implemented.
*/
aflibUndoRedo
aflibAudioEdit::getUndoRedoStatus() const
{
   // Until we implement undo redo we will return none.
   return(AFLIB_NONE_MODE);
}


/*! \brief Not Yet Implemented.
*/
void
aflibAudioEdit::performUndoRedo()
{
   // Need to undo the last action if in undo mode. If in redo mode then reapply
   // the last action.
}


/*! \brief Main process function.

    Since we have to deal with multiple inputs we override the base classes process function
    with our own. This will retrieve the audio data from the proper input based on position.
    It is responsible for mapping the output sample position to the correct input and
    its sample position. See the base class aflibAudio::process for what the process
    function is suppose to do.
*/
aflibData *
aflibAudioEdit::process(
   aflibStatus& ret_status,
   long long position,
   int& num_samples,
   bool free_memory) 
{
   int  list_size = 0;
   aflibData * data = NULL;
   long long start_input_position = 0;
   ret_status = AFLIB_SUCCESS;
   int use_input = -1;
   set<aflibEditClip, less < aflibEditClip > >::iterator it;
   long  length;
   list<aflibData *> d_list;

   incrementLevel();

   // Check to see if chain has been preprocessed if at start of chain
   examineChain();

   // TBD in the future we may need to make two process calls for a data item if it
   // spans both. For now if a data item spans two clips we will just read from the
   // first one.

   map<int, aflibAudio *, less<int> > audio_list = this->getParents();
   list_size = audio_list.size();

   // Go thru all clips and find which clip to use
   for (it = _clip_array.begin(); it != _clip_array.end(); it++)
   {
      if ((position >= (*it).getStartSamplesOutput()) &&
          (position < (*it).getStopSamplesOutput()))
      {
         start_input_position = 
            position - (*it).getStartSamplesOutput() + 
            (*it).getStartSamplesInput();
         use_input = (*it).getInput();
         break;
      }
   }

   // IF no clip was found then we have reached the end of the file
   if (use_input == -1)
   {
      ret_status = AFLIB_END_OF_FILE;
   }
   else
   {
      // IF no more parents then process since we are at the end of the chain
      if (list_size == 0)
   {
         // IF node is not enabled then skip processing
         if (getEnable() == TRUE)
                {
            if (num_samples == 0)
               data = new aflibData(4096);
            else
               data = new aflibData(num_samples);

            d_list.push_back(data);
            ret_status = compute_segment(d_list, position);
   }
            }
      // ELSE call parent and let it process it first then process result
      else
      {
         data = ((aflibAudio *)audio_list[use_input])->process(
            ret_status, start_input_position, num_samples, FALSE);
         // ptr can be NULL if parent was not enabled
         if (data == NULL)
         {
            if (num_samples == 0)
               data = new aflibData(4096);
            else
               data = new aflibData(num_samples);
         }

         if (getEnable() == TRUE)
         {
            d_list.push_back(data);
            ret_status = compute_segment(d_list, position);
         }
      }
   }

   // Set num_samples with correct value
   if (data)
   {
      data->getLength(length);
      num_samples = (int)length;
        }

   // IF caller does not want memory returned then free
   if (free_memory == TRUE)
   {
      delete data;
      data = NULL;
    }

   decrementLevel();

   return (data);
}


/*! \brief Main work function.

   We don't do any real processing of the data. We actually only route the data.
*/
aflibStatus
aflibAudioEdit::compute_segment(
   list<aflibData *>& data,
   long long position) 
{
   return (AFLIB_SUCCESS);
}

void
aflibAudioEdit::parentWasDestroyed(int parent_id)
{
   // We need to rebuild everything if a parent was destroyed. This
   // is a callback from aflibChain letting us know that an input was
   // destroyed for some reason.

   removeInput(parent_id);
}

void
aflibAudioEdit::parentWasAdded(int parent_id)
{
   // If user has added an input to this class then we need to setup a new input

   addInput(parent_id);
}


bool
aflibAudioEdit::isDataSizeSupported(aflib_data_size size)
{
   // This overrides the virtual function in the base class.
 
   bool state = FALSE;
 
   if (size == getInputConfig().getSampleSize())
      state = TRUE;
 
   return (state);
}
 
bool
aflibAudioEdit::isEndianSupported(aflib_data_endian end)
{
   // This overrides the virtual function in the base class.
 
   bool state = FALSE;
 
   if (end == getInputConfig().getDataEndian())
      state = TRUE;
 
   return (state);
}

bool
aflibAudioEdit::isSampleRateSupported(int& rate)
{
   // This overrides the virtual function in the base class. See if the rate requested
   // is the rate that we have computed that we will be outputting.

   int value;
   bool ret_value = FALSE;

   // Get the rate of the data
   value = getOutputConfig().getSamplesPerSecond();
 
   // IF same rate then TRUE else return desired rate
   if (rate == value)
      ret_value = TRUE;
   else
      rate = value;
 
   return (ret_value);
}

bool
aflibAudioEdit::isChannelsSupported(int& channels)
{
   // This overrides the virtual function in the base class. See if the channels requested
   // is the channels that we have computed that we will be outputting.

   int value;
   bool ret_value = FALSE;
 
   // Get the number of channels of the data
   value = getOutputConfig().getChannels();
 
   // IF same number of channels then TRUE else return desired number of channels
   if (channels == value)
      ret_value = TRUE;
   else
      channels = value;
 
   return (ret_value);
}


