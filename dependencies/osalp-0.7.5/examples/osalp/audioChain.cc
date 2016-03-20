// This file contains no license whatsoever. It is provided in the public 
// domain as an example of how to use the audio library
//
// Bruce Forsberg
// forsberg@tns.net
// 
//

/* For pitch function */
#include "aflib/aflibAudioPitch.h"

/* For convert, pitch, mix and id functions */
#include "aflib/aflibAudioFile.h"

/* To print current time */
#include "aflib/aflibDateTime.h"

/* For mix function */
#include "aflib/aflibAudioMixer.h"

/* For formats function */
#include "aflib/aflibFileItem.h"
#include "aflib/aflibFile.h"

/* For printing info, warnings and errors */
#include "aflib/aflibDebug.h"

/* For getopts */
#include <unistd.h>
#include "getopt.h"

/* For getchar() */
#include <stdio.h>

/* For errno */
#include <errno.h>
/* For strerror() */
#include <string.h>

#include "audioChain.h"

int
audioChain::exec(int argc, char* argv[])
{

	opterr = 0;
	int optchar;

   optchar = getopt(argc, argv, "+pimaht");
	
	switch(optchar)
	{
		case 'p':
			optind--;
			pitch(argc, argv);
			break;
		case 'i':
			id(argc,argv);
			break;
		case 'm':
			mix(argc,argv);
			break;
		case 't':
			testall_formats(argc, argv);
			break;
		case 'a':
			formats(argc,argv);
			break;
		case 'h':
			help(argc, argv);
			date(argc, argv);
			break;
		case '?':
			optind--;
			convert(argc,argv);
			break;
		case EOF:
		default:
			convert(argc,argv);
			break;
	}
	
	return 0;
}

/* 
 * This function will open a file for reading and open a file for writing.
 * 
 * This is an example of the basics of what the osalp library can do.
 * 
 * First a file is opened for reading.  This can be a file of any of 
 * osalp supported file type including a soundcard device.
 * 
 * Second a file is opened for writing and at the same time the input
 * file is added as a parent.
 * 
 * Once these two objects are created the process function is called which 
 * reads the input and writes it to the output.  
 *
 * The osalp library by default will perform any conversions needed to 
 * write one file to the other.  This includes data size, endianess, sample
 * rate and number of channels.
 * 
 * i.e. if a user wants to convert a 16bit signed, 2 channel wave file with 
 * a rate of 44100 to an 8bit unsigned, 1 channel au file with a rate of 
 * 8000 they would use the following command line and this function would 
 * do that for them.
 *
 * osalp filename1.wav -B -c1 -r8000 -f AU filename.au 
 * 
 * */

void
audioChain::convert(int argc, char **argv) 
{
	aflibConfig in_config, out_config;

	aflibStatus status = AFLIB_SUCCESS;

	string in_file, out_file;
	string in_format, out_format;

	aflibAudioFile* input;
	aflibAudioFile* output;

	/* by default try to auto detect the input file */
	in_format = "AUTO";

	/* let the user override default values if they would like */
	parseConfigArgs(argc,argv,in_config,in_format); 
	
	/* get the output file name */
	if(optind >= argc)
	{
		/* aflib_fatal calls exit(1) */
		aflib_fatal("No input file!");
	}
	
	in_file = argv[optind];
	optind++;
	
	/* get input aflibAudioFile object 
	 * this call will fill in_config with the appropriate values if
	 * the filetype supports that
	 */
	input = new aflibAudioFile(in_format,in_file,&in_config,&status);
	
	if(status != AFLIB_SUCCESS)
	{
		aflib_fatal("Error opening input file %s", in_file.c_str());
	}

	/* copy the input config to the output config */
	out_config = in_config;
	out_file = "/dev/audio";
	out_format = "DEVICE";
	
	/* let the user override these values if they would like */
	parseConfigArgs(argc,argv,out_config,out_format); 
	
	/* assume the last arg is a user specified output file */
	if(optind < argc)
	{
		out_file = argv[optind];
	}

	/* get output aflibAudioFile object.
	 * The constructor will try to set the byte, endianess and rate from the 
	 * config passed to this constructor.
	 * However this is only a suggestion and if the format doesn't support
	 * the config then it will use default values.
	 */
  	output = 
		new aflibAudioFile(*input,out_format, out_file, &out_config, &status);
	
	if(status != AFLIB_SUCCESS)
	{
		aflib_fatal("Error opening output file %s", out_file.c_str());
	}

	/* process it */
	process(output);

	delete output;
	delete input;

}

