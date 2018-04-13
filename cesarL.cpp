// Author: Cesar Lara


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

//High-score functions
//
bool checkScore(Game &g);

bool checkScore(Game &g) {
	bool check = false;
	if (g.topScores[9] < g.score)
		check = true;
	return check;
}

void sortScores(Game &g);

void sortScores(Game &g) {
	string name(g.player1.name);
	g.topPlayers[9] = name;
	g.topScores[9] = g.score;
	for(int oloop=0;oloop<10;oloop++) {
		for(int iloop=0;iloop<9;iloop++) {
			if(g.topScores[iloop] < g.topScores[iloop+1]) {
				swap(g.topScores[iloop],g.topScores[iloop+1]);
				swap(g.topPlayers[iloop],g.topPlayers[iloop+1]);
			}
		}
	}
}

void saveScores(Game &g);

void saveScores(Game &g) {
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
void updateTime(Game &g);

void updateTime(Game &g) {
    	struct timespec pt;
	clock_gettime(CLOCK_REALTIME, &pt);
	g.player1.ptime = timeDiff(&g.player1.time, &pt);
	if (g.player1.ptime > 5.0 && g.wave < 1) {
	    	g.wave++;
	}

}

int checkQuad(Global &gl, Game &g);

int checkQuad(Global &gl, Game &g) {
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

void spawnWave(Global &gl, Game &g) {
    	if(g.wave == 1) {
	    	createZombie(g, checkQuad(gl, g), 3);
		listZombies(g);
		g.wave += 1;
	}
}

void createZombie(Game &g, int pquad, int n) {
	int quad = 4;
	while (quad == pquad) {
		quad = rand() % 4 + 1;
	}
	for(int i=0;i<n;i++) {
		Zombie *z = new Zombie;
		z->pos[0] = g.pos[quad-1][0];
		z->pos[1] = g.pos[quad-1][1];
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
	}
}

// prints the linked list to make sure pointers were assigned correctly
// during createZombie
void listZombies(Game &g);

void listZombies(Game &g) {
    	Zombie *z = g.znext;
	int n = 1;
	while (z) {
	    	cout << n << "th zombie address: " << z << endl
		     << "\tprev: " << z->prev << endl
		     << "\tnext: " << z->next << endl;
		z = z->next;
		n++;
	}
}

void drawZombies(Game &g);

void drawZombies(Game &g) {	
    	Zombie *z = g.znext;
	int w, h;
	while (z) {
	    	w = z->width;
		h = z->height;
	    	glColor3ub(20,74,23);
	    	glPushMatrix();
	    	glTranslatef(z->pos[0], z->pos[1], z->pos[2]);
	    	glBegin(GL_QUADS);
	    		glVertex2i(-w, -h);
			glVertex2i(-w, h);
			glVertex2i(w, h);
			glVertex2i(w, -h);
	    	glEnd();
	    	glPopMatrix();
		z = z->next;
	}
}

// removes one zombie
void removeZombie(Game &g, Zombie *z);

void removeZombie(Game &g, Zombie *z) {
	Zombie *temp = z->next;    
    	if (z->prev == NULL) {
	    	if (z->next == NULL) {
		    g.znext = NULL;
		} else {
		    z->next->prev = NULL;
		    g.znext = z->next;
		}
	} else {
	    	if (z->next == NULL) {
		    z->prev->next = NULL;
		} else {
		    z->prev->next = z->next;
		    z->next->prev = z->prev;
		}
	}
	delete z;
	z = temp;
	g.nzombies--;
}

// traverses list and deletes all zombies
void deleteZombies(Game &g);

void deleteZombies(Game &g) {
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

void showMenu(Global &gl);

void showMenu(Global &gl) {
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
	//
	glClear(GL_COLOR_BUFFER_BIT);
        Rect r;
        r.bot = 700;
        r.center = 450;
        r.left = 625;
        ggprint16(&r, 16, 0x00ffffff, "ZOMBIE SHOOTER");
        // Render boxes for main menu options
	//
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
	//
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
	//
        glColor3ub(255, 255, 255);
        glPushMatrix();
	int position;
	if (gl.menuOption == 0)
		position = 500;
	if (gl.menuOption == 1)
		position = 380;
	if (gl.menuOption == 2)
		position = 260;
	if (gl.menuOption == 3)
		position = 140;
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

void showScores(Global &gl, Game &g);

void showScores(Global &gl, Game &g) {
	glClear(GL_COLOR_BUFFER_BIT);
	Rect r;
	r.bot = 800;
	r.left = 625;
	ggprint16(&r, 16, 0x00ffffff, "High Scores");
	r.bot = 725;
	r.left = 312;
	for(int i=0;i<10;i++) {
		r.left = 312;
		if (g.topPlayers[i] != "null")
	    		ggprint16(&r, 16, 0x00ffffff, "NAME: ");
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
			ggprint16(&r, 16, 0x00ffffff, "SCORE");
		r.bot -= 50;
	}
	
}

void showEndScreen(Global &gl, Game &g);

void showEndScreen(Global &gl, Game &g) {
}

void enterScore(Global &gl, Game &g);

void enterScore(Global &gl, Game &g) {
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
	// Underlines for chars
	for (int i=0;i<5;i++) {
		glColor3ub(255,255,255);
                glPushMatrix();
                glTranslatef(465 + (i * 80), 380, 0);
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
	glTranslatef(625, 280, 0);
	glBegin(GL_QUADS);
		glVertex2i(-200, -40);
		glVertex2i(-200, 40);
        	glVertex2i(200, 40);
        	glVertex2i(200, -40);
	glEnd();
	glPopMatrix();
	// Render cursors
	glColor3ub(255, 255, 255);
        glPushMatrix();
        int xposition;
        if (gl.menuOption == 0)
                xposition = 500;
        if (gl.menuOption == 1)
                xposition = 380;
        if (gl.menuOption == 2)
                xposition = 260;
        if (gl.menuOption == 3)
                xposition = 140;
        glTranslatef(880, xposition , 0);
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

	r.bot = 280;
	r.left = gl.xres / 2;
	ggprint16(&r, 16, txtcolor, "%s", "Continue");
	// Render letters for name
	for (int i=0;i<(scoreoption+1);i++) {
	    	r.bot = 400;
		r.left = 465 + (i * 80);
		ggprint16(&r, 16, 0x00ffffff, "%c", arr[i]);
	}

	

}
