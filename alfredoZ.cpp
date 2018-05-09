// Author: Alfredo Zavala
// Last Modified on: 4/20/2018

#include "zlib.h"
#include "fonts.h"

extern double timeDiff(struct timespec *start, struct timespec *end);
extern void timeCopy(struct timespec *dest, struct timespace *source);

double unoptimizedFunct();
double optimizedFunct();

void displayAlfredo(int botPos, int leftPos, int centerPos,
		int textColor, const char* textName) 
{

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

void displayHUD(Global &gl, Game &g) 
{
	Rect r;
	r.bot = gl.yres - 20;
	r.left = 10;
	r.center = 0;
	ggprint8b(&r, 16, 0x00ff0000, "-------------Player1 HUD-------------");
	ggprint8b(&r, 16, 0x00ffff00, "| \t PlayerName: %s", g.player1.name);
	ggprint8b(&r, 16, 0x00ffff00, "| \t Health: %i", g.player1.health);
	ggprint8b(&r, 16, 0x00ffff00, "| \t Weapon: 9mm Handgun");
	ggprint8b(&r, 16, 0x00ffff00, "| \t Score:  %i", g.score);
	ggprint8b(&r, 16, 0x00ff0000, "-------------------------------------");
}


double unoptimizedFunct() 
{
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


double optimizedFunct() 
{
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

void drawMainRoad(Global &gl, Game &g)
{
	// Binding the texture 
	glBindTexture(GL_TEXTURE_2D, gl.mainRoadTexture);
	// Enabling alpha channel 
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER,0.0f);
	glColor4ub(255,255,255,255);

	glPushMatrix();

	// Drawing the main road
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 1.0f); glVertex2i(0, 0);
	glTexCoord2f(0.0f, 0.0f); glVertex2i(0, gl.yres);
	glTexCoord2f(1.0f, 0.0f); glVertex2i(gl.xres, gl.yres);
	glTexCoord2f(1.0f, 1.0f); glVertex2i(gl.xres, 0);
	glEnd();

	glPopMatrix();

}

void draw_player1(Global &gl, Game &g)
{
	float wid = 45.0f;

	glColor3fv(g.player1.color);
	glPushMatrix();

	glTranslatef(g.player1.pos[0], g.player1.pos[1], 
			g.player1.pos[2]);

	glBindTexture(GL_TEXTURE_2D, gl.silhouetteTexture);

	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER,0.0f);
	glColor4ub(255,255,255,255);

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 1.0f); glVertex2i(-wid, -wid);
	glTexCoord2f(0.0f, 0.0f); glVertex2i(-wid,  wid);
	glTexCoord2f(1.0f, 0.0f); glVertex2i( wid,  wid);
	glTexCoord2f(1.0f, 1.0f); glVertex2i( wid, -wid);
	glEnd();

	glPopMatrix();

}

void renderMovingBackground(Global &gl, Game &g) 
{
	glColor3f(1.0, 1.0, 1.0);

	glBindTexture(GL_TEXTURE_2D, gl.texture.backTexture);
	glBegin(GL_QUADS);
	
	glTexCoord2f(gl.texture.xc[0], gl.texture.yc[1]); 
	glVertex2i(0, 0);
	
	glTexCoord2f(gl.texture.xc[0], gl.texture.yc[0]); 
	glVertex2i(0, gl.yres);
	
	glTexCoord2f(gl.texture.xc[1], gl.texture.yc[0]); 
	glVertex2i(gl.xres, gl.yres);
	
	glTexCoord2f(gl.texture.xc[1], gl.texture.yc[1]); 
	glVertex2i(gl.xres, 0);
	glEnd();

}



