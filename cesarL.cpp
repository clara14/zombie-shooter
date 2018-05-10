// Author: Cesar Lara
// Date last modified: 4/21/18


#include "zlib.h"
#include "fonts.h"

extern double timeDiff(struct timespec *start, struct timespec *end);
extern void timeCopy(struct timespec *dest, struct timespec *source);
extern void sortScores(Game &g);
extern void saveScores(Game &g);
extern bool checkScore(Game &g);
extern void endGame(Global &gl, Game &g);
extern void deleteZombies(Game &g);

// Function - alfredoZ.cpp
extern void renderGameMenuBackground(Global &gl);
extern void renderGameOver(Global &gl);

#define MENU 1
#define GAME 2
#define HELP 3
#define SCORES 4
#define END 5
#define NEWSCORE 6

void displayCesarL(int bpos, int cpos, int lpos, int color, const char* name)
{
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

// key cases for check_keys function in main file
int xk_escape(Global &gl, Game &g)
{
	int ret = 0;
	if (gl.menuState == MENU) {
		saveScores(g);
		ret = 1;
	} else if (gl.menuState == GAME) {
		gl.menuState = MENU;
		deleteZombies(g);
	} else if (gl.menuState == HELP) {
		if (gl.helpScreen == 1)
			gl.menuState = MENU;
	} else if (gl.menuState == SCORES) {
		gl.menuState = MENU;
	}
	return ret;
}
int xk_e(Global &gl, Game &g)
{
	int ret = 0;
	if (gl.menuState == MENU) {
		if (gl.menuOption == 0) {
			struct timespec pt;
			clock_gettime(CLOCK_REALTIME, &pt);
			timeCopy(&g.player1.time, &pt);
			timeCopy(&g.time, &pt);
			g.wave = 0;
			gl.menuState = GAME;
		} else if (gl.menuOption == 1) {
			gl.menuState = HELP;
		} else if (gl.menuOption == 2) {
			gl.menuState = SCORES;
		} else if (gl.menuOption == 3) {
			saveScores(g);
			ret = 1;
		}
	} else if (gl.menuState == HELP) {
		if (gl.helpScreen < 2)
			gl.helpScreen += 1;
		else {
			gl.menuState = MENU;
		}
	} else if (gl.menuState == END) {
		if (checkScore(g)) {
			gl.menuState = NEWSCORE;
			endGame(gl, g);
		} else {
			gl.menuState = MENU;
			endGame(gl, g);
		}
	} else if (gl.menuState == NEWSCORE) {
		HighScore *s = &g.scoreUI;
		// char in list array is selected and entered into name array
		if (s->cursorPos != 5) {
			char c = s->charList[s->charPos];
			s->playerName[s->cursorPos++] = c;
			s->charPos = 10;
		} else {
			gl.menuState = 1;
			s->cursorPos = 0;
			string name(s->playerName);
			g.topPlayers[9] = name;
			g.topScores[9] = g.score;
			sortScores(g);
			g.score = 0;
		}
	}
	return ret;
}

void xk_w(Global &gl, Game &g)
{
	if (gl.menuState == MENU) {
		if (gl.menuOption > 0)
			gl.menuOption--;
	} else if (gl.menuState == NEWSCORE) {
		HighScore *s = &g.scoreUI;
		if (s->cursorPos != 5) {
			if (s->charPos > 0)
				s->charPos--;
		}
	}
}

void xk_s(Global &gl, Game &g)
{
	if (gl.menuState == MENU) {
		if (gl.menuOption < 3)
			gl.menuOption++;
	} else if (gl.menuState == NEWSCORE) {
		HighScore *s = &g.scoreUI;
		if (s->cursorPos != 5) {
			if (s->charPos < 36)
				s->charPos++;
		}
	}
}

void xk_q(Global &gl, Game &g)
{
	if (gl.menuState == HELP) {
		if (gl.helpScreen > 1)
			gl.helpScreen -= 1;
		else
			gl.menuState = MENU;
	} else if (gl.menuState == SCORES) {
		gl.menuState = MENU;
	} else if (gl.menuState == NEWSCORE) {
		HighScore *s = &g.scoreUI;
		if (s->cursorPos > 0)
			s->cursorPos--;
	}
}

//High-score functions
//
bool checkScore(Game &g)
{
	bool check = false;
	if (g.topScores[9] < g.score)
		check = true;
	return check;
}

void sortScores(Game &g)
{
	for(int oloop=0;oloop<10;oloop++) {
		for(int iloop=0;iloop<9;iloop++) {
			if(g.topScores[iloop] < g.topScores[iloop+1]) {
				swap(g.topScores[iloop],g.topScores[iloop+1]);
				swap(g.topPlayers[iloop],g.topPlayers[iloop+1]);
			}
		}
	}
}

void saveScores(Game &g)
{
    	ofstream fout;
	fout.open("scores.txt");
	if (fout.fail()) {
	    	cout << "scores.txt failed to open\n";
		exit(1);
	}
	for (int i=0;i<10;i++) {
		fout << g.topPlayers[i] << "\t" << g.topScores[i] << endl;
	}
}

// Player functions
//
void updateTime(Game &g)
{
	struct timespec pt;
	clock_gettime(CLOCK_REALTIME, &pt);
	g.player1.ptime = timeDiff(&g.player1.time, &pt);
}

int checkQuad(Global &gl, Game &g)
{
	float xpos = g.player1.pos[0];
	float ypos = g.player1.pos[1];
	int quadrant = 3;
	if (xpos >= (gl.xres / 2)) {
		if (ypos <= 340 && ypos > 170)
			quadrant = 1;
		else if (ypos <= 170)
			quadrant = 4;
	} else {
		if (ypos > 170)
			quadrant = 2;
	}
	return quadrant;
}
// Zombie functions
//
extern void listZombies(Game &g);
extern void createZombie(Game &g, int pquad, int n);

void spawnWave(Global &gl, Game &g)
{
	struct timespec gt;
	clock_gettime(CLOCK_REALTIME, &gt);
	g.gtime = timeDiff(&g.time, &gt);
	int t = g.gtime;
	if (t >= 5) {
		int n = (rand() % (g.wave+1)) + 1;
		createZombie(g, checkQuad(gl, g), n);
		listZombies(g);
		clock_gettime(CLOCK_REALTIME, &g.time);
		g.wave++;
		if (g.wave > 5) {
			g.wave = 3;
		} else {
			g.wave++;
		}
	}
}

void createZombie(Game &g, int pquad, int n)
{
	int quad = 4;
	int spawnx, spawny;
	while (quad == pquad) {
		quad = rand() % 4 + 1;
	}
	for(int i=0;i<n;i++) {
	    	if (quad == 1) {
	    		spawnx = (rand() % 80) + 1160;
	    		spawny = (rand() % 30) + 310;
		}
		if (quad == 2) {
    			spawnx = (rand() % 30) + 1;
    			spawny = (rand() % 30) + 310;
		}
		if (quad == 3) {
    			spawnx = (rand() % 30) + 1;
    			spawny = (rand() % 30) + 1;
		}
		if (quad == 4) {
    			spawnx = (rand() % 80) + 1160;
    			spawny = (rand() % 30) + 1;
		}			
		Zombie *z = new Zombie;
		z->pos[0] = spawnx;
		z->pos[1] = spawny;
		z->pos[2] = 0.0f;
		z->next = g.znext;
		if (g.znext != NULL)
			g.znext->prev = z;
		g.znext = z;
		g.nzombies++;
		if (quad != 4)
			quad++;
		else
			quad = 1;
		while (quad == pquad) {
		    	quad = rand() % 4 + 1;
		}
	}
}
// prints the linked list to make sure pointers were assigned correctly
// during createZombie
void listZombies(Game &g)
{
	Zombie *z = g.znext;
	int n = 1;
	while (z) {
		cout << n << "th zombie address: " << z << endl;
		cout << "\tprev: " << z->prev << endl;
		cout << "\tnext: " << z->next << endl;
		z = z->next;
		n++;
	}
}

void drawZombies(Global &gl, Game &g) 
{
	Zombie *z = g.znext;
	int w, h;
	while (z) {
		w = z->width;
		h = z->height;
		glColor3ub(20,74,23);
		glPushMatrix();
		glTranslatef(z->pos[0], z->pos[1], z->pos[2]);
		glBindTexture(GL_TEXTURE_2D, gl.zombieTexture);
		glEnable(GL_ALPHA_TEST);
		glAlphaFunc(GL_GREATER,0.0f);
		glColor4ub(255,255,255,255);
		glBegin(GL_QUADS);
			glTexCoord2f(0.0f, 1.0f); 
			glVertex2i(-w, -h);
			glTexCoord2f(0.0f, 0.0f); 
			glVertex2i(-w, h);
			glTexCoord2f(1.0f, 0.0f); 
			glVertex2i(w, h);
			glTexCoord2f(1.0f, 1.0f); 
			glVertex2i(w, -h);
		glEnd();
		glPopMatrix();
		z = z->next;
	}
}

// removes one zombie
void removeZombie(Game &g, Zombie *z)
{
	cout << "beginning of removeZombie()\n";
	if (g.nzombies == 0)
		return;
	Zombie *temp;
	Zombie *prev;
	if (g.znext == z) {
		temp = g.znext->next;
		delete g.znext;
		g.nzombies--;
		g.znext = temp;
	} else {
		temp = g.znext;
		while (temp != NULL && temp != z) {
			prev = temp;
			temp = temp->next;
		}
		if (temp) {
			prev->next = temp->next;
			delete temp;
			g.nzombies--;
		}
	}
	cout << "end of removeZombie()\n";
}
// traverses list and deletes all zombies
void deleteZombies(Game &g)
{
	Zombie *temp1;
	Zombie *temp2;
	temp1 = g.znext;
	while (temp1!=NULL) {
		temp2 = temp1->next;
		delete temp1;
		temp1 = temp2;
	}
	g.nzombies = 0;
	g.znext = NULL;
}

void showMenu(Global &gl)
{
	// reset tutorial screen
	if (gl.helpScreen != 1)
		gl.helpScreen = 1;
	// Implementing a timer
#ifdef PROFILING_ON
	static double duration = 0.0;
	struct timespec timeStart, timeEnd;
	clock_gettime(CLOCK_REALTIME, &timeStart);
#endif
	// Render game title
	glClear(GL_COLOR_BUFFER_BIT);
	
	renderGameMenuBackground(gl);
	
	Rect r;
	r.bot = 700;
	r.center = 450;
	r.left = 625;
	ggprint16(&r, 16, 0x00ffffff, " ");
	// Render boxes for main menu options
	for(int i=0;i<4;i++) {
		glColor3ub(20,74,23);
		glPushMatrix();
		glTranslatef(625, (500 - (i*120)), 0);
		glBegin(GL_QUADS);
			glVertex2i(-200, -40);
			glVertex2i(-200, 40);
			glVertex2i(200, 40);
			glVertex2i(200, -40);
		glEnd();
		glPopMatrix();
	}	
	// Draw text for menu options
	for(int i=0;i<4;i++) {
		r.bot = (500 - (i*120)) - 12;
		r.left = gl.xres / 2;
		if(i == 0)
			ggprint16(&r, 16, 0xffffffff, "%s", "Play Game");
		if(i == 1)
			ggprint16(&r, 16, 0xffffffff, "%s", "Tutorial");
		if(i == 2)
			ggprint16(&r, 16, 0xffffffff, "%s", "High Scores");
		if(i == 3)
			ggprint16(&r, 16, 0xffffffff, "%s", "Exit Game");
	}
	// Render the 'cursor' for choosing the menu options
	glColor3ub(255, 255, 255);
	glPushMatrix();
	int position = 500 - (gl.menuOption * 120);
	glTranslatef(880, position , 0);
	glBegin(GL_TRIANGLES);
		glVertex2f(-55.0f, 0.0f);
		glVertex2f(  0.0f, 30.0f);
		glVertex2f(  -8.0f, 0.0f);
	glEnd();
	glColor3ub(210, 210, 210);
	glBegin(GL_TRIANGLES);
		glVertex2f(  0.0f, -30.0f);
		glVertex2f( -8.0f, 0.0f);
		glVertex2f( -55.0f, 0.0f);
	glEnd();
	glPopMatrix();
	// Show brief instructions for moving through options
	r.bot = 20;
	r.left = 130;
	ggprint16(&r, 16, 0x00ffffff, "W / S - Menu Up / Down");
	r.left = gl.xres / 2;
	r.bot = 20;
	const char* option;
	if (gl.menuOption == 0) {
		option = "Play Game";
	} else if (gl.menuOption == 1) {
		option = "View Tutorial";
	} else if (gl.menuOption == 2) {
		option = "View High Scores";
	} else if (gl.menuOption == 3) {
		option = "Exit Game";
	}
	ggprint16(&r, 16, 0x00ffffff, "E - %s", option);
	// Add time to it
#ifdef PROFILING_ON
	clock_gettime(CLOCK_REALTIME, &timeEnd);
	duration += timeDiff(&timeStart, &timeEnd);
	r.bot = gl.yres - 50;
	r.left = 100;
	r.center += 20;
	ggprint16(&r, 16, 0x00ff00fff, "render function time: %f", duration);
#endif
}

void showTutorial(Global &gl)
{
	glClear(GL_COLOR_BUFFER_BIT);
	// heading underline
	glColor3ub(255,255,255);
	glPushMatrix();
	glTranslatef(625, 770, 0);
	glBegin(GL_QUADS);
		glVertex2i(-120, -2);
		glVertex2i(-120, 2);
		glVertex2i(120, 2);
		glVertex2i(120, -2);
	glEnd();
	glPopMatrix();
	Rect r;
	r.bot = 800;
	r.left = 590;
	if (gl.helpScreen == 1) {
		ggprint16(&r, 16, 0x00ffffff, "Objective");
		r.bot = 700;
		r.left = (gl.xres / 2)-200;
		const char* text;
		text = "A zombie apocalypse has taken over the world!";
		ggprint16(&r, 16, 0x00ffffff, "%s", text);
		text = "Fight your way through hoardes of zombies";
		r.bot = 650;
		r.left = 440;
		ggprint16(&r, 16, 0x00ffffff, "%s", text);
		text = "Survive as long as you can";
		r.bot = 600;
		r.left = 500;
		ggprint16(&r, 16, 0x00ffffff, "%s", text);
	}
	if (gl.helpScreen == 2) {
		ggprint16(&r, 16, 0x00ffffff, "Controls");
		r.bot = 700;
		r.left = (gl.xres/2)- 60;
		ggprint16(&r, 16, 0x00ffffff, "W - Move Up");
		r.bot = 650;
		ggprint16(&r, 16, 0x00ffffff, "S - Move Down");
		r.bot = 600;
		ggprint16(&r, 16, 0x00ffffff, "A - Move Left");
		r.bot = 550;
		ggprint16(&r, 16, 0x00ffffff, "D - Move Right");
		r.bot = 500;
		ggprint16(&r, 16, 0x00ffffff, "Space Bar - Shoot");
	}
	if (gl.helpScreen == 1) {
	    	r.left = 105;
		r.bot = 20;
		ggprint16(&r, 16, 0x00ffffff, "Q - Return to Menu");
	} else {
	    	r.left = 105;
		r.bot = 20;
		ggprint16(&r, 16, 0x00ffffff, "Q - Go Back");
	}
	Rect r2;
	if (gl.helpScreen == 2) {
	    	r2.left = 1145;
		r2.bot = 20;
		ggprint16(&r2, 16, 0x00ffffff, "E - Return to Menu");
	} else {
	    	r2.left = 1145;
		r2.bot = 20;
		ggprint16(&r2, 16, 0x00ffffff, "E - Next");
	}
}

void showScores(Global &gl, Game &g)
{
	glClear(GL_COLOR_BUFFER_BIT);
	Rect r;
	r.bot = 800;
	r.left = 625;
	ggprint16(&r, 16, 0x00ffffff, "High Scores");
	r.bot = 725;
	r.left = 312;
	for(int i=0;i<10;i++) {
		r.left = 312;
		if (g.topPlayers[i] != "null") {
		    	const char* c = g.topPlayers[i].c_str();
			ggprint16(&r, 16, 0x00ffffff, "%s", c);
		}
		r.bot -= 50;
	}
	r.bot = 725;
	for(int i=0;i<10;i++) {
		r.left = 625;
		ggprint16(&r, 16, 0x00ffffff, "-");
		r.bot -= 50;
	}
	r.bot = 725;
	for(int i=0;i<10;i++) {
		r.left = 937;
		if (g.topScores[i] != 0)
			ggprint16(&r, 16, 0x00ffffff, "%i", g.topScores[i]);
		r.bot -= 50;
	}
	r.left = 105;
	r.bot = 20;
	ggprint16(&r, 16, 0x00ffffff, "Q - Return to Menu");	
}

void endGame(Global &gl, Game &g)
{
	g.player1.pos[0] = 250;
	g.player1.pos[1] = 175;
	g.player1.health = 100;
}

void showEndScreen(Global &gl, Game &g)
{
	glClear(GL_COLOR_BUFFER_BIT);
	
	renderGameOver(gl);
	
	Rect r;
	r.bot = 800;
	r.left = gl.xres / 2;
	ggprint16(&r, 16, 0x00ffffff, " ");
	// Option box
	glColor3ub(20,74,23);
	glPushMatrix();
	glTranslatef((gl.xres/2),(gl.yres/2), 0);
	glBegin(GL_QUADS);
		glVertex2i(-200, -40);
		glVertex2i(-200, 40);
		glVertex2i(200, 40);
		glVertex2i(200, -40);
	glEnd();
	glPopMatrix();
	// Box text
	r.bot = (gl.yres/2) - 12;
	r.left = (gl.xres/2);
	ggprint16(&r, 16, 0x00ffffff, "Continue");
	// Cursor
	glColor3ub(255, 255, 255);
	glPushMatrix();
	glTranslatef(880, (gl.yres/2) , 0);
	glBegin(GL_TRIANGLES);
		glVertex2f(-55.0f, 0.0f);
		glVertex2f(  0.0f, 30.0f);
		glVertex2f(  -8.0f, 0.0f);
	glEnd();
	glColor3ub(210, 210, 210);
	glBegin(GL_TRIANGLES);
		glVertex2f(  0.0f, -30.0f);
		glVertex2f( -8.0f, 0.0f);
		glVertex2f( -55.0f, 0.0f);
	glEnd();
	glPopMatrix();
}

void enterScore(Global &gl, Game &g)
{
	glClear(GL_COLOR_BUFFER_BIT);
	int scoreoption = g.scoreUI.cursorPos;
	char *arr = g.scoreUI.charList;
	Rect r;
	r.bot = 800;
	r.left = gl.xres / 2;
	ggprint16(&r, 16, 0x00ffffff, "NEW HIGH SCORE");
	r.bot = 700;
	r.left = gl.xres / 2;
	ggprint16(&r, 16, 0x00ffffff, "- ENTER NAME -");
	// instructions
	r.bot = 20;
	r.left = 625; 
	ggprint16(&r, 16, 0x00ffffff, "W / S - Menu Up / Down");
	r.left = 1145;
	r.bot = 20;
	ggprint16(&r, 16, 0x00ffffff, "E - Select");
	// Underlines for chars
	for (int i=0;i<5;i++) {
		glColor3ub(255,255,255);
		glPushMatrix();
		glTranslatef(465 + (i * 80), 450, 0);
		glBegin(GL_QUADS);
			glVertex2i(-20, -2);
			glVertex2i(-20, 2);
			glVertex2i(20, 2);
			glVertex2i(20, -2);
		glEnd();
		glPopMatrix();
	}
	// Render box for continuing
	int txtcolor, bcolor[3];
	int ybox = 280;
	if (scoreoption == 5) {
		txtcolor = 0x00ffffff;
		bcolor[0] = 20;
		bcolor[1] = 74;
		bcolor[2] = 23;
	} else {
		txtcolor = 0x00dddddd;
		bcolor[0] = 7;
		bcolor[1] = 28;
		bcolor[2] = 9;
	}
	glColor3ub(bcolor[0],bcolor[1],bcolor[2]); // darker one is 7, 28, 9
	glPushMatrix();
	glTranslatef(625, ybox, 0);
	glBegin(GL_QUADS);
		glVertex2i(-200, -40);
		glVertex2i(-200, 40);
		glVertex2i(200, 40);
		glVertex2i(200, -40);
	glEnd();
	glPopMatrix();
	r.bot = 268;
	r.left = gl.xres / 2;
	ggprint16(&r, 16, txtcolor, "Continue");
	// Render cursors
	if (scoreoption != 5) {
		int xposition = 465 + (80 * scoreoption);
		// top cursor
		glColor3ub(255, 255, 255);
		glPushMatrix();
		glTranslatef(xposition, 520 , 0);
		glBegin(GL_TRIANGLES);
			glVertex2f(  -30.0f, 0.0f);
			glVertex2f(  0.0f, 55.0f);
			glVertex2f(  0.0f, 8.0f);
		glEnd();
		glColor3ub(210, 210, 210);
		glBegin(GL_TRIANGLES);
			glVertex2f(  30.0f, 0.0f);
			glVertex2f( 0.0f, 8.0f);
			glVertex2f( 0.0f, 55.0f);
		glEnd();
		glPopMatrix();
		// Bottom cursor
		glColor3ub(255, 255, 255);
		glPushMatrix();
		glTranslatef(xposition, 400, 0);
		glBegin(GL_TRIANGLES);
			glVertex2f( 0.0f, -55.0f);
			glVertex2f(-30.0f, 0.0f);
			glVertex2f( 0.0f, -8.0f);
		glEnd();
		glColor3ub(210, 210, 210);
		glBegin(GL_TRIANGLES);
			glVertex2f( 30.0f, 0.0f);
			glVertex2f( 0.0f, -55.0f);
			glVertex2f( 0.0f, -8.0f);
		glEnd();
		glPopMatrix();
	} else {
		glColor3ub(255, 255, 255);
		glPushMatrix();
		glTranslatef(880, ybox, 0);
		glBegin(GL_TRIANGLES);
			glVertex2f(-55.0f, 0.0f);
			glVertex2f(  0.0f, 30.0f);
			glVertex2f(  -8.0f, 0.0f);
		glEnd();
		glColor3ub(210, 210, 210);
		glBegin(GL_TRIANGLES);
			glVertex2f(  0.0f, -30.0f);
			glVertex2f( -8.0f, 0.0f);
			glVertex2f( -55.0f, 0.0f);
		glEnd();
		glPopMatrix();
	}
	// Render letters for name
	char c;
	for (int i=0;i<scoreoption;i++) {
		r.bot = 470;
		r.left = 465 + (i * 80);
		c = g.scoreUI.playerName[i];
		ggprint16(&r, 16, 0x00ffffff, "%c", c);
	}
	if (scoreoption != 5) {
		r.bot = 470;
		r.left = 465 + (scoreoption * 80);
		c = arr[g.scoreUI.charPos];
		ggprint16(&r, 16, 0x00ffffff, "%c", c);
	}
	// instruction to go back
	if (scoreoption > 0) {
		r.left = 130;
		r.bot = 20;
		ggprint16(&r, 16, 0x00ffffff, "Q - Go Back");
	}
}