/* This is an example of how to use the aflibAudioPitch class.
 */
void
audioChain::pitch(int argc, char **argv) 
{
	aflibConfig in_config, out_config;

	aflibStatus status = AFLIB_SUCCESS;

	string in_file, out_file;
	string in_format, out_format;

	aflibAudioFile* input;
	aflibAudioPitch* pitch;
	aflibAudioFile* output;

	double pitch_factor = 1.0;
	int linear_interpolation = 0;
	int high_quality = 0;
	int filter_interpolation = 0;

	int optchar;

	static struct option long_options[] =
	{
		{"linear",0,&linear_interpolation,1},
		{"high",0,&high_quality,1},
		{"filter",0,&filter_interpolation,1}
	};
	
	do
	{
		optchar = getopt_long(argc, argv, "+p:",long_options,NULL);
		
		switch(optchar)
		{
			case 'p':
				if(optind > argc){
					aflib_fatal("Must specify a pitch value",argv[0]);
				} else { 
					pitch_factor = atof(optarg);
				}
				break;
			case '?':
				optind--;
				break;
			case EOF:
			default:
				break;
		}
	} while(optchar != '?' && optchar != EOF);
	
	/* by default try to auto detect the input file */
	in_format = "AUTO";

	/* let the user override default values if they would like */
	parseConfigArgs(argc,argv,in_config,in_format); 
	
	/* get the output file name */
	if(optind >= argc)
	{
		/* aflib_fatal calls exit(1) */
		aflib_fatal("No input file!");
	}
	
	in_file = argv[optind];
	optind++;
	
	/* get input aflibAudioFile object 
	 * this call will fill in_config with the appropriate values if
	 * the filetype supports that
	 */
	input = new aflibAudioFile(in_format,in_file,&in_config,&status);
	
	if(status != AFLIB_SUCCESS)
	{
		aflib_fatal("Error opening input file %s", in_file.c_str());
	}

	/* copy the input config to the output config */
	out_config = in_config;
	out_file = "/dev/audio";
	out_format = "DEVICE";
	
	/* let the user override these values if they would like */
	parseConfigArgs(argc,argv,out_config,out_format); 
	
	/* assume the last arg is a user specified output file */
	if(optind < argc)
	{
		out_file = argv[optind];
	}

	/* Create the pitch object. */
	pitch = new aflibAudioPitch(*input,pitch_factor,linear_interpolation,high_quality,filter_interpolation);

	/* get output aflibAudioFile object */
  	output = 
		new aflibAudioFile(*pitch,out_format, out_file, &out_config, &status);
	
	if(status != AFLIB_SUCCESS)
	{
		aflib_fatal("Error opening output file %s", out_file.c_str());
	}

	/* process it */
	process(output);

	delete output;
	delete input;

}

