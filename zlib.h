#include <iostream>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <cmath>
using namespace std;
#include <unistd.h>
#include <X11/Xlib.h>
//#include <X11/Xutil.h>
//#include <GL/gl.h>
//#include <GL/glu.h>
#include <X11/keysym.h>
#include <GL/glx.h>
#include "log.h"
#include "fonts.h"

//defined types
typedef float Flt;
typedef float Vec[3];
typedef Flt     Matrix[4][4];

//macros
#define rnd() (((Flt)rand())/(Flt)RAND_MAX)
#define random(a) (rand()%(a))
#define VecZero(v) (v)[0]=0.0,(v)[1]=0.0,(v)[2]=0.0
#define MakeVector(x, y, z, v) (v)[0]=(x),(v)[1]=(y),(v)[2]=(z)
#define VecCopy(a,b) (b)[0]=(a)[0];(b)[1]=(a)[1];(b)[2]=(a)[2]
#define VecDot(a,b)     ((a)[0]*(b)[0]+(a)[1]*(b)[1]+(a)[2]*(b)[2])
#define VecSub(a,b,c) (c)[0]=(a)[0]-(b)[0]; \
                                                (c)[1]=(a)[1]-(b)[1]; \
                                                (c)[2]=(a)[2]-(b)[2]
//constants
const float TIMESLICE = 1.0f;
const float GRAVITY = 0.1;
#define PI 3.141592653589793
#define ALPHA 1
const int MAX_BULLETS = 11;
const Flt MINIMUM_ASTEROID_SIZE = 60.0;
const int XRES = 1250;
const int YRES = 900;

#pragma once
class Global {
public:
        int xres, yres;
        char keys[65536];
        Global() {
                xres = 1250;
                yres = 900;
                memset(keys, 0, 65536);
        }
};

class Ship {
public:
        Vec dir;
        Vec pos;
        Vec vel;
        float angle;
        float color[3];
public:
        Ship() {
                VecZero(dir);
                pos[0] = (Flt)(XRES/2);
                pos[1] = (Flt)(YRES/2);
                pos[2] = 0.0f;
                VecZero(vel);
                angle = 0.0;
                color[0] = color[1] = color[2] = 1.0;
        }
};

class Bullet {
public:
        Vec pos;
        Vec vel;
        float color[3];
        struct timespec time;
public:
        Bullet() { }
};

class Game {
public:
        Ship ship;
        Bullet *barr;
        int nasteroids;
        int nbullets;
        struct timespec bulletTimer;
        struct timespec mouseThrustTimer;
        bool mouseThrustOn;
public:
        Game(Global gl) {
                barr = new Bullet[MAX_BULLETS];
                nasteroids = 0;
                nbullets = 0;
                mouseThrustOn = false;
                clock_gettime(CLOCK_REALTIME, &bulletTimer);
        }
        ~Game() {
                delete [] barr;
        }
};

class X11_wrapper {
private:
        Display *dpy;
        Window win;
        GLXContext glc;
public:
        X11_wrapper(Global &gl) {
                GLint att[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
                //GLint att[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, None };
                XSetWindowAttributes swa;
                setup_screen_res(gl.xres, gl.yres, gl);
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
        void check_resize(XEvent *e, Global &gl) {
                //The ConfigureNotify is sent by the
                //server if the window is resized.
                if (e->type != ConfigureNotify)
                        return;
                XConfigureEvent xce = e->xconfigure;
                if (xce.width != gl.xres || xce.height != gl.yres) {
                        //Window size did change.
                        reshape_window(xce.width, xce.height, gl);
                }
        }
        void reshape_window(int width, int height, Global &gl) {
                //window has been resized.
                setup_screen_res(width, height, gl);
                glViewport(0, 0, (GLint)width, (GLint)height);
                glMatrixMode(GL_PROJECTION); glLoadIdentity();
                glMatrixMode(GL_MODELVIEW); glLoadIdentity();
                glOrtho(0, gl.xres, 0, gl.yres, -1, 1);
                set_title();
        }
        void setup_screen_res(const int w, const int h, Global &gl) {
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
};



