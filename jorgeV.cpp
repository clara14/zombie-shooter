// sprite animations
//name: Jorge Vasquez
#include "zlib.h"
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
     Rect r;

     r.bot = 490;
     r.left = 50;
     r.center = 80;
     ggprint8b(&r, 16, 0x00ffff00, "Jorge Vasquez");
}