void
audioChain::mix(
		int argc,
		char* argv[]
  ) 
{
	aflibConfig in_config1, in_config2, out_config;

	aflibStatus status = AFLIB_SUCCESS;

	string in_file1, in_file2, out_file;
	string in_format1, in_format2, out_format;

	aflibAudioFile* input1;
	aflibAudioFile* input2;
	aflibAudioMixer* mix;
	aflibAudioFile* output;

/* Get first files format and name*/
	
	/* by default try to auto detect the input file */
	in_format1 = "AUTO";

	/* let the user override default values if they would like */
	parseConfigArgs(argc,argv,in_config1,in_format1); 
	
	/* get the output file name */
	if(optind >= argc)
	{
		/* aflib_fatal calls exit(1) */
		aflib_fatal("Missing first input file!");
	}
	
	in_file1 = argv[optind];
	optind++;
	
/* Get second files format and name*/
	/* by default try to auto detect the input file */
	in_format2 = "AUTO";

	/* let the user override default values if they would like */
	parseConfigArgs(argc,argv,in_config2,in_format2); 
	
	/* get the output file name */
	if(optind >= argc)
	{
		/* aflib_fatal calls exit(1) */
		aflib_fatal("Missing second input file!");
	}
	
	in_file2 = argv[optind];
	optind++;
	
	/* Create input aflibAudioFile objects 
	 * this call will fill in_config with the appropriate values if
	 * the filetype supports that
	 */
	input1 = new aflibAudioFile(in_format1,in_file1,&in_config1,&status);
	
	if(status != AFLIB_SUCCESS)
	{
		aflib_fatal("Error opening input file %s", in_file1.c_str());
	} 
	else if(_verbose) 
	{
	   aflib_info("%s type: %s", in_file1.c_str(), input1->getFormat().c_str());
   	aflib_info("rate: %d, channels: %d, samples: %d", 
			in_config1.getSamplesPerSecond(), 
			in_config1.getChannels(), 
			in_config1.getTotalSamples() 
			);
	}

	input2 = new aflibAudioFile(in_format2,in_file2,&in_config2,&status);
	
	if(status != AFLIB_SUCCESS)
	{
		aflib_fatal("Error opening input file %s", in_file2.c_str());
	}
	else if(_verbose) 
	{
	   aflib_info("%s type: %s", in_file2.c_str(), input2->getFormat().c_str());
   	aflib_info("rate: %d, channels: %d, samples: %d", 
			in_config2.getSamplesPerSecond(), 
			in_config2.getChannels(), 
			in_config2.getTotalSamples() 
			);
	}
	
	
	/* copy the input config to the output config */
	out_config = in_config1;
	out_file = "/dev/audio";
	out_format = "DEVICE";
	
	/* let the user override these values if they would like */
	parseConfigArgs(argc,argv,out_config,out_format); 
	
	/* assume the last arg is a user specified output file */
	if(optind < argc)
	{
		out_file = argv[optind];
	}

	int output_channels = out_config.getChannels();
	
	if(output_channels > 2)
	{
		aflib_fatal("mix function currently only supports one or two output channels");
	}
	
	mix = new aflibAudioMixer();
	// First add input1 to the mix
	int id = mix->addParent(*input1);

	int input_channels = input1->getInputConfig().getChannels();
	
	if(input_channels == 2)
	{
		if(output_channels == 2)
		{
			/* left to left - right to right */
			mix->addMix(id,0,0, 100);
			mix->addMix(id,1,1, 100);
			aflib_info("Mixing Stereo file %s to Stereo",in_file1.c_str()); 
		}
		else
		{
			/* left to mono - right to mono */
			mix->addMix(id,1,0, 100);
			mix->addMix(id,0,0, 100);
			aflib_info("Mixing Stereo file %s to Mono",in_file1.c_str()); 
		}
	}	
	else if(input_channels == 1)
	{
		if(output_channels == 2)
		{
			/* mono to left - mono to right */
			mix->addMix(id,0,0, 100);
			mix->addMix(id,0,1, 100);
			aflib_info("Mixing Mono file %s to Stereo",in_file1.c_str()); 
		}
		else
		{
			/* mono to mono */
			mix->addMix(id,0,0, 100);
			aflib_info("Mixing Mono file %s to Mono",in_file1.c_str()); 
		}
	} 
	else 
	{
		aflib_fatal("Mix function only currently supports one or two input channels: input file %s", in_file1.c_str());
	}

	input_channels = input2->getInputConfig().getChannels();
	
	id = mix->addParent(*input2);
	
	if(input_channels == 2)
	{
		if(output_channels == 2)
		{
			/* left to left - right to right */
			mix->addMix(id,0,0, 100);
			mix->addMix(id,1,1, 100);
			aflib_info("Mixing Stereo file %s to Stereo",in_file2.c_str()); 
		}
		else
		{
			/* left to mono - right to mono */
			mix->addMix(id,1,0, 100);
			mix->addMix(id,0,0, 100);
			aflib_info("Mixing Mono file %s to Mono",in_file2.c_str()); 
		}
	}
	else if(input_channels == 1)
	{
		if(output_channels == 2)
		{
			/* mono to left - mono to right */
			mix->addMix(id,0,0, 100);
			mix->addMix(id,0,1, 100);
			aflib_info("Mixing Mono file %s to Stereo",in_file2.c_str()); 
		}
		else
		{
			/* mono to mono */
			mix->addMix(id,0,0, 100);
			aflib_info("Mixing Mono file %s to Mono",in_file2.c_str()); 
		}
	} 
	else 
	{
		aflib_fatal("Mix function only currently supports one or two input channels: input file %s", in_file2.c_str());
	}


	/* get output aflibAudioFile object */
  	output = 
		new aflibAudioFile(*mix,out_format, out_file, &out_config, &status);
	
	if(status != AFLIB_SUCCESS)
	{
		aflib_fatal("Error opening output file %s", out_file.c_str());
	}
	else if(_verbose) 
	{
	   aflib_info("%s type: %s", out_file.c_str(), output->getFormat().c_str());
   	aflib_info("rate: %d, channels: %d, samples: %d", 
			out_config.getSamplesPerSecond(), 
			out_config.getChannels(), 
			out_config.getTotalSamples() 
			);
	}

	/* process it */
	process(output);

	delete output;
	delete input1;
	delete input2;
	delete mix;

}

