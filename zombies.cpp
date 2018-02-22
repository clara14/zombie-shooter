//program: zombies.cpp
#include "zlib.h"


//function prototypes
extern void init_opengl(Global &gl, Game &g);
extern int check_keys(XEvent *e, Global &gl, Game &g);
extern void physics(Global &gl, Game &g);
extern void render(Global &gl, Game &g);
extern void normalize2d(Vec v);

//==========================================================================
// M A I N
//==========================================================================
int main()
{
	logOpen();
	Global gl;
	X11_wrapper x11(gl);
	Game g(gl);
	init_opengl(gl, g);
	srand(time(NULL));
	x11.set_mouse_position(100, 100);
	int done=0;
	while (!done) {
		while (x11.getXPending()) {
			XEvent e = x11.getXNextEvent();
			x11.check_resize(&e, gl);
			done = check_keys(&e, gl, g);
		}
		physics(gl,g);
		render(gl,g);
		x11.swapBuffers();
	}
	cleanup_fonts();
	logClose();
	return 0;
}



