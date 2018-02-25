// Author: Cesar Lara
//

#include "zlib.h"
#include "fonts.h"

void displayCesarL(int bpos, int cpos, int lpos, int color,
			 const char* name) {
	Rect r;
	r.bot = bpos;
	r.center = cpos;
	r.left = lpos;
	ggprint16(&r, 16, color, name);
}
	

// object definitions
//
struct Pos {
        float x, y, z;
};

struct Shape {
        float width, height;
        float radius;
        Pos center;
};

class Global_menu : public Global {
public:
	Shape box[3];
        Global_menu() {
                xres = 1250;
                yres = 900;
                memset(keys, 0, 65536);
		for(int i=0;i<3;i++) {
			box[i].width = 110;
			box[i].height = 30;
			box[i].center.x = xres/2;
			box[i].center.y = yres/2;
		}
		
        }
};

// Function prototypes
//
void menu_physics(Global_menu &gl);
void menu_render(Global_menu &gl);
void menu_opengl(Global_menu &gl);
int menu_check_keys(XEvent *e, Global_menu &gl);

// Main Menu Function
// 

int Menu() {
        logOpen();
        Global_menu gl;
        X11_wrapper x11(gl);
        menu_opengl(gl);
        srand(time(NULL));
        x11.set_mouse_position(100, 100);
        int done=0;
        while (!done) {
                while (x11.getXPending()) {
                        XEvent e = x11.getXNextEvent();
                        x11.check_resize(&e, gl);
                        done = menu_check_keys(&e, gl);
                }
                menu_physics(gl);
                menu_render(gl);
                x11.swapBuffers();
        }
        cleanup_fonts();
        logClose();
        return 0;
}

// Function definitions
//
void menu_opengl(Global_menu &gl) {
        //OpenGL initialization
        glViewport(0, 0, gl.xres, gl.yres);
        //Initialize matrices
        glMatrixMode(GL_PROJECTION); glLoadIdentity();
        glMatrixMode(GL_MODELVIEW); glLoadIdentity();
        //This sets 2D mode (no perspective)
        glOrtho(0, gl.xres, 0, gl.yres, -1, 1);
        //
        glDisable(GL_LIGHTING);
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_FOG);
        glDisable(GL_CULL_FACE);
        //
        //Clear the screen to black
        glClearColor(0.0, 0.0, 0.0, 1.0);
        //Do this to allow fonts
        glEnable(GL_TEXTURE_2D);
        initialize_fonts();
}
 
int menu_check_keys(XEvent *e, Global_menu &gl) {
        //keyboard input?
        static int shift=0;
        if (e->type != KeyPress && e->type != KeyRelease)
                return 0;
        int key = (XLookupKeysym(&e->xkey, 0) & 0x0000ffff);
        //Log("key: %i\n", key);
        if (e->type == KeyRelease) {
                gl.keys[key]=0;
                if (key == XK_Shift_L || key == XK_Shift_R)
                        shift=0;
                return 0;
        }
        gl.keys[key]=1;
        if (key == XK_Shift_L || key == XK_Shift_R) {
                shift=1;
                return 0;
        }
        (void)shift;
        switch (key) {
                case XK_Escape:
                        return 1;
                case XK_w:
                        break;
                case XK_s:
                        break;
        }
        return 0;
}

void menu_physics(Global_menu &gl) {
}

void menu_render(Global_menu &gl) {
}

