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
	
void showMenu(Global &gl);

void showMenu(Global &gl) {
	// reset tutorial screen
	if (gl.helpScreen != 1)
		gl.helpScreen = 1;
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
void showTutorial(Global &gl);

void showTutorial(Global &gl) {
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

void showScores(Global &g);

void showScores(Global &gl) {
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
