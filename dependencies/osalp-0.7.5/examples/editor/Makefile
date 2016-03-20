CFLAGS = -g -Wall \
		-I/usr/local/include/g++ \
		-I/usr/include/g++ \
		-I/usr/include/X11 \
		-I/usr/X11R6/include \
		-I/usr/local/include \
		-I/usr/local/include/aflib \
		-I.

OBJECTS = \
	graph_widget.o \
	connect_widget.o \
	main.o \
	gui.o \
	plot_data.o \
	plot.o

SOURCES = ${OBJECTS:.o=.cc}

all: editor

editor: ${OBJECTS}
	g++ -g -rdynamic -o editor ${OBJECTS} \
	-L/usr/local/lib -laflib \
	-L/usr/X11/lib -L/usr/X11R6/lib -lXm -lXt -lXext -lX11 -ldl -lm -lc

clean:
	rm -rf ${OBJECTS} editor

.cc.o:
	g++ ${CFLAGS} -c $*.cc

