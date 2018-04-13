// Modified by: Alfredo Zavala
// Date: 2/24/2018

#include "zlib.h"
#include "fonts.h"

extern unsigned char *buildAlphaData(Image *img);

Image mainCharacter = "./images/spartan.png";
Image mainRoad = "./images/darkRoad.png";

Image img[3] = {
	"./images/8bit_bullet_bill_02.png",
	"./images/spartan.png",
	"./images/rocket_ship.png"
};

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
	ggprint8b(&r, 16, 0x00ffff00, "| \t Score:  %i", g.score);
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


void initialize_mainRoad(Global &gl)
{
	// main road 
	glGenTextures(1, &gl.mainRoadTexture);

	int w = mainRoad.width;
	int h = mainRoad.height;

	glBindTexture(GL_TEXTURE_2D, gl.mainRoadTexture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, w, h, 0, 
			GL_RGB, GL_UNSIGNED_BYTE, mainRoad.data);

	// silhouette

	glBindTexture(GL_TEXTURE_2D, gl.silhouetteTexture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	unsigned char *silhouetteData = buildAlphaData(&mainRoad);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, 
			GL_RGBA, GL_UNSIGNED_BYTE, silhouetteData);

	free(silhouetteData);


}

void drawMainRoad(Global &gl, Game &g)
{
//	float wid = 45.0f;

//	glPushMatrix();

//	glTranslatef(g.player1.pos[0], g.player1.pos[1], 
//			g.player1.pos[2]);


	glBindTexture(GL_TEXTURE_2D, gl.mainRoadTexture);

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


}

void initialize_player1(Global &gl) 
{
	// main character 
	glGenTextures(1, &gl.mainCharacterTexture);

	int w = mainCharacter.width;
	int h = mainCharacter.height;

	glBindTexture(GL_TEXTURE_2D, gl.mainCharacterTexture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, w, h, 0, 
			GL_RGB, GL_UNSIGNED_BYTE, mainCharacter.data);

	// silhouette

	glBindTexture(GL_TEXTURE_2D, gl.silhouetteTexture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	unsigned char *silhouetteData = buildAlphaData(&mainCharacter);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, 
			GL_RGBA, GL_UNSIGNED_BYTE, silhouetteData);

	free(silhouetteData);
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




