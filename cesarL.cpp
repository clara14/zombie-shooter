// Menu
//
//
// State

#include "zlib.h"

// object definitions
//

struct Vec {
        float x, y, z;
};

struct Shape {
        float width, height;
        float radius;
        Vec center;
};

class Global_menu {
public:
        int xres, yres;
        char keys[65536];
	Shape box[MAX_BOXES];
        Global() {
                xres = 1250;
                yres = 900;
                memset(keys, 0, 65536);
		for(int i=0;i<MAX_BOXES;i++) {
			box[i].width = 110;
			box[i].height = 30;
			box[i].center.x = xres/2;
			box[i].center.y = yres/2;
		}
		
        }
};

class X11_menu {
private:
        Display *dpy;
        Window win;
        GLXContext glc;
public:
        X11_wrapper(Global_menu &gl) {
                GLint att[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
                //GLint att[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, None };
                XSetWindowAttributes swa;
                setup_screen_res(gl.xres, gl.yres);
                dpy = XOpenDisplay(NULL);
                if (dpy == NULL) {
                        std::cout << "\n\tcannot connect to X server" << std::endl;
                        exit(EXIT_FAILURE);
                }
                Window root = DefaultRootWindow(dpy);
                XVisualInfo *vi = glXChooseVisual(dpy, 0, att);
                if (vi == NULL) {
                        std::cout << "\n\tno appropriate visual found\n" << std::endl;
                        exit(EXIT_FAILURE);
                }
                Colormap cmap = XCreateColormap(dpy, root, vi->visual, AllocNone);
                swa.colormap = cmap;
                swa.event_mask = ExposureMask | KeyPressMask | KeyReleaseMask |
                        PointerMotionMask | MotionNotify | ButtonPress | ButtonRelease |
                        StructureNotifyMask | SubstructureNotifyMask;
                win = XCreateWindow(dpy, root, 0, 0, gl.xres, gl.yres, 0,
                                vi->depth, InputOutput, vi->visual,
                                CWColormap | CWEventMask, &swa);
                set_title();
                glc = glXCreateContext(dpy, vi, NULL, GL_TRUE);
                glXMakeCurrent(dpy, win, glc);
                show_mouse_cursor(0);
        }
        ~X11_wrapper() {
                XDestroyWindow(dpy, win);
                XCloseDisplay(dpy);
        }
        void set_title() {
                //Set the window title bar.
                XMapWindow(dpy, win);
                XStoreName(dpy, win, "Asteroids template");
        }
        void check_resize(XEvent *ei, Global_menu gl) {
                //The ConfigureNotify is sent by the
                //server if the window is resized.
                if (e->type != ConfigureNotify)
                        return;
                XConfigureEvent xce = e->xconfigure;
                if (xce.width != gl.xres || xce.height != gl.yres) {
                        //Window size did change.
                        reshape_window(xce.width, xce.height);
                }
        }
        void reshape_window(int width, int height, Global_menu gl) {
                //window has been resized.
                setup_screen_res(width, height);
                glViewport(0, 0, (GLint)width, (GLint)height);
                glMatrixMode(GL_PROJECTION); glLoadIdentity();
                glMatrixMode(GL_MODELVIEW); glLoadIdentity();
                glOrtho(0, gl.xres, 0, gl.yres, -1, 1);
                set_title();
        }
        void setup_screen_res(const int w, const int h, Global_menu gl) {
                gl.xres = w;
                gl.yres = h;
        }
        void swapBuffers() {
                glXSwapBuffers(dpy, win);
        }
        bool getXPending() {
                return XPending(dpy);
        }
        XEvent getXNextEvent() {
                XEvent e;
                XNextEvent(dpy, &e);
                return e;
        }
        void set_mouse_position(int x, int y) {
                XWarpPointer(dpy, None, win, 0, 0, 0, 0, x, y);
        }
        void show_mouse_cursor(const int onoff) {
                if (onoff) {
                        //this removes our own blank cursor.
                        XUndefineCursor(dpy, win);
                        return;
                }
                //vars to make blank cursor
                Pixmap blank;
                XColor dummy;
                char data[1] = {0};
                Cursor cursor;
                //make a blank cursor
                blank = XCreateBitmapFromData (dpy, win, data, 1, 1);
                if (blank == None)
                        std::cout << "error: out of memory." << std::endl;
                cursor = XCreatePixmapCursor(dpy, blank, blank, &dummy, &dummy, 0, 0);
                XFreePixmap(dpy, blank);
                //this makes you the cursor. then set it using this function
                XDefineCursor(dpy, win, cursor);
                //after you do not need the cursor anymore use this function.
                //it will undo the last change done by XDefineCursor
                //(thus do only use ONCE XDefineCursor and then XUndefineCursor):
        }
} 

// Main Menu Function
// 

int Menu()
{
        logOpen();
        Global_menu gl;
        X11_menu x11(gl);
        menu_opengl(gl);
        srand(time(NULL));
        x11.set_mouse_position(100, 100);
        int done=0;
        while (!done) {
                while (x11.getXPending()) {
                        XEvent e = x11.getXNextEvent();
                        x11.check_resize(&e, gl);
                        done = check_keys(&e, gl);
                }
                menu_physics(gl);
                menu_render(gl);
                x11.swapBuffers();
        }
        cleanup_fonts();
        logClose();
        return 0;
}


// Function prototypes
//
void menu_physics(Global_menu &gl);
void menu_render(Global_menu &gl);
void menu_opengl(Global_menu &gl);
int menu_check_keys(XEvent *e, Global_menu &gl);

// Function definitions
//
void menu_opengl(Global_menu &gl)
{
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
 

