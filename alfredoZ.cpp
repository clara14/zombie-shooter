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

void displayGridTest () 
{
	Rect r;
	r.bot = 0;
	r.left = 0;
	r.center = 0;

	// X - values of the grid 
	ggprint8b(&r, 16, 0x00ff0000,"0,0");
	r.left = 100;
	r.bot = 1;
	ggprint8b(&r, 16, 0x00ff0000,"100");
	r.left = 200;
	r.bot = 1;
	ggprint8b(&r, 16, 0x00ff0000,"200");
	r.left = 300;
	r.bot = 1;
	ggprint8b(&r, 16, 0x00ff0000,"300");
	r.left = 400;
	r.bot = 1;
	ggprint8b(&r, 16, 0x00ff0000,"400");
	r.left = 500;
	r.bot = 1;
	ggprint8b(&r, 16, 0x00ff0000,"500");
	r.left = 600;
	r.bot = 1;
	ggprint8b(&r, 16, 0x00ff0000,"600");
	r.left = 700;
	r.bot = 1;
	ggprint8b(&r, 16, 0x00ff0000,"700");
	r.left = 800;
	r.bot = 1;
	ggprint8b(&r, 16, 0x00ff0000,"800");
	r.left = 900;
	r.bot = 1;
	ggprint8b(&r, 16, 0x00ff0000,"900");
	r.left = 1000;
	r.bot = 1;
	ggprint8b(&r, 16, 0x00ff0000,"1000");
	r.left = 1100;
	r.bot = 1;
	ggprint8b(&r, 16, 0x00ff0000,"1100");
	r.left = 1200;
	r.bot = 1;
	ggprint8b(&r, 16, 0x00ff0000,"1200");

	// Y - values of the grid
	r.left = 0;
	r.bot = 100;
	ggprint8b(&r, 16, 0x00ff0000,"100");
	r.left = 0;
	r.bot = 200;
	ggprint8b(&r, 16, 0x00ff0000,"200");
	r.left = 0;
	r.bot = 300;
	ggprint8b(&r, 16, 0x00ff0000,"300");
	r.left = 0;
	r.bot = 400;
	ggprint8b(&r, 16, 0x00ff0000,"400");
	r.left = 0;
	r.bot = 500;
	ggprint8b(&r, 16, 0x00ff0000,"500");
	r.left = 0;
	r.bot = 600;
	ggprint8b(&r, 16, 0x00ff0000,"600");
	r.left = 0;
	r.bot = 700;
	ggprint8b(&r, 16, 0x00ff0000,"700");
	r.left = 0;
	r.bot = 800;
	ggprint8b(&r, 16, 0x00ff0000,"800");
	r.left = 0;
	r.bot = 900;
	ggprint8b(&r, 16, 0x00ff0000,"900");
}





void displyGameScore(Global &gl, Game &g)
{
	Rect r;
	r.bot = gl.yres - 47;
	r.left = gl.xres - 140;
	r.center = 0;
	ggprint8b(&r, 16, 0x00ff0000, "Score: %i", g.score);

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

	glBindTexture(GL_TEXTURE_2D, gl.mainCharacterTexture);

	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER,0.0f);
	glColor4ub(255,255,255,255);

	glBegin(GL_QUADS);

	if( g.player1.facingRight == true) {

		glTexCoord2f(0.0f, 1.0f); glVertex2i(-wid, -wid);
		glTexCoord2f(0.0f, 0.0f); glVertex2i(-wid,  wid);
		glTexCoord2f(1.0f, 0.0f); glVertex2i( wid,  wid);
		glTexCoord2f(1.0f, 1.0f); glVertex2i( wid, -wid);

	} else {

		glTexCoord2f(1.0f, 1.0f); glVertex2i(-wid, -wid);
		glTexCoord2f(1.0f, 0.0f); glVertex2i(-wid,  wid);
		glTexCoord2f(0.0f, 0.0f); glVertex2i( wid,  wid);
		glTexCoord2f(0.0f, 1.0f); glVertex2i( wid, -wid);


	}
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
	glVertex2i(0, gl.yres + 300);

	glTexCoord2f(gl.texture.xc[1], gl.texture.yc[0]); 
	glVertex2i(gl.xres, gl.yres + 300);

	glTexCoord2f(gl.texture.xc[1], gl.texture.yc[1]); 
	glVertex2i(gl.xres, 0);
	glEnd();

}

void physicsScrollingBackground(Global &gl) 
{
	gl.texture.xc[0] += 0.001;
	gl.texture.xc[1] += 0.001;
}

void renderForegroundArena(Global &gl, Game &g) 
{
	glPushMatrix();
	glTranslatef(0, 0, 0);

	glBindTexture(GL_TEXTURE_2D, gl.foregroundTexture);                  
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER,0.0f);
	glColor4ub(255,255,255,255);		

	glBegin(GL_QUADS);                                              
	glTexCoord2f(0.0f, 1.0f); glVertex2i(-140, -90);                     
	glTexCoord2f(0.0f, 0.0f); glVertex2i(-90, gl.yres/2 - 22);                
	glTexCoord2f(1.0f, 0.0f); glVertex2i(gl.xres, gl.yres/2 - 22);           
	glTexCoord2f(1.0f, 1.0f); glVertex2i(gl.xres+140, -100);                
	glEnd();                    

	glPopMatrix();

}

