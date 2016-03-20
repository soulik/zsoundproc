lib_LTLIBRARIES = libaflibMpgFile.la
INCLUDES=-I$(top_srcdir)/aflib
libaflibMpgFile_la_SOURCES = aflibMpgFile.cc aflibMpgFile.h soundinputstream.cc fileinput.cc httpinput.cc \
	mpegtable.cc filter.cc filter_2.cc \
	mpegtoraw.cc mpeglayer1.cc mpeglayer2.cc \
	mpeglayer3.cc bitwindow.cc huffmantable.cc \
	mpegsound.h mpegsound_locals.h

libaflibMpgFile_la_LDFLAGS = -module -release $(VERSION)

libdir=@libdir@/aflib
