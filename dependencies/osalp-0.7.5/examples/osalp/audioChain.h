#ifndef _audioChain_H_
#define _audioChain_H_


class aflibAudio;
class aflibConfig;

#include <string>

using namespace std;
class audioChain
{
   public:

      audioChain(){_verbose = true;};

      int exec(int argc, char* argv[]);

      void convert(int argc, char* argv[]);

      void mix(int argc, char* argv[]);

      void pitch(int argc, char* argv[]);

      void id(int argc, char* argv[]);

      void formats(int argc, char* argv[]);

      void testall_formats(int argc, char* argv[]);

      void help(int argc, char* argv[]);

      void date(int argc, char* argv[]);

      int process(aflibAudio*);

   private:

      void parseConfigArgs(
            int argc, 
            char **argv, 
            aflibConfig& config,
            string& format
                          ); 

      bool _verbose;

};

#endif
