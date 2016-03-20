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


// Base class for audio file reading and writing
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


#include "aflibFile.h"
#include "aflibFileItem.h"

#include "aflibDebug.h"

#include <stdio.h>  //For findModuleFormat
#include <stdlib.h>
#include <dlfcn.h>
#include <unistd.h> // for setenv and unsetenv
#include <iostream>
using std::cerr;
using std::endl;

#include <dirent.h>
#include <sys/types.h>

#ifdef NO_MODULES
#include <aflibDevFile.h>
#include <aflibLameFile.h>
#include <aflibWavFile.h>
#include <aflibAuFile.h>
#endif

#define MODULE_FORMAT "Format:"
#define MODULE_DSCR "Description:"
#define MODULE_EXT "Extension:"
#define MODULE_MAGIC "Magic:"
#define MODULE_NAME "Name:"
#define MODULE_VALUE1 "Value1:"
#define MODULE_VALUE2 "Value2:"
#define MODULE_VALUE3 "Value3:"


list<aflibFileItem *> aflibFile::_support_list;
bool                  aflibFile::_list_created = FALSE;


aflibFile::aflibFile(const char * module_format)
{
	// This is a constructor that will load the proper shared object based on
	// the format string that is passed in. Once the shared object is loaded it will
	// then call the wrapper function to allocate an object for the derived
	// aflibFile class. This object that is allocate is the actual object that
	// that will be used to call the member functions. But it will be called
	// thru this object. This object is the one that the user will see. After
	// the file object is obtained then it will be used to call the derived
	// objects constructor.

	_lib1 = NULL;
	_file_object = NULL;

#ifndef NO_MODULES 
	const char * error_msg;
	string module_name_full;
	list<aflibFileItem *>::iterator  it;
	string module_name;

	/* TODO should probably just include the fullpath and name in
	 * aflibAudioItem.  This will allow for future support of various
	 * directories to contain modules 
	 */
	if (getenv("AFLIB_MODULE_FILE_DIR"))
	{
		module_name_full = getenv("AFLIB_MODULE_FILE_DIR");
	}
	else
	{
		module_name_full = MODULE_INSTALL_DIR;
	}
	module_name_full.append("/lib");

	/* TODO make _support_list a map and key on module_format */
	// Set the format for this object and find modules name and values
	for (it = _support_list.begin(); it != _support_list.end(); it++)
	{
		if (strcmp((*it)->getFormat().c_str(), module_format) == 0)
		{
			_format = module_format;
			_value1 = (*it)->getValue1();
			_value2 = (*it)->getValue2();
			_value3 = (*it)->getValue3();
			module_name = (*it)->getName();
			break;
		}
	}

	if(module_name.length() == 0) return;

	module_name_full.append(module_name);
	module_name_full.append(".so");

	_lib1 = dlopen(module_name_full.c_str(), RTLD_LAZY );
	
	// IF we can't load module then we are done
	if(_lib1 == NULL)
	{
		error_msg = dlerror();
		aflib_warning("%s", error_msg);
		return;
	}
	
	aflibFile * (*func_ptr)() ;

	func_ptr = (aflibFile *(*)()) dlsym(_lib1, "getAFileObject");

	if (func_ptr == NULL)
	{
		error_msg = dlerror();
		aflib_warning("%s", error_msg);
		return;
	}
	// call getAFileObject in derived class and get a pointer to allocated object
	_file_object = func_ptr();
	
	// Pass the value parameters to the file object so derived objects can use it
	if (_file_object)
   {
		_file_object->setValue1(_value1);
		_file_object->setValue2(_value2);
		_file_object->setValue3(_value3);
	}

#endif
}

aflibFile::aflibFile()
{
	_lib1 = NULL;
	_file_object = NULL;
}


/*! \brief Destructor.
 */
aflibFile::~aflibFile()
{
	// IF a file object was created then delete it now
	delete _file_object;
	_file_object = NULL;

	// IF a library is currently open then close it. We have to close the library
	// after the delete above so the destructor can be called.
	if (_lib1)
   {
		dlclose(_lib1);
   }
}


/*! \brief Main static function to open a file or device for reading based on aflibFileType.

  This is a static function of this class. Users call this function as the
  main API to open an audio file or device for reading. Since this is a
  static member function it is not associated with any aflibFile object and
  thus no data members can be accessed. The user passes in the file type
  to be opened. This can be one of the defined types or AFLIB_AUTO_TYPE. This
  will search the list of supported formats for a match and use that format. If
  AFLIB_AUTO_TYPE is specified then it will open the file and use a format
  that matches that file type based on information supplied by each module.
  Also passed is the file name which should
  contain the file and directory in which the file can be found. Passed back
  to the user will be a cfg structure filled that was allocated by the
  caller. This will tell information about the data. Also will be a status
  to indicate if any errors occurred.
 */
