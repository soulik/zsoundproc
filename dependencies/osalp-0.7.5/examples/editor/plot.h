#ifndef _PLOT_H_
#define _PLOT_H_

extern "C"
{
#include <X11/X.h>
#include <Xm/Xm.h>
}

extern void
plot_build(
   Widget  parent);

extern void
plot_input(
   char * text_file);

extern void
plot_enlarge();

extern void
plot_reduce();

extern void
plot_shift_left(int value);

extern void
plot_shift_right(int value);

extern void
plot_play( char * text_file);

extern void
plot_pause();

extern void
plot_stop();

extern void
plot_cut();

extern void
plot_erase();

extern void
plot_reset();

extern void
plot_insert();

extern void
plot_set_mode(bool set_mode);

extern void
plot_select_input(int num);


#endif
