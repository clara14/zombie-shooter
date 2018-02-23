// sprite animations
//name: Jorge Vasquez
#include "zlib.h"
void showFloor(Global &gl, Game &g) {
     glBegin(GL_QUADS);
            glColor3f(0.2, 0.2, 0.2);
            glVertex2i(0,       350);
            glVertex2i(gl.xres, 350);
            glColor3f(0.4, 0.4, 0.4);
            glVertex2i(gl.xres,   0);
            glVertex2i(0,        0);
     glEnd();


}