aflibFile *
aflibFile::open(
		aflibFileType type_enum,
		const string& file,
		aflibConfig* cfg,
		aflibStatus* status)
{
	aflibFile * file_return = NULL;
	aflibStatus ret_status = AFLIB_ERROR_OPEN;
	const char * module_name = NULL;


	// Parse the module file if not done yet
	parseModuleFile();

	// IF user wants auto recognition
	/* TODO move auto type to a module */
	if (type_enum == AFLIB_AUTO_TYPE)
	{
		if((module_name = findModuleFile(file)) != NULL)
			// Allocate the real file pointer to return to user
			file_return = allocateModuleFile(type_enum, module_name);
		else
			ret_status = AFLIB_CANT_AUTO_TYPE; 
	} else {
		// Allocate the real file pointer to return to user
		file_return = allocateModuleFile(type_enum, module_name);
	}
 

	if (file_return)
	{
		// For module support this calls base class function, for no module
		// support this calls derived class
		ret_status = file_return->afopen(file.c_str(), cfg);
	}
	else
	{
		// Unable to allocate a module so error
		ret_status = AFLIB_ERROR_INITIALIZATION_FAILURE;
   }

	if (status != NULL)
   {
		*status = ret_status;
	}
	return (file_return);
}


/*! \brief Main static function to open a file or device for reading based on format string.

  This is a static function of this class. Users call this function as the
  main API to open an audio file or device for reading. Since this is a
  static member function it is not associated with any aflibFile object and
  thus no data members can be accessed. The user passes in the file type
  to be opened. This must be one of the file types supplied by the function
  aflibFile::returnSupportedFormats. Also passed is the file name which should
  contain the file and directory in which the file can be found. Passed back
  to the user will be a cfg structure filled that was allocated by the
  caller. This will tell information about the data. Also will be a status
  to indicate if any errors occurred.
 */
aflibFile *
aflibFile::open(
		const string& format,
		const string& file,
		aflibConfig* cfg,
		aflibStatus* status)
{
	aflibFile * file_return = NULL;
	aflibStatus ret_status = AFLIB_ERROR_OPEN;

	const char * module_name = NULL;

	// Parse the module file if not done yet
	parseModuleFile();

	// IF user wants auto recognition
	/* TODO move auto type to a module */
	if (format == "AUTO")
	{
		if((module_name = findModuleFile(file)) != NULL)
			// Allocate the real file pointer to return to user
			file_return = allocateModuleFile(AFLIB_AUTO_TYPE, module_name);
		else
			ret_status = AFLIB_CANT_AUTO_TYPE; 
	} else {
		// Allocate the real file pointer to return to user
		file_return = allocateModuleFile(AFLIB_AUTO_TYPE, format.c_str());
	}
 

	if (file_return)
   {
		// For module support this calls base class function, for no module
		// support this calls derived class
		ret_status = file_return->afopen(file.c_str(), cfg);
   }
   else
   {
		// Unable to allocate a module so error
		ret_status = AFLIB_ERROR_INITIALIZATION_FAILURE;
   }

	if (status != NULL)
	{
		*status = ret_status;
   }
	return (file_return);
}


/*! \brief Main static function for writing to a device or file by type.

  This is a static function of this class. Users call this function as the
  main API to open an audio file or device for writing. Since this is a
  static member function it is not associated with any aflibFile object and
  thus no data members can be accessed. The user passes in the file type
  to create. The user also specifies the file name. The specific type of
  audio data is specified in the cfg parameter. The status of this operation
  will be returned to the user as status.
  */
aflibFile *
aflibFile::create(
		aflibFileType type_enum,
		const string& file,
		const aflibConfig& cfg,
		aflibStatus* status)
{
	aflibFile * file_return = NULL;
	aflibStatus ret_status = AFLIB_ERROR_OPEN;


	// Parse the module file if not done yet
	parseModuleFile();

	// Allocate the real file pointer to return to user
	file_return = allocateModuleFile(type_enum, NULL);

	if (file_return)
	{
		// For module support this calls base class function, for no module
		// support this calls derived class
		ret_status = file_return->afcreate(file.c_str(), cfg);
	}
	else
	{
		// Unable to allocate a module so error
		ret_status = AFLIB_ERROR_INITIALIZATION_FAILURE;
	}

	// If user wants status returned then return it
	if (status != NULL)
	{
		*status = ret_status;
	}
	return (file_return);
}


