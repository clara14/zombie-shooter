#
# Makefile for asteroids game
#
# Enter one of the following
#
# make
# make all
# make asteroids
# make clean
#
CFLAGS = -I ./include
#LFLAGS = -lrt -lX11 -lGLU -lGL -pthread -lm
LFLAGS = -lrt -lX11 -lGL

all: zombies

zombies: zombies.cpp log.cpp timers.cpp alfredoZ.cpp cesarA.cpp jorgeV.cpp\
	 cesarL.cpp
	g++ $(CFLAGS) zombies.cpp log.cpp timers.cpp alfredoZ.cpp cesarA.cpp \
	jorgeV.cpp cesarL.cpp libggfonts.a -Wall $(LFLAGS) -o zombies \
	/usr/lib/x86_64-linux-gnu/libopenal.so \
	/usr/lib/x86_64-linux-gnu/libalut.so

clean:
	rm -f zombies
	rm -f *.o