void renderScoreHUD(Global &gl, Game &g) 
{
	glPushMatrix();
	glTranslatef(0, 0, 0);

	glBindTexture(GL_TEXTURE_2D, gl.gameScoreTexture);                  
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER,0.0f);
	glColor4ub(255,255,255,255);		

	glBegin(GL_QUADS);                                
	glTexCoord2f(0.0f, 1.0f); glVertex2i(gl.xres - 200, gl.yres - 100 );                     
	glTexCoord2f(0.0f, 0.0f); glVertex2i(gl.xres - 200, gl.yres );                
	glTexCoord2f(1.0f, 0.0f); glVertex2i(gl.xres -   0, gl.yres );           
	glTexCoord2f(1.0f, 1.0f); glVertex2i(gl.xres -   0, gl.yres - 100);                
	glEnd();                    

	glPopMatrix();
}

void renderGameMenuBackground(Global &gl)
{
	glBindTexture(GL_TEXTURE_2D, gl.menuBgTexture);

	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER,0.0f);
	glColor4ub(255,255,255,255);

	glPushMatrix();

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 1.0f); glVertex2i(0, 0);
	glTexCoord2f(0.0f, 0.0f); glVertex2i(0, gl.yres);
	glTexCoord2f(1.0f, 0.0f); glVertex2i(gl.xres, gl.yres);
	glTexCoord2f(1.0f, 1.0f); glVertex2i(gl.xres, 0);
	glEnd();

	glPopMatrix();

}

void renderPlayerAvatarHUD(Global &gl)
{
	glBindTexture(GL_TEXTURE_2D, gl.playerAvatarTexture);

	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER,0.0f);
	glColor4ub(255,255,255,255);

	glPushMatrix();

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 1.0f); glVertex2i(0		, gl.yres - 100);
	glTexCoord2f(0.0f, 0.0f); glVertex2i(0		, gl.yres);
	glTexCoord2f(1.0f, 0.0f); glVertex2i(100	, gl.yres);
	glTexCoord2f(1.0f, 1.0f); glVertex2i(100	, gl.yres - 100);
	glEnd();

	glPopMatrix();

}

void renderHealthBarHUD(Global &gl)
{
	glBindTexture(GL_TEXTURE_2D, gl.healthBarTexture);

	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER,0.0f);
	glColor4ub(255,255,255,255);

	glPushMatrix();

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 1.0f); glVertex2i(120	, gl.yres - 30);
	glTexCoord2f(0.0f, 0.0f); glVertex2i(120	, gl.yres - 10);
	glTexCoord2f(1.0f, 0.0f); glVertex2i(300	, gl.yres - 10);
	glTexCoord2f(1.0f, 1.0f); glVertex2i(300	, gl.yres - 30);
	glEnd();

	glPopMatrix();

}

void renderHealthBar2HUD(Global &gl)
{
	glBindTexture(GL_TEXTURE_2D, gl.healthBar2Texture);

	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER,0.0f);
	glColor4ub(255,255,255,255);

	glPushMatrix();

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 1.0f); glVertex2i(120	, gl.yres - 30);
	glTexCoord2f(0.0f, 0.0f); glVertex2i(120	, gl.yres - 10);
	glTexCoord2f(1.0f, 0.0f); glVertex2i(300	, gl.yres - 10);
	glTexCoord2f(1.0f, 1.0f); glVertex2i(300	, gl.yres - 30);
	glEnd();

	glPopMatrix();

}

void renderHealthBar3HUD(Global &gl)
{
	glBindTexture(GL_TEXTURE_2D, gl.healthBar3Texture);

	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER,0.0f);
	glColor4ub(255,255,255,255);

	glPushMatrix();

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 1.0f); glVertex2i(120	, gl.yres - 30);
	glTexCoord2f(0.0f, 0.0f); glVertex2i(120	, gl.yres - 10);
	glTexCoord2f(1.0f, 0.0f); glVertex2i(300	, gl.yres - 10);
	glTexCoord2f(1.0f, 1.0f); glVertex2i(300	, gl.yres - 30);
	glEnd();

	glPopMatrix();

}



void renderPlayerWeaponHUD(Global &gl)
{
	glBindTexture(GL_TEXTURE_2D, gl.playerWeaponTexture);

	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER,0.0f);
	glColor4ub(255,255,255,255);

	glPushMatrix();

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 1.0f); glVertex2i(120	, gl.yres - 95);
	glTexCoord2f(0.0f, 0.0f); glVertex2i(120	, gl.yres - 33);
	glTexCoord2f(1.0f, 0.0f); glVertex2i(300	, gl.yres - 33);
	glTexCoord2f(1.0f, 1.0f); glVertex2i(300	, gl.yres - 95);
	glEnd();

	glPopMatrix();

}

void renderGameOver(Global &gl)
{
	glBindTexture(GL_TEXTURE_2D, gl.gameOverTexture);

	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER,0.0f);
	glColor4ub(255,255,255,255);

	glPushMatrix();

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 1.0f); glVertex2i(200		, gl.yres - 300);
	glTexCoord2f(0.0f, 0.0f); glVertex2i(200		, gl.yres);
	glTexCoord2f(1.0f, 0.0f); glVertex2i(gl.xres - 200, gl.yres);
	glTexCoord2f(1.0f, 1.0f); glVertex2i(gl.xres - 200, gl.yres - 300);
	glEnd();

	glPopMatrix();

}