/*! \brief Main static function for writing to a device or file by format.

  This is a static function of this class. Users call this function as the
  main API to open an audio file or device for writing. Since this is a
  static member function it is not associated with any aflibFile object and
  thus no data members can be accessed. The user passes in the file format
  to create. The user also specifies the file name. The specific type of
  audio data is specified in the cfg parameter. The status of this operation
  will be returned to the user as status.
 */
aflibFile *
aflibFile::create(
		const string& format,
		const string& file,
		const aflibConfig& cfg,
		aflibStatus* status)
{
	aflibFile * file_return = NULL;
	aflibStatus ret_status = AFLIB_ERROR_OPEN;


	// Parse the module file if not done yet
	parseModuleFile();

	file_return = allocateModuleFile(AFLIB_AUTO_TYPE, format.c_str());

	if (file_return)
   {
		// For module support this calls base class function, for no module
		// support this calls derived class
		ret_status = file_return->afcreate(file.c_str(), cfg);
	}
	else
	{
		// Unable to allocate a module so error
		ret_status = AFLIB_ERROR_INITIALIZATION_FAILURE;
	}
 
	// If user wants status returned then return it
	if (status != NULL)
	{
		*status = ret_status;
   }
	return (file_return);
}


/*! \brief Return the current file or device format.
 */
const string&
aflibFile::getFormat() const
{
	return (_format);
}


/*! \brief Set a file format unique value number 1.
 */
	void
aflibFile::setValue1(const string& value)
{
	_value1 = value;
}


/*! \brief Set a file format unique value number 2.
 */
	void
aflibFile::setValue2(const string& value)
{
	_value2 = value;
}


/*! \brief Set a file format unique value number 3.
 */
	void
aflibFile::setValue3(const string& value)
{
	_value3 = value;
}


/*! \brief Main API for opening a file or device in read mode.

    This function is only used for module support. For no modules support the
  derived classes afopen function will be called instead. For module support
    this function will make a call to the derived classes function. 
  */
aflibStatus
aflibFile::afopen(
		const char * file,
		aflibConfig* cfg)
{
	aflibStatus status = AFLIB_SUCCESS;

	// IF we are using modules
#ifndef NO_MODULES
	if(initialized()==true)
	{
		status = _file_object->afopen(file, cfg);

		// Get the config parameter from the file object and store in this object
		if (cfg)
		{
			*cfg = _file_object->getOutputConfig();
		}
	}
	else
	{
		status = AFLIB_ERROR_INITIALIZATION_FAILURE;
	}
#endif

	return (status);
}


/*! \brief Main API for opening a file or device in write mode.

    This function is only used for module support. For no module support the
    derived classes afcreate function will be called instead. For module support
    this function will make a call to the derived classes function. 
  */
aflibStatus
aflibFile::afcreate(
		const char * file,
		const aflibConfig& cfg)
{
	aflibStatus status = AFLIB_SUCCESS;
   
	if(_file_object)
	{
		status = _file_object->afcreate(file, cfg);
	}
	else
	{
		status = AFLIB_ERROR_INITIALIZATION_FAILURE;
	}

	return (status);
}


/*! \brief Main API function to read a segment of data.

    This function is only used for module support. For no module support the
    derived classes afread function will be called instead. For module support
    this function will make a call to the derived classes function. 
  */
aflibStatus
aflibFile::afread(
   aflibData& data,
		long long position)
{
	aflibStatus status = AFLIB_SUCCESS;
   
	if(_file_object)
	{
		status  = _file_object->afread(data, position);
	}
	else
	{
		status = AFLIB_ERROR_INITIALIZATION_FAILURE;
	}

	return (status);
}


/*! \brief Main API function to write a segment of audio data.

    This function is only used for module support. For no module support the
    derived classes afwrite function will be called instead. For module support
    this function will make a call to the derived class.
  */
aflibStatus
aflibFile::afwrite(
   aflibData& data,
		long long position)
{
	aflibStatus status = AFLIB_SUCCESS;
 
	if(_file_object)
	{
		status  = _file_object->afwrite(data, position);
	}
	else
	{
		status = AFLIB_ERROR_INITIALIZATION_FAILURE;
	}
 
	return (status);
}


/*! \brief Provides the ability to set format specific information.

  This provides the ability to set specific information relating to a specific
  format. One passes an item that is a specific character string that will be
  recognized by a format. One also passes a void pointer to a data item that is
  the data to be set. The documentation for the specific item will say what
  format the data should be. TRUE will be returned if the item was processed
  successfully otherwise FALSE will be returned.
  */
