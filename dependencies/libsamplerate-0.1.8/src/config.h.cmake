/* src/config.h.in.  Generated from configure.ac by autoheader.  */

/* Set to 1 if the compile is GNU GCC. */
#cmakedefine COMPILER_IS_GCC @COMPILER_IS_GCC@

/* Target processor clips on negative float to int conversion. */
#define CPU_CLIPS_NEGATIVE @CPU_CLIPS_NEGATIVE@

/* Target processor clips on positive float to int conversion. */
#define CPU_CLIPS_POSITIVE @CPU_CLIPS_POSITIVE@

/* Target processor is big endian. */
#cmakedefine CPU_IS_BIG_ENDIAN @CPU_IS_BIG_ENDIAN@

/* Target processor is little endian. */
#cmakedefine CPU_IS_LITTLE_ENDIAN @CPU_IS_LITTLE_ENDIAN@

/* Major version of GCC or 3 otherwise. */
#cmakedefine GCC_MAJOR_VERSION @GCC_MAJOR_VERSION@

/* Define to 1 if you have the `alarm' function. */
#cmakedefine HAVE_ALARM 1

/* Define to 1 if you have the `calloc' function. */
#cmakedefine HAVE_CALLOC 1

/* Define to 1 if you have the `ceil' function. */
#cmakedefine HAVE_CEIL 1

/* Define to 1 if you have the <dlfcn.h> header file. */
#cmakedefine HAVE_DLFCN_H 1

/* Set to 1 if you have libfftw3. */
#cmakedefine HAVE_FFTW3 1

/* Define to 1 if you have the `floor' function. */
#cmakedefine HAVE_FLOOR 1

/* Define to 1 if you have the `fmod' function. */
#cmakedefine HAVE_FMOD 1

/* Define to 1 if you have the `free' function. */
#cmakedefine HAVE_FREE 1

/* Define to 1 if you have the <inttypes.h> header file. */
#cmakedefine HAVE_INTTYPES_H 1

/* Define to 1 if you have the `m' library (-lm). */
#cmakedefine HAVE_LIBM 1

/* Define if you have C99's lrint function. */
#cmakedefine HAVE_LRINT 1

/* Define if you have C99's lrintf function. */
#cmakedefine HAVE_LRINTF 1

/* Define to 1 if you have the `malloc' function. */
#cmakedefine HAVE_MALLOC 1

/* Define to 1 if you have the `memcpy' function. */
#cmakedefine HAVE_MEMCPY 1

/* Define to 1 if you have the `memmove' function. */
#cmakedefine HAVE_MEMMOVE 1

/* Define to 1 if you have the <memory.h> header file. */
#cmakedefine HAVE_MEMORY_H 1

/* Define if you have signal SIGALRM. */
#cmakedefine HAVE_SIGALRM 1

/* Define to 1 if you have the `signal' function. */
#cmakedefine HAVE_SIGNAL 1

/* Set to 1 if you have libsndfile. */
#cmakedefine HAVE_SNDFILE 1

/* Define to 1 if you have the <stdint.h> header file. */
#cmakedefine HAVE_STDINT_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#cmakedefine HAVE_STDLIB_H 1

/* Define to 1 if you have the <strings.h> header file. */
#cmakedefine HAVE_STRINGS_H 1

/* Define to 1 if you have the <string.h> header file. */
#cmakedefine HAVE_STRING_H 1

/* Define to 1 if you have the <sys/stat.h> header file. */
#cmakedefine HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/times.h> header file. */
#cmakedefine HAVE_SYS_TIMES_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#cmakedefine HAVE_SYS_TYPES_H 1

/* Define to 1 if you have the <unistd.h> header file. */
#cmakedefine HAVE_UNISTD_H 1

/* Define to the sub-directory in which libtool stores uninstalled libraries.
   */
#cmakedefine LT_OBJDIR "@LT_OBJDIR@"

/* Define to 1 if your C compiler doesn't accept -c and -o together. */
#cmakedefine NO_MINUS_C_MINUS_O 1

/* Set to 1 if compiling for Win32 */
#cmakedefine OS_IS_WIN32 1

/* Name of package */
#cmakedefine PACKAGE "@PACKAGE@"

/* Define to the address where bug reports for this package should be sent. */
#cmakedefine PACKAGE_BUGREPORT "@PACKAGE_BUGREPORT@"

/* Define to the full name of this package. */
#cmakedefine PACKAGE_NAME "@PACKAGE_NAME@"

/* Define to the full name and version of this package. */
#cmakedefine PACKAGE_STRING "@PACKAGE_STRING@"

/* Define to the one symbol short name of this package. */
#cmakedefine PACKAGE_TARNAME "@PACKAGE_TARNAME"

/* Define to the home page for this package. */
#cmakedefine PACKAGE_URL "@PACKAGE_URL@"

/* Define to the version of this package. */
#cmakedefine PACKAGE_VERSION "@PACKAGE_VERSION@"

/* The size of `double', as computed by sizeof. */
#cmakedefine SIZEOF_DOUBLE @SIZEOF_DOUBLE@

/* The size of `float', as computed by sizeof. */
#cmakedefine SIZEOF_FLOAT @SIZEOF_FLOAT@

/* The size of `int', as computed by sizeof. */
#cmakedefine SIZEOF_INT @SIZEOF_INT@

/* The size of `long', as computed by sizeof. */
#cmakedefine SIZEOF_LONG @SIZEOF_LONG@

/* Define to 1 if you have the ANSI C header files. */
#cmakedefine STDC_HEADERS 1

/* Version number of package */
#cmakedefine VERSION "@VERSION@"

#cmakedefine inline @C_INLINE@