void
audioChain::testall_formats(
		int argc,
		char* argv[]
  ) 
{

   list<aflibFileItem*> modules_list;
   list<aflibFileItem*>::iterator  it;

   modules_list = aflibFile::returnSupportedFormats();

	string cmdname = argv[0];
	cmdname += " ";

	string in_args;
	for(int i = 1; i < argc ; i++){
		if(string(argv[i]) != "-t"){
			in_args += argv[i];
			in_args += " ";
		}
	}

   for ( it = modules_list.begin(); it != modules_list.end(); it++)
   {
		if((*it)->getFormat() == "nul") continue;
		if((*it)->getFormat() == "ossdsp") continue;
		if((*it)->getFormat() == "DEVICE") continue;
		if((*it)->getFormat() == "auto") continue;
		if((*it)->getFormat() == "MPEG") continue;
		if((*it)->getFormat() == "raw") continue;
		if((*it)->getFormat() == "hcom") continue;
		string out_args, cmdline;
		char tempfile[] = "osalp-testfile.XXXXXX";
		if(!mktemp(tempfile)) aflib_fatal("%s", strerror(errno));

		out_args += "-f ";
		out_args += (*it)->getFormat();
		out_args += " ";
		out_args += tempfile;
		cmdline = cmdname;
		cmdline += in_args;
		cmdline += out_args;
		aflib_info("Testing %s",(*it)->getFormat().c_str() );
//		aflib_info("Converting to %s",(*it)->getFormat().c_str() );
		if(system(cmdline.c_str())==0){
			cmdline = cmdname;
			cmdline += out_args;
			cmdline += " -f ossdsp";
			system(cmdline.c_str());
		}
		else 
		{
			aflib_info("Error converting to %s",(*it)->getFormat().c_str() );
		}
		unlink(tempfile);
		
   }
}

void
audioChain::id(
		int argc,
		char* argv[]
  ) 
{

static char* data_size_strs[] =   
{
   "UNDEFINED",
   "8 bit signed",
   "8 bit unsigned",
   "16 bit signed",
   "16 bit unsigned",
   "32 bit signed",
   "32 bit unsigned",
	0
};

static char* data_endian_strs[] =
{
   "UNDEFINED",
   "little endian",
   "big endian",
	0
};


	aflibStatus status = AFLIB_SUCCESS;
	aflibConfig config;
	
	string format, file;

	aflibAudioFile* input;

	format = "AUTO";

	parseConfigArgs(argc,argv,config,format); 

	if(optind >= argc)
	{
		aflib_fatal("No input file!");
	}
	
	file = argv[optind];
	
	/* Get input aflibAudioFile object
	 * which will give us the configuration info of the file
	 */
	input = new aflibAudioFile(format,file,&config,&status);
	
	if(status != AFLIB_SUCCESS)
	{
		aflib_fatal("Error opening input file %s", file.c_str());
	}
	
   aflib_info("%s type: %s", file.c_str(), input->getFormat().c_str());
   aflib_info("rate: %d, channels: %d, samples: %d", 
			config.getSamplesPerSecond(), 
			config.getChannels(), 
			config.getTotalSamples() 
			);
   aflib_info("data: %s %s", 
			data_size_strs[config.getSampleSize()], 
			data_endian_strs[config.getDataEndian()]
			);
	
	delete input;
}