bool
aflibFile::setItem(
		const char * item,
		const void * value)
{
	bool status = FALSE;

	if(_file_object)
	{
		status  = _file_object->setItem(item, value);
	}
	else
	{
		status = AFLIB_ERROR_INITIALIZATION_FAILURE;
    }

	return (status);
}
  

/*! \brief Provides the ability to get format specific information.

  This provides the ability to get specific information relating to a specific
  format. One passes an item that is a specific character string that will be
  recognized by a format. One also passes a void pointer to an allocated object
  of the correct type. This data will be filled with the correct information.
  The documentation for the specific item will say what format the data should
  be. TRUE will be returned if the item was processed successfully otherwise
  FALSE will be returned.
  */
bool
aflibFile::getItem(
		const char * item,
		void * value)
{
	bool status = FALSE;

	if(_file_object)
	{
		status  = _file_object->getItem(item, value);
	}
	else
	{
		status = AFLIB_ERROR_INITIALIZATION_FAILURE;
	}

	return (status);
}

	void
aflibFile::setInputConfig(const aflibConfig& cfg)
{
	// This is a virtual function that derived classes can override if needed.
	// It allows the caller to set the configuration of the audio data of an
	// object.


	if (_file_object)
		_file_object->setInputConfig(cfg);
	else
		_cfg_input = cfg;
}


const aflibConfig&
aflibFile::getInputConfig() const
{
	// This function will return the input audio data configuration

	if (_file_object)
		return (_file_object->getInputConfig()); 
	else
		return (_cfg_input);
}

	void
aflibFile::setOutputConfig(const aflibConfig& cfg)
{
	// This function allows one to store the output audio configuration
	// for a derived class.

	if (_file_object)
		_file_object->setOutputConfig(cfg);
	else
		_cfg_output = cfg;
}

const aflibConfig&
aflibFile::getOutputConfig() const
{
	// This function will return the output audio data configuration

	if (_file_object)
		return (_file_object->getOutputConfig()); 
	else
		return (_cfg_output);
}

	bool
aflibFile::isDataSizeSupported(aflib_data_size size)
{
	// See the description for aflibAudio::isDataSizeSupported.

	bool ret_value = TRUE;


	if (_file_object)
		ret_value = _file_object->isDataSizeSupported(size);

	return (ret_value);
}

	bool
aflibFile::isEndianSupported(aflib_data_endian end)
{
	// See the description for aflibAudio::isEndianSupported.

	bool ret_value = TRUE;


	if (initialized()==true)
		ret_value = _file_object->isEndianSupported(end);

	return (ret_value);
}

	bool
aflibFile::isSampleRateSupported(int& rate)
{
	// See the description for aflibAudio::isSampleRateSupported.

	bool ret_value = TRUE;


	if (_file_object)
		ret_value = _file_object->isSampleRateSupported(rate);

	return (ret_value);
}

	bool
aflibFile::isChannelsSupported(int& channels)
{
	// See the description for aflibAudio::isChannelsSupported.

	bool ret_value = TRUE;


	if (_file_object)
		ret_value = _file_object->isChannelsSupported(channels);

	return (ret_value);
}




const char *
aflibFile::findModuleFile(
		const string& file_name)
{
	/* 
	 * FIXME: Shouldn't use FILE.  Doesn't work well for unseekable streams. 
	 * I think we could use a module for this and have it open up the proper
	 * format module.
	 * */
	const char * module_format = NULL;
	FILE * fd;
	list<aflibFileItem *>::iterator it;
	vector<unsigned char> array;
	bool recognized = FALSE;
	int i;


	// Open the audio file
	fd = fopen(file_name.c_str(), "r");
	if (fd != NULL)
	{
		// Read first 1000 bytes to be used to ID the file
		for (i = 0; i < 1000; i++)
		{
			array.push_back((unsigned char)fgetc(fd));
   }
		fclose(fd);

		// Loop thru every format to see if it is the file
		for (it = _support_list.begin(); it != _support_list.end(); it++)
		{
			// Is this the correct format
			if (*(*it) == array)
				recognized = TRUE;

			if (recognized == TRUE)
			{
				module_format = (*it)->getFormat().c_str();
				break;
			}
		}
	}

	return(module_format);
}


