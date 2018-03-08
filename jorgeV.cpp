// sprite animations
//name: Jorge Vasquez
//
#include "zlib.h"
#include <time.h>

extern double timeDiff(struct timespec *start, struct timespec *end);

void showFloor(Global &gl, Game &g) 
{
     glBegin(GL_QUADS);
            glColor3f(0.2, 0.2, 0.2);
            glVertex2i(0,       350);
            glVertex2i(gl.xres, 350);
            glColor3f(0.4, 0.4, 0.4);
            glVertex2i(gl.xres,   0);
            glVertex2i(0,        0);
     glEnd();
}
void show_jorge(Global &gl, Game &g)
{
     struct timespec timeStart, timeEnd;
     static double time;
     clock_gettime(CLOCK_REALTIME, &timeStart);

     Rect r;

     r.bot = 790;
     r.left = 85;
     r.center = 180;
     ggprint8b(&r, 16, 0x00ffff00, "Jorge Vasquez");
  
    
     clock_gettime(CLOCK_REALTIME, &timeEnd);
     time += timeDiff(&timeStart, &timeEnd);

     ggprint8b(&r, 16, 0x00ffff00,"function time:%lf", time);
}
void test1_jorge(Global &gl, Game &g)
{
    struct timespec timeStart, timeEnd;
    static double time;
    clock_gettime(CLOCK_REALTIME, &timeStart);

    int a = 60;
    int b = 5;
    
    b = a & 5;
    a = b & 2;
    a = b & 4;
    b = a & 17;

    

    clock_gettime(CLOCK_REALTIME, &timeEnd);
    time += timeDiff(&timeStart, &timeEnd);

    Rect r;

    r.bot = 750;
    r.left = 85;
    r.center = 180;
    ggprint8b(&r, 16, 0x00ffff00, "function 2 time:%lf", time);
}










