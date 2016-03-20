// This file contains no license whatsoever. It is provided in the public domain as an example
// of how to use the audio library
//
// Bruce Forsberg
// forsberg@tns.net
// 
//

#include <iostream.h>
#include <stdio.h>
#include "plot_data.h"


plot_data::plot_data(
   aflibAudio * audio,
   int          id)
{
   _audio = audio;
   _id    = id;
}

plot_data::~plot_data()
{
}

aflibAudio *
plot_data::getAfLibAudio()
{
   return (_audio);
}

int
plot_data::getID()
{
   return (_id);
}


