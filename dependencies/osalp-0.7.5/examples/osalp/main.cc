// This file contains no license whatsoever. It is provided in the public domain as an example
// of how to use the audio library
//
// Bruce Forsberg
// forsberg@tns.net
// 
//


#include "audioChain.h"


int
main(
   int argc,
   char * argv[])
{
	audioChain* chain = new audioChain();

	chain->exec(argc,argv);

   return (0);
}