void
audioChain::formats(
		int argc,
		char* argv[]
  ) 
{
   list<aflibFileItem*> modules_list;
   list<aflibFileItem*>::iterator  it;

   modules_list = aflibFile::returnSupportedFormats();

	aflib_info("Supported file formats\n");
	aflib_info("format\t\tdescription");

   for ( it = modules_list.begin(); it != modules_list.end(); it++)
   {
		aflib_info("%-10s\t%s",
				(*it)->getFormat().c_str(),
				(*it)->getDescription().c_str()
			);
   }

}

void
audioChain::help(int argc, char* argv[])
{
  	aflib_info("osalp [mode [mode options]] [format options] infile [ [format options] outfile ]");
	
	aflib_info("\nmode:");
	aflib_info( "-a            Print availible formats ");
	aflib_info( "-h            Print this help message ");
	aflib_info( "-p <factor>   Pitch");
	aflib_info( "-i <filename> Print info about file ");
	
	aflib_info("\npitch options:");
	aflib_info( "--linear      Linear interpolation");
	aflib_info( "--high        High quality");
	aflib_info( "--filter      Filter interpolation");
	
	aflib_info( "\nformat options:");
	aflib_info( "-f            File format");
	aflib_info( "-c            Number of channels");
	aflib_info( "-r            Sampling Rate");
	aflib_info( "-b            Data is signed byte");
	aflib_info( "-B            Data is unsigned byte");
	aflib_info( "-w            Data is signed word");
	aflib_info( "-W            Data is unsigned word");

   list<aflibFileItem*> modules_list;
   list<aflibFileItem*>::iterator  it;

   modules_list = aflibFile::returnSupportedFormats();

	aflib_info("\nfile formats:");

	string formats;
	
   for ( it = modules_list.begin(); it != modules_list.end(); it++)
   {
		formats += (*it)->getFormat().c_str();
		formats += " ";
   }

	aflib_info("%s",formats.c_str());

}

void
audioChain::date(
		int argc,
		char* argv[]
  ) 

{

	aflibDateTime datetime;

	datetime.setCurrentTime();

	cout << datetime << endl;
}

#define PLAY_DELTA  4096 

int
audioChain::process(aflibAudio* output)
{
	// This function continues to process PLAY_DELTA until
	// AFLIB_END_OF_FILE is reached or another error is
	// encountered.

   aflibStatus  status;

	int num_samples;
	long position = 0;

   // IF audio chain is not setup correctly then we are done
   if (output == NULL)
      return (false);

	do
	{
   	num_samples = PLAY_DELTA;
	   output->process(status, position, num_samples);
		position += num_samples;
	} 
	while(num_samples);

	return (true);
	
}

void
audioChain::parseConfigArgs(
		int argc, 
		char **argv, 
		aflibConfig& config,
		string& format) 
{
	
	int optchar;
	int findmore = 1;
   while (findmore)
	{
   	optchar = getopt(argc, argv, "+f:r:c:bBwW");
		switch(optchar)
		{
			case 'b':
         	config.setSampleSize(AFLIB_DATA_8S);
			break;
			case 'B':
         	config.setSampleSize(AFLIB_DATA_8U);
			break;
			case 'w':
         	config.setSampleSize(AFLIB_DATA_16S);
			break;
			case 'W':
         	config.setSampleSize(AFLIB_DATA_16U);
			break;

			case 'f':
				if(optind > argc){
					aflib_fatal("Must specify a file format from %s -a",argv[0]);
			  	} else { 
					format = optarg;
				}
			break;
			case 'r':
         	if (optind > argc){
	            aflib_fatal("Must specify a sample rate with -r option");
         	} else {
            	config.setSamplesPerSecond(atoi(optarg));
   	      }
			break;
			case 'c':
         	if (optind > argc){
	            aflib_fatal("Must specify channels with -c option");
         	} else {
            	config.setChannels(atoi(optarg));
   	      }
			break;
			case '?':
				optind--;
				findmore = 0;
			break;
			case EOF:
				findmore = 0;
			break;
		}
	}
}