aflibFile *
aflibFile::allocateModuleFile(
		aflibFileType type_enum,
		const char *  module_format)
{
	// This is a static private function. It allocates an aflibFile object for
	// modules support. For no module support it allocates an object of the
	// derived class. For this users call the derived functions directly. For
	// module support the base class functions are called. Since this is really
	// just a wrapper aflibFile object it contains a pointer to the real derived
	// object.

	aflibFile * file_return = NULL;


#ifndef NO_MODULES
	/* TODO should'nt hardcode filetypes.  should just use strings - DAS */
	if (type_enum == AFLIB_AUTO_TYPE)
	{
		if (module_format != NULL)
			file_return = new aflibFile(module_format);
	}
	else if (type_enum == AFLIB_DEV_TYPE)
	{
		file_return = new aflibFile("DEVICE");
	}
	else if (type_enum == AFLIB_MPEG_TYPE)
	{
		file_return = new aflibFile("MP3(LAME) 48Khz");
	}
	else if (type_enum == AFLIB_WAV_TYPE)
	{
		file_return = new aflibFile("WAV");
	}
	else if (type_enum == AFLIB_AU_TYPE)
	{
		file_return = new aflibFile("AU");
	}

	// Check to see if the correct module was loaded. If not return NULL as error
	if (file_return)
	{
		if (file_return->initialized() == FALSE)
		{
			delete file_return;
			file_return = NULL;
		}
	}
#else
	if (type_enum == AFLIB_DEV_TYPE)
	{
		file_return = new aflibDevFile();
	}
	else if (type_enum == AFLIB_MPEG_TYPE)
	{
		file_return = new aflibLameFile();
	}
	else if (type_enum == AFLIB_WAV_TYPE)
	{
		file_return = new aflibWavFile();
	}
	else if (type_enum == AFLIB_AU_TYPE)
	{
		file_return = new aflibWavFile();
	}
#endif

	return (file_return);
}


	void
aflibFile::parseModuleFile()
{
	string  path;

	DIR* dir_handle;
	void* dl_handle;
	void (*query)(list<aflibFileItem*>&);
	struct dirent* entry;
	const char* dl_errstr;
	string module_name;
	string module_path_name;
			
	// We are creating a static list. Has it been created yet
	if (_list_created == TRUE)
		return;

	_list_created = TRUE;

	/*TODO should save this path in aflibFileItem class. */
	if (getenv("AFLIB_MODULE_FILE_DIR"))
	{
		path = getenv("AFLIB_MODULE_FILE_DIR");
	}
	else
	{
		path = MODULE_INSTALL_DIR;
			}
			

	if((dir_handle = opendir(path.c_str())) == NULL ) return;
	path += "/";
	for(entry = readdir(dir_handle);entry;entry = readdir(dir_handle))
	{
		module_name = entry->d_name;

		if((module_name.find("aflib") < module_name.length())&&
				(module_name.find("File.so") < module_name.length()))
		{
			module_path_name = path + module_name;

			dl_handle = dlopen(module_path_name.c_str(),RTLD_LAZY);

			if(dl_handle == NULL)
			{
				dl_errstr = dlerror();
				aflib_debug("%s",dl_errstr);
				continue;
			}

			query = (void (*)(list<aflibFileItem*>&) ) dlsym(dl_handle,"query");

			if(query==NULL)
			{
				dl_errstr = dlerror();
				aflib_warning("%s",dl_errstr);
			} 
			else
			{
				(query)(_support_list);
				aflib_debug("Recognized file module %s",module_name.c_str());
			}
			dlclose(dl_handle);
		}
	}
	closedir(dir_handle);
}


	bool
aflibFile::initialized()
{
	// For the dynamic load logic this will let static functions determine if
	// the module to be loaded was initialized correctly.

	return(_file_object != NULL);
}


/*! \brief Returns all currently supported formats.

  This public static function returns to the user two lists. The first
  is the list of supported formats. The second is a corresponding list of
  descriptions that relate to the formats returned.
  */ 
void
aflibFile::returnSupportedFormats(
		list <string>& formats,
		list <string>& descriptions)
{
	list<aflibFileItem *>::iterator  it;


	// IF we have not parsed the modules file then do so
	parseModuleFile();

	for (it = _support_list.begin(); it != _support_list.end(); it++)
	{
		formats.push_back((*it)->getFormat());
		descriptions.push_back((*it)->getDescription());
   }
}

/*! \brief Returns all currently supported formats.

  This public static function returns a reference to the supported module
  formats.  See aflibFileItem.h for details. 
 */
const
	list<aflibFileItem*>&
aflibFile::returnSupportedFormats ()  
{
	// IF we have not parsed the modules file then do so
	parseModuleFile();
	return _support_list;
}

