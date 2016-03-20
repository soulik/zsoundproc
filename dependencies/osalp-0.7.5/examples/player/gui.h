#ifndef _GUI_H_
#define _GUI_H_

#include <string>

extern void
gui_build(int argc, char * argv[]);

extern void
gui_loop();

extern void
gui_spectrum(int array, double *db_array);

extern void
gui_set_slider_position(
   long long cur_pos,
   long long total_pos,
   long long sam_per_sec);

extern void
gui_set_record_mode(int record);

extern void
gui_set_channels(int chan);

extern void
gui_set_bits(int bits);

extern void
gui_set_file(string& file);

extern void
gui_set_format(const string& format);

#endif
