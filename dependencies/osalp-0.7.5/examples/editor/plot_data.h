#ifndef _PLOT_DATA_H
#define _PLOT_DATA_H


class aflibAudio;

class plot_data {

public:

   plot_data(
      aflibAudio *audio,
      int         id);

   ~plot_data();

   aflibAudio *
   getAfLibAudio();

   int
   getID();


protected:

private:

aflibAudio * _audio;
int          _id;

};

#endif
