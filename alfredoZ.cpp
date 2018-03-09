// Modified by: Alfredo Zavala
// Date: 2/24/2018

#include "zlib.h"
#include "fonts.h"

extern double timeDiff(struct timespec *start, struct timespec *end);
extern void timeCopy(struct timespec *dest, struct timespace *source);

double unoptimizedFunct();
double optimizedFunct();

void displayAlfredo(int botPos, int leftPos, int centerPos,
	int textColor, const char* textName) {

    Rect r;
    r.bot = botPos;
    r.left= leftPos;
    r.center = centerPos;

    ggprint8b(&r, 16, textColor, textName);
	
    // displaying the optimized and unoptimized functions
    Rect rFunct;
    rFunct.bot = botPos - 25;
    rFunct.left = leftPos + 40;
    rFunct.center = centerPos;

    ggprint16(&rFunct, 16, textColor, "unoptimizedFunct(): %f ",
	    (unoptimizedFunct() * 100));
    
    rFunct.bot = botPos - 50;
    ggprint16(&rFunct, 16, textColor,  "optimizedFunct(): %f ", 
	    (optimizedFunct() * 100));

}



void displayHUD(Global &gl, Game &g) {
    Rect r;
    glClear(GL_COLOR_BUFFER_BIT);
    //
    r.bot = gl.yres - 20;
    r.left = 10;
    r.center = 0;
    ggprint8b(&r, 16, 0x00ff0000, "-------------Player1 HUD-------------");
    ggprint8b(&r, 16, 0x00ffff00, "| \t PlayerName: %s", g.player1.name);
    ggprint8b(&r, 16, 0x00ffff00, "| \t Health: %i", g.player1.health);
    ggprint8b(&r, 16, 0x00ffff00, "| \t Weapon: 9mm Handgun");
    ggprint8b(&r, 16, 0x00ff0000, "-------------------------------------");
}


double unoptimizedFunct() {

    static double amountOfTime = 0.0;
    struct timespec timeStart, timeEnd;

    clock_gettime(CLOCK_REALTIME, &timeStart);

    int a = 100000;
    int b = 100000;

    for( int i = 0 ; i < 100 ; i++ ) {
	a = b / 2;
	b = a / 4;
    }

    clock_gettime(CLOCK_REALTIME, &timeEnd);
    amountOfTime += timeDiff(&timeStart, &timeEnd);

    return amountOfTime;
}


double optimizedFunct() {

    static double amountOfTime = 0.0;
    struct timespec timeStart, timeEnd;

    clock_gettime(CLOCK_REALTIME, &timeStart);

    int a = 100000;
    int b = 100000;

    for( int i = 0 ; i < 100 ; i++ ) {
	a = b >> 1;
	b = a >> 2;
    }

    clock_gettime(CLOCK_REALTIME, &timeEnd);
    amountOfTime += timeDiff(&timeStart, &timeEnd);

    return amountOfTime;
}







