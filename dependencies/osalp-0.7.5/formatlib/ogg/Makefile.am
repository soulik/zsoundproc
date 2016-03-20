lib_LTLIBRARIES = libaflibOggFile.la 
INCLUDES=-I$(top_srcdir)/aflib

libaflibOggFile_la_SOURCES = \
	aflibBufferUtils.cc \
	aflibBufferUtils.h \
	aflibOggFile.cc \
	aflibOggFile.h
libaflibOggFile_la_LDFLAGS   = -module -release $(VERSION) -logg -lvorbis -lvorbisfile
libdir=@libdir@/aflib
