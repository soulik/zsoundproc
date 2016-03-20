lib_LTLIBRARIES = libaflibAiffFile.la 
INCLUDES=-I$(top_srcdir)/aflib -I../@AUDIOFILE_PATH@/libaudiofile

libaflibAiffFile_la_SOURCES = \
aflibAiffFile.cc \
aflibAiffFile.h 
libaflibAiffFile_la_LIBADD   = ../@AUDIOFILE_PATH@/libaudiofile/libaudiofile.la 
libaflibAiffFile_la_LDFLAGS   = -module -release $(VERSION) 
libdir=@libdir@/aflib
