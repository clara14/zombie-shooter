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

zombies: zombies.cpp log.cpp timers.cpp cesarA.cpp alfredoZ.cpp
	g++ $(CFLAGS) zombies.cpp log.cpp timers.cpp cesarA.cpp alfredoZ.cpp \
	libggfonts.a -Wall $(LFLAGS) -o zombies

clean:
	rm -f zombies
	rm -f *.o

