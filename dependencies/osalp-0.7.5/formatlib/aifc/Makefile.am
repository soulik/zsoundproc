lib_LTLIBRARIES = libaflibAifcFile.la 
INCLUDES = -I$(top_srcdir)/aflib -I../@AUDIOFILE_PATH@/libaudiofile

libaflibAifcFile_la_SOURCES = \
aflibAifcFile.cc \
aflibAifcFile.h 
libaflibAifcFile_la_LIBADD   = ../@AUDIOFILE_PATH@/libaudiofile/libaudiofile.la 
libaflibAifcFile_la_LDFLAGS   = -module -release $(VERSION) 
libdir=@libdir@/aflib
