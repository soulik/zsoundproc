// This file contains no license whatsoever. It is provided in the public domain as an example
// of how to use the audio library
//
// Bruce Forsberg
// forsberg@tns.net
// 
//


#include <stdio.h>
#include <stdlib.h>

#include <X11/X.h>
 
extern "C"
{
#include <Xm/Xm.h>
}

#include "gui.h"
#include "audio.h"

int
main(int argc, char * argv[])
{
   gui_build(argc, argv);

   // IF file specified then play it
   if (argc == 2)
      audio_play(argv[1]);

   gui_loop();

   return (0);
}


