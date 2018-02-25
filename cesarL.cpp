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
	int option;
        Global_menu() {
                xres = 1250;
                yres = 900;
                memset(keys, 0, 65536);
		option = 0;
		for(int i=0;i<3;i++) {
			box[i].width = 200;
			box[i].height = 40;
			box[i].center.x = xres/2;
			box[i].center.y = 500 - (i*160);
		}
		
        }
	void menuUp() {
		option--;
	}
        void menuDown() { 
                option++;
        }

};

// Function prototypes
//
void menu_render(Global_menu &gl);
void menu_opengl(Global_menu &gl);
int menu_check_keys(XEvent *e, Global_menu &gl);
extern void game();
// Main Menu Function
// 

void Menu() {
        logOpen();
        Global_menu gl;
        X11_wrapper x11(gl);
        menu_opengl(gl);
        srand(time(NULL));
        x11.set_mouse_position(100, 100);
        int done=0;
        while (done != 3) {
                while (x11.getXPending()) {
                        XEvent e = x11.getXNextEvent();
                        x11.check_resize(&e, gl);
                        done = menu_check_keys(&e, gl);
                }
                menu_render(gl);
                x11.swapBuffers();
        }

        cleanup_fonts();
        logClose();
        return;
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
                        return 2;
                case XK_w:
			if (gl.option > 0)
				gl.menuUp();
			break;
                case XK_s:
			if (gl.option < 2)
				gl.menuDown();
			break;
		case XK_e:
			cout << "enter was pressed\n";
			return (gl.option+1);
        }
        return 0;
}

void menu_render(Global_menu &gl) {
	// Render game title
	//
	glClear(GL_COLOR_BUFFER_BIT);
        Rect r;
        r.bot = 700;
        r.center = 450;
        r.left = 625;
        ggprint16(&r, 16, 0x00ffffff, "ZOMBIE SHOOTER: %i", gl.option);
        // Render boxes for main menu options
	//
	float w, h;
	for(int i=0;i<3;i++) {
            Shape *s;
            glColor3ub(30,84,33);
            s = &gl.box[i];
            glPushMatrix();
            glTranslatef(s->center.x, s->center.y, s->center.z);
            w = s->width;
            h = s->height;
            glBegin(GL_QUADS);
                glVertex2i(-w, -h);
                glVertex2i(-w,  h);
                glVertex2i( w,  h);
                glVertex2i( w, -h);
            glEnd();
            glPopMatrix();
	}	
	// Render the 'cursor' for choosing the menu options
	//
	Shape *s;
	s = &gl.box[gl.option];
        glColor3ub(255, 255, 255);
        glPushMatrix();
        glTranslatef(865, s->center.y , s->center.z);
        glBegin(GL_TRIANGLES);
                glVertex2f(-40.0f, 0.0f);
                glVertex2f(  0.0f, 40.0f);
                glVertex2f(  0.0f, 0.0f);
                glVertex2f(  0.0f, -40.0f);
                glVertex2f(  0.0f, 0.0f);
                glVertex2f( -40.0f, 0.0f);
        glEnd();
	glBegin(GL_POINTS);
		glVertex2f(0.0f, 0.0f);
	glEnd();
	glPopMatrix();

}

