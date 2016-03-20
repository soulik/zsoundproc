lib_LTLIBRARIES = libaflibAuFile.la 
INCLUDES=-I$(top_srcdir)/aflib -I../@AUDIOFILE_PATH@/libaudiofile

libaflibAuFile_la_SOURCES = \
aflibAuFile.cc \
aflibAuFile.h 
libaflibAuFile_la_LIBADD   = ../@AUDIOFILE_PATH@/libaudiofile/libaudiofile.la 
libaflibAuFile_la_LDFLAGS   = -module -release $(VERSION) 
libdir=@libdir@/aflib
