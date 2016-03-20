CFLAGS = -g -Wall \
		-I/usr/include/X11 \
		-I/usr/X11R6/include \
		-I/usr/local/include \
		-I/usr/local/include/aflib \
		-I.

OBJECTS = \
	LiteClue.o \
	main.o \
	ltdraw.o \
	lbdraw.o \
	rbdraw.o \
	rtdraw.o \
	gui.o \
	popup.o \
	audio.o

SOURCES = ${OBJECTS:.o=.cc}

all: player

player: ${OBJECTS}
	c++ -g -rdynamic -o player ${OBJECTS} \
	-L/usr/local/lib -laflib \
	-L/usr/X11/lib -L/usr/X11R6/lib -lXm -lXt -lX11 -ldl -lm -lc

clean:
	rm -rf ${OBJECTS} player

.c.o:
	gcc ${CFLAGS} -c $*.c

.cc.o:
	c++ ${CFLAGS} -c $*.cc


