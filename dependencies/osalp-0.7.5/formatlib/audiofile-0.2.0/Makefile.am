## Process this file with automake to produce Makefile.in

bin_SCRIPTS = \
         audiofile-config

SUBDIRS = libaudiofile 

EXTRA_DIST= \
	ACKNOWLEDGEMENTS \
	NOTES \
	README \
	TODO \
	COPYING.GPL \
	configure configure.in\
	audiofile.m4 \
	audiofile.spec.in

m4datadir = $(datadir)/aclocal
m4data_DATA = audiofile.m4

dist-hook: audiofile.spec
	cp audiofile.spec $(distdir)
