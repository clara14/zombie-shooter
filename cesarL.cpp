// Author: Cesar Lara
//

#include "zlib.h"
#include "fonts.h"

extern double timeDiff(struct timespec *start, struct timespec *end);
extern void timeCopy(struct timespec *dest, struct timespec *source);

void displayCesarL(int bpos, int cpos, int lpos, int color,
			 const char* name) {
	static double duration = 0.0;
	struct timespec timeStart, timeEnd;
	clock_gettime(CLOCK_REALTIME, &timeStart);
    	Rect r;
	r.bot = bpos;
	r.center = cpos;
	r.left = lpos;
	ggprint16(&r, 16, color, name);
	clock_gettime(CLOCK_REALTIME, &timeEnd);
	duration += timeDiff(&timeStart, &timeEnd);
	r.bot -= 10;
	ggprint16(&r, 16, color, "function time: %f", duration);
}
	

// object definitions
//
/*
struct Pos {
        float x, y, z;
};

struct Shape {
        float width, height;
        float radius;
        Pos center;
};
*/
class Global_scores : public Global {
public:
    	string name[10];
	int score[10];
    	Global_scores() {
	    	xres = 1250;
		yres = 900;
		memset(keys, 0, 65536);
		ifstream fin;
		fin.open("scores.txt", ios::in);
		if(fin.fail()) {
		    exit(1);
		}
		for(int i=0;i<10;i++) {
		    fin >> name[i];
		    fin >> score[i];
		}
		fin.close();
	}
};
// Function prototypes
//
void menu_render(Global &gl);
int menu_check_keys(XEvent *e, Global &gl);
 
int menu_check_keys(XEvent *e, Global &gl) {
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
			if (gl.menuOption > 0)
				gl.menuOption--;
			break;
		case XK_Up:
			if (gl.menuOption > 0)
				gl.menuOption--;
			break;
                case XK_s:
			if (gl.menuOption < 3)
				gl.menuOption++;
			break;
		case XK_Down:
			if (gl.menuOption < 3)
				gl.menuOption++;
			break;
		case XK_e:
			return (gl.menuOption+1);
		case XK_Return:
			return (gl.menuOption+1);
        }
        return 0;
}

void menu_render(Global &gl) {
	// Implementing a timer
        static double duration = 0.0;
        struct timespec timeStart, timeEnd;
        clock_gettime(CLOCK_REALTIME, &timeStart);
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
	s = &gl.box[gl.menuOption];
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
	// Add time to it
        clock_gettime(CLOCK_REALTIME, &timeEnd);
        duration += timeDiff(&timeStart, &timeEnd);
	r.bot = gl.yres - 50;
	r.left = 100;
	r.center += 20;
	ggprint16(&r, 16, 0x00ff00fff, "render function time: %f", duration);

}
//----------------------------------------------------------------------------
// Tutorial Menu-state
//----------------------------------------------------------------------------
// Function prototypes
//
int tutorial_keys(XEvent *e, Global &gl);
void tutorial_render(Global &gl);

int tutorial_keys(XEvent *e, Global &gl) {
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
                        if (gl.helpScreen > 1)
				gl.helpScreen -= 1;
			else
				return 1;
			break;
                case XK_e:
			if (gl.helpScreen < 4)
                               gl.helpScreen += 1;
                        else
                                return 1;
			break;
                case XK_Return:
                        if (gl.helpScreen < 4)
				gl.helpScreen += 1;
			else
				return 1;
			break;
        }
	return 0;
}

void tutorial_render(Global &gl) {
	glClear(GL_COLOR_BUFFER_BIT);
	Rect r;
	r.bot = 700;
	r.center = 0;
	r.left = 625;
	if (gl.helpScreen == 1) {
		ggprint16(&r, 16, 0x00ffffff, "Intro");
	}
	if (gl.helpScreen == 2) {
		ggprint16(&r, 16, 0x00ffffff, "Controls");
	}
        if (gl.helpScreen == 3) {
                ggprint16(&r, 16, 0x00ffffff, "Extra Info");
        }
        if (gl.helpScreen == 4) {
                ggprint16(&r, 16, 0x00ffffff, "Extra Info 2");
        }
}
//----------------------------------------------------------------------------
// High Scores Menu-state
//----------------------------------------------------------------------------
//Function prototypes
//
int scores_keys(XEvent *e, Global &gl);
void scores_render(Global &gl);


int scores_keys(XEvent *e, Global &gl) {
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
		case XK_q:
			return 1;
        }
        return 0;
}

void scores_render(Global &gl) {
	glClear(GL_COLOR_BUFFER_BIT);
	Rect r;
	r.bot = 800;
	r.left = 625;
	ggprint16(&r, 16, 0x00ffffff, "High Scores");
	r.bot = 725;
	r.left = 312;
	for(int i=0;i<10;i++) {
	    	ggprint16(&r, 16, 0x00ffffff, "NAME: ");
		r.bot -= 50;
	}
	r.bot = 725;
	r.left = 625;
	for(int i=0;i<10;i++) {
		ggprint16(&r, 16, 0x00ffffff, "-");
		r.bot -= 50;
	}
	r.bot = 725;
	r.left = 937;
	for(int i=0; i<10;i++) {
		ggprint16(&r, 16, 0x00ffffff, "SCORE");
		r.bot -= 50;
	}
	
}
