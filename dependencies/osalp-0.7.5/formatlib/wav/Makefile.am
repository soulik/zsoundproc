lib_LTLIBRARIES = libaflibWavFile.la 
INCLUDES=-I$(top_srcdir)/aflib -I../@AUDIOFILE_PATH@/libaudiofile

libaflibWavFile_la_SOURCES = \
aflibWavFile.cc \
aflibWavFile.h 
libaflibWavFile_la_LIBADD = ../@AUDIOFILE_PATH@/libaudiofile/libaudiofile.la
libaflibWavFile_la_LDFLAGS   = -module -release $(VERSION) 
libdir=@libdir@/aflib
