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
	Shape box[4];
	int option;
        Global_menu() {
                xres = 1250;
                yres = 900;
                memset(keys, 0, 65536);
		option = 0;
		for(int i=0;i<4;i++) {
			box[i].width = 200;
			box[i].height = 40;
			box[i].center.x = xres/2;
			box[i].center.y = 500 - (i*120);
		}
		
        }
	void menuUp() {
		option--;
	}
        void menuDown() { 
                option++;
        }

};

class Global_tutorial : public Global {
public:
	int screen;
	Global_tutorial() {
		xres = 1250;
		yres = 900;
		memset(keys, 0, 65536);
		screen = 1;
	}
};

// Function prototypes
//
void menu_render(Global_menu &gl);
void menu_opengl(Global_menu &gl);
int menu_check_keys(XEvent *e, Global_menu &gl);
//-----------------------------------------------------------------------------
// Main Menu Function
//----------------------------------------------------------------------------- 

int menu() {
        logOpen();
        Global_menu gl;
        X11_wrapper x11(gl);
        menu_opengl(gl);
        srand(time(NULL));
        x11.set_mouse_position(100, 100);
        int done=0;
        while (done==0) {
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
        return done;
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
                        return 4;
                case XK_w:
			if (gl.option > 0)
				gl.menuUp();
			break;
		case XK_Up:
			if (gl.option > 0)
				gl.menuUp();
			break;
                case XK_s:
			if (gl.option < 3)
				gl.menuDown();
			break;
		case XK_Down:
			if (gl.option < 3)
				gl.menuDown();
			break;
		case XK_e:
			return (gl.option+1);
		case XK_Return:
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
        ggprint16(&r, 16, 0x00ffffff, "ZOMBIE SHOOTER");
        // Render boxes for main menu options
	//
	float w, h;
	for(int i=0;i<4;i++) {
            Shape *s;
            glColor3ub(20,74,23);
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
	// Draw text for menu options
	//
        for(int i=0;i<4;i++){
            Shape *s;
            s = &gl.box[i];
            r.bot = s->center.y - 12;
            r.left = s->center.x;
            r.center=-20;
            if(i==0)
                ggprint16(&r, 16, 0xffffffff, "%s", "Play Game");
            if(i==1)
                ggprint16(&r, 16, 0xffffffff, "%s", "Tutorial");
            if(i==2)
                ggprint16(&r, 16, 0xffffffff, "%s", "High Scores");
            if(i==3)
                ggprint16(&r, 16, 0xffffffff, "%s", "Exit Game");

        }
	// Render the 'cursor' for choosing the menu options
	//
	Shape *s;
	s = &gl.box[gl.option];
        glColor3ub(255, 255, 255);
        glPushMatrix();
        glTranslatef(880, s->center.y , s->center.z);
        glBegin(GL_TRIANGLES);
                glVertex2f(-55.0f, 0.0f);
                glVertex2f(  0.0f, 30.0f);
                glVertex2f(  0.0f, 0.0f);
                glVertex2f(  0.0f, -30.0f);
                glVertex2f(  0.0f, 0.0f);
                glVertex2f( -55.0f, 0.0f);
        glEnd();
	glBegin(GL_POINTS);
		glVertex2f(0.0f, 0.0f);
	glEnd();
	glPopMatrix();

}
//----------------------------------------------------------------------------
// Tutorial Menu-state
//----------------------------------------------------------------------------
// Function prototypes
//
void tutorial_opengl(Global_tutorial &gl);
int tutorial_keys(XEvent *e, Global_tutorial &gl);
void tutorial_render(Global_tutorial &gl);

int tutorial() {
        Global_tutorial gl;
        X11_wrapper x11(gl);
        tutorial_opengl(gl);
        srand(time(NULL));
        x11.set_mouse_position(100, 100);
        int done=0;
        while (done==0) {
                while (x11.getXPending()) {
                        XEvent e = x11.getXNextEvent();
                        x11.check_resize(&e, gl);
                        done = tutorial_keys(&e, gl);
                }
                tutorial_render(gl);
                x11.swapBuffers();
        }

        cleanup_fonts();
	return 0;
}

//Function definitions
//
void tutorial_opengl(Global_tutorial &gl) {
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

int tutorial_keys(XEvent *e, Global_tutorial &gl) {
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
                        if (gl.screen > 1)
				gl.screen -= 1;
			else
				return 1;
			break;
                case XK_e:
			if (gl.screen < 4)
                               gl.screen += 1;
                        else
                                return 1;
			break;
                case XK_Return:
                        if (gl.screen < 4)
				gl.screen += 1;
			else
				return 1;
			break;
        }
	return 0;
}

void tutorial_render(Global_tutorial &gl) {
	glClear(GL_COLOR_BUFFER_BIT);
	Rect r;
	r.bot = 700;
	r.center = 0;
	r.left = 625;
	if (gl.screen == 1) {
		ggprint16(&r, 16, 0x00ffffff, "Intro");
	}
	if (gl.screen == 2) {
		ggprint16(&r, 16, 0x00ffffff, "Controls");
	}
        if (gl.screen == 3) {
                ggprint16(&r, 16, 0x00ffffff, "Extra Info");
        }
        if (gl.screen == 4) {
                ggprint16(&r, 16, 0x00ffffff, "Extra Info 2");
        }
}
//----------------------------------------------------------------------------
// High Scores Menu-state
//----------------------------------------------------------------------------
int highScores() {
	return 0;
}

