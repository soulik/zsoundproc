AUTOMAKE_OPTIONS = foreign
SUBDIRS = aflib formatlib doc 

dist-hook:
	mkdir $(distdir)/examples
	cp -R $(srcdir)/examples/player $(distdir)/examples/player
	cp -R $(srcdir)/examples/editor $(distdir)/examples/editor
	cp -R $(srcdir)/examples/osalp $(distdir)/examples/osalp
