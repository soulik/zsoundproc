#  flac - Command-line FLAC encoder/decoder
#  Copyright (C) 2002,2003,2004,2005  Josh Coalson
#
#  This program is free software; you can redistribute it and/or
#  modify it under the terms of the GNU General Public License
#  as published by the Free Software Foundation; either version 2
#  of the License, or (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

AUTOMAKE_OPTIONS = foreign

SUBDIRS = . html

if OSALP__HAS_DOXYGEN
OSALP.tag: doxygen.config
	doxygen doxygen.config 
	rm -rf html/api
	mv doxytmp/html html/api
	rm -rf doxytmp
else
OSALP.tag:
	echo "*** Warning: Doxygen not found; documentation will not be built."
	touch $@
	mkdir -p html/api
endif

docdir = $(datadir)/doc/$(PACKAGE)-$(VERSION)

# doc_DATA = \
#	OSALP.tag

EXTRA_DIST = doxygen.config $(doc_DATA)

maintainer-clean-local:
	rm -rf html/api doxytmp
