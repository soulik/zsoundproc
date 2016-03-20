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
#include <stdlib.h>
#include <sys/stat.h>
#include <iostream>
using std::cerr;
using std::endl;

#include "aflibEnvFile.h"


#define DEFAULT_DIR ".aflib"
#define DEFAULT_FILE "aflibenv"
#define MAX_LINE_SIZE  2048


/*! \brief Constructor with parameters.

    This will create a environment directory name to use with env_dir. If this is NULL
    then a default directory in the users home directory will be used. If env_file is
    set then this file name will be used in the env_dir. Otherwise a default file name
    will be used in the env_dir.
*/
aflibEnvFile::aflibEnvFile(
   char * env_file,
   char * env_dir)
{
   char * home_dir;

   // Store the correct environment directory
   if (env_dir == NULL)
   {
      if (getenv("HOME"))
      {
         home_dir = getenv("HOME");
         _env_file.append(home_dir);
         _env_file.append("/");
      }
      else
      {
         cerr << "Environment var HOME not set!" << endl;
      }

      _env_file.append(DEFAULT_DIR);
   }
   else
   {
      _env_file.append(env_dir);
   }

   _env_file.append("/");

   // Store the correct environment file
   if (env_file == NULL)
   {
      _env_file.append(DEFAULT_FILE);
   }
   else
   {
      _env_file.append(env_file);
   }
}


/*! Constructor with no parameters.

   This constructor will use defaults for the directory and env files.
*/
aflibEnvFile::aflibEnvFile()
{
   char * home_dir;

   if (getenv("HOME"))
   {
      home_dir = getenv("HOME");
      _env_file.append(home_dir);
      _env_file.append("/");
   }
   else
   {
      cerr << "Environment var HOME not set!" << endl;
   }

   _env_file.append(DEFAULT_DIR);
   _env_file.append("/");

   _env_file.append(DEFAULT_FILE);
}


/*! \brief Destructor.
*/
aflibEnvFile::~aflibEnvFile()
{
}


/*! \brief Read a value from a file.

    This function will read a string from the env file that corresponds to the
    key key_str passed in.
*/
bool
aflibEnvFile::readValueFromFile(
   string& key_str,
   string& results)
{
   FILE *  fd = 0;
   char  buf[MAX_LINE_SIZE];
   bool ret_flag = false;

   /* open file */
   fd = fopen(_env_file.c_str(), "r");

   if (fd)
   {
      while (fgets(buf, MAX_LINE_SIZE-1, fd))
      {
         strtok(buf, "\n");
         /* find key in file */
         if (strstr(buf, key_str.c_str()))
         {
            results = (buf + strlen(key_str.c_str()));
            ret_flag = true;
         }
      }
      fclose(fd);
   }

   return(ret_flag);
}


/*! \brief Write a value to a file.

    This function will write a value for the key key_str. If the key_str
    already exists then it will be overwritten.
*/
void
aflibEnvFile::writeValueToFile(
   string& key_str,
   string& value)
{
   FILE *  fd = 0;
   char * dir_str;
   char * dir_ptr;
   char  buf[MAX_LINE_SIZE];
   int   lines = 0;
   char * str_arrays[1024];
   int   i;


   dir_str = strdup(_env_file.c_str());
   if (dir_str)
   {
      /* remove file from the directory string */
      dir_ptr = strrchr(dir_str, (int)'/');
      if (dir_ptr)
      {
         *dir_ptr = 0;

         /* make the directory path if it does not exist */
         mkdir(dir_str, S_IRUSR | S_IWUSR | S_IXUSR |
                        S_IRGRP | S_IWGRP | S_IXGRP |
                        S_IROTH | S_IXOTH);

         fd = fopen(_env_file.c_str(), "r");
         /* If file exists we need to save contents */
         if (fd)
         {
            while (fgets(buf, MAX_LINE_SIZE-1, fd) && lines < 1024 - 1)
            {
               str_arrays[lines] = (char *)malloc(strlen(buf) + 1);
               strcpy(str_arrays[lines], buf);
               lines++;
            }
            fclose(fd);
         }

         /* open resource file */
         fd = fopen(_env_file.c_str(), "w");
         if (fd)
         {
            for (i = 0; i < lines; i++)
            {
               /* IF not the key string then write out to file */
               if (!strstr(str_arrays[i], key_str.c_str()))
               {
                  fprintf(fd, "%s", str_arrays[i]);
                  free(str_arrays[i]);
               }
            }

            fprintf(fd, "%s%s\n", key_str.c_str(), value.c_str());
            fclose(fd);
         }
         else
         {
            cerr << "Can't open file " << _env_file.c_str() << endl;
         }
      }
      free(dir_str);
   }
}


