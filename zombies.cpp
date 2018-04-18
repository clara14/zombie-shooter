//program: zombies.cpp
#include "zlib.h"

#define PROFILING_OFF

//function prototypes
void init_opengl(Global &gl, Game &g);
int check_keys(XEvent *e, Global &gl, Game &g);
void physics(Global &gl, Game &g);
void cesar_physics_and_movement(Global &gl, Game &g);
void render(Global &gl, Game &g);
void normalize2d(Vec v);
extern double timeDiff(struct timespec *start, struct timespec *end);
extern void timeCopy(struct timespec *dest, struct timespec *source);

extern void display_name_cesar(Global &gl, Game &g);


// Alfredo's Functions---------------------------------------------------------
extern void displayHUD(Global &gl, Game &g);
extern void displayAlfredo(int botPos, int leftPos, int centerPos,
			int textColor, const char* textName); 
extern void initialize_player1(Global &gl);
extern void initialize_mainRoad(Global &gl);
extern void draw_player1(Global &gl, Game &g);
extern void drawMainRoad(Global &gl, Game &g);
//----------------------------------------------------------------------------

extern void showFloor(Global &gl, Game &g);
extern void show_jorge(Global &gl, Game &g);
extern void showMenu(Global &gl);
extern void showTutorial(Global &gl);
extern void showScores(Global &gl, Game &g);
extern void updateTime(Game &g);
extern void drawZombies(Game &g);
extern int checkQuad(Global &gl, Game &g);
extern void deleteZombies(Game &g);
extern void spawnWave(Global &gl, Game &g);
extern void enterScore(Global &gl, Game &g);
extern void showEndScreen(Global &gl, Game &g);
extern bool checkScore(Game &g);
extern void endGame(Global &gl, Game &g);
extern void enterScoreXK_e(Global &gl, Game &g);
extern void enterScoreXK_w(Game &g);
extern void enterScoreXK_s(Game &g);
extern void enterScoreXK_q(Game &g);

#define MENU 1
#define GAME 2
#define HELP 3
#define SCORES 4
#define END 5
#define NEWSCORE 6

bool displayNames = false;
bool showCesarL = false;

int main()
{
	logOpen();
	Global gl;
	X11_wrapper x11(gl);
	Game g(gl);
	init_opengl(gl, g);
	srand(time(NULL));
	x11.set_mouse_position(100, 100);
	int done=0;
	while (!done) {
		while (x11.getXPending()) {
			XEvent e = x11.getXNextEvent();
			x11.check_resize(&e, gl);
			done = check_keys(&e, gl, g);
		}
		physics(gl,g);
		render(gl,g);
		x11.swapBuffers();
	}
	cleanup_fonts();
	logClose();
	return 0;
}

// Function definitions

void physics(Global &gl, Game &g)
{
	if (gl.menuState == GAME) {
		updateTime(g);
		spawnWave(gl, g);
	    	cesar_physics_and_movement(gl, g);
		if (g.player1.health <= 0) {
			endGame(gl, g);
			gl.menuState = END;
		}
	}	
}

int check_keys(XEvent *e, Global &gl, Game &g)
{
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
                        if (gl.menuState == MENU) {
				return 1;
			}
			if (gl.menuState == GAME) {
				gl.menuState = MENU;
				deleteZombies(g);
			}
			if (gl.menuState == HELP) {
				if (gl.helpScreen == 1)
					gl.menuState = MENU;
			}
			if (gl.menuState == SCORES) {
				gl.menuState = MENU;
			}
			break;
                case XK_w:
			if (gl.menuState == MENU) {
				if (gl.menuOption > 0)
					gl.menuOption--;
			} else if (gl.menuState == NEWSCORE) {
				enterScoreXK_w(g);
			}
                        break;
                case XK_a:
                        break;
                case XK_s:
                        if (gl.menuState == MENU) {
				if (gl.menuOption < 3)
					gl.menuOption++;
                        } else if (gl.menuState == NEWSCORE) {
				enterScoreXK_s(g);
			}
                        break;
                case XK_d:
                        break;
		case XK_e:
			if (gl.menuState == MENU) {
				if (gl.menuOption == 0) {
				    	struct timespec pt;
				    	clock_gettime(CLOCK_REALTIME, &pt);
					timeCopy(&g.player1.time, &pt);
					g.wave = 0;
					gl.menuState = GAME;
				} else if (gl.menuOption == 1) {
					gl.menuState = HELP;
				} else if (gl.menuOption == 2) {
					gl.menuState = SCORES;
				} else if (gl.menuOption == 3) {
					return 1;
				}
			} else if (gl.menuState == HELP) {
				if (gl.helpScreen < 4)
					gl.helpScreen += 1;
				else {
					gl.menuState = MENU;
				}
			} else if (gl.menuState == END) {
				if (checkScore(g)) {
					gl.menuState = NEWSCORE;
				} else {
					gl.menuState = MENU;
					endGame(gl, g);
				}
			} else if (gl.menuState == NEWSCORE) {
				enterScoreXK_e(gl, g);
			}	 
			break;
		case XK_q:
			if (gl.menuState == HELP) {
				if (gl.helpScreen > 1)
					gl.helpScreen -= 1;
				else 
					gl.menuState = MENU;
			} else if (gl.menuState == SCORES) {
				gl.menuState = MENU;
			} else if (gl.menuState == NEWSCORE) {
				enterScoreXK_q(g);
			}
			break;
                case XK_equal:
                        break;
                case XK_minus:
                        break;
		case XK_n:
			displayNames = true;
			break;
		case XK_l:
			if (showCesarL)
				showCesarL = false;
			else
				showCesarL = true;
			break;
        }
        return 0;
}

unsigned char *buildAlphaData(Image *img)                                       
{                                                                     
	// transparency color RGB value: (180, 250, 188)          
	//add 4th component to RGB stream...                                    
	int i;                                                                  
	int a,b,c;                                                              
	unsigned char *newdata, *ptr;                                           
	unsigned char *data = (unsigned char *)img->data;                       
	newdata = (unsigned char *)malloc(img->width * img->height * 4);        
	ptr = newdata;                                                          
	for (i=0; i<img->width * img->height * 3; i+=3) {                       
		a = *(data+0);                                                  
		b = *(data+1);                                                  
		c = *(data+2);                                                  
		*(ptr+0) = a;                                                   
		*(ptr+1) = b;                                                   
		*(ptr+2) = c;                                                   
		//-----------------------------------------------               
		//get largest color component...                                
		//*(ptr+3) = (unsigned char)((                                  
		//              (int)*(ptr+0) +                                 
		//              (int)*(ptr+1) +                                 
		//              (int)*(ptr+2)) / 3);                            
		//d = a;                                                        
		//if (b >= a && b >= c) d = b;                                  
		//if (c >= a && c >= b) d = c;                                  
		//*(ptr+3) = d;                                                 
		//-----------------------------------------------               
		//this code optimizes the commented code above.                 
		*(ptr+3) = (a|b|c);

		/*
		   if( a == trans_color[0] && b == trans_color[1] && 
		   c == trans_color[2] ) {

		 *(ptr + 3) = 0;
		 }

		 */
		//-----------------------------------------------               
		ptr += 4;                                                       
		data += 3;                                                      
	}                                                                       
	return newdata;                                                         
}                           


void normalize2d(Vec v)
{
        Flt len = v[0]*v[0] + v[1]*v[1];
        if (len == 0.0f) {
                v[0] = 1.0;
                v[1] = 0.0;
                return;
        }
        len = 1.0f / sqrt(len);
        v[0] *= len;
        v[1] *= len;
}


void init_opengl(Global &gl, Game &g)
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
	glClearColor(1.0, 1.0, 1.0, 1.0);
	//Do this to allow fonts
	glEnable(GL_TEXTURE_2D);
	initialize_fonts();


//	initialize_mainRoad(gl);
//	initialize_player1(gl);
}



extern void displayCesarL(int, int, int, int, const char*);

void render(Global &gl, Game &g)
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f( 1.0, 1.0, 1.0);


	// Show the menu, game, tutorial, or high scores
	if (gl.menuState == MENU) {
		showMenu(gl);
	} else if (gl.menuState == GAME) {
		glClear(GL_COLOR_BUFFER_BIT);
		// rendering the heads up display	
		displayHUD(gl, g);

//============================================================================
#ifdef PROFILING_ON
		// Displaying group names for lab5 assignment
		//
		if ( displayNames == false ) {	
			displayAlfredo( 500, 100, 150, 0x0079ccb3, 
					"Press 'n' to display names");	
		} else {
			displayAlfredo( 500, 100, 150, 0x0079ccb3, "Alfredo Zavala");	
		}

		if (showCesarL)
			displayCesarL(600, 150, 200, 0x009508f8, "Cesar Lara");
		else
			displayCesarL(600, 150, 200, 0x009508f8,
					"Press 'L' to display my name");
	
		display_name_cesar(gl, g);
	
		show_jorge(gl, g);

		Rect r;
		r.bot = 800;
		r.left = 500;
		int quad = checkQuad(gl, g);
		ggprint16(&r, 16, 0x009508f8, "player quad: %i", quad);
		r.bot = 750;
		quad = g.nzombies;
		ggprint16(&r, 16, 0x009508f8, "zombies: %i", quad);
#endif
//=============================================================================
		
		showFloor(gl, g);
        	
		//draw_player1(gl, g);
		//-------------
        	//Draw the ship
        	
		glColor3fv(g.ship.color);
        	glPushMatrix();
        	glTranslatef(g.player1.pos[0], g.player1.pos[1], g.player1.pos[2]);
        	glRotatef(g.player1.angle, 0.0f, 0.0f, 1.0f);
        	glBegin(GL_TRIANGLES);
                	glVertex2f(-12.0f, -10.0f);
                	glVertex2f(  0.0f, 20.0f);
                	glVertex2f(  0.0f, -6.0f);
                	glVertex2f(  0.0f, -6.0f);
                	glVertex2f(  0.0f, 20.0f);
                	glVertex2f( 12.0f, -10.0f);
        	glEnd();
        	glColor3f(1.0f, 0.0f, 0.0f);
        	glBegin(GL_POINTS);
                glVertex2f(0.0f, 0.0f);
        	glEnd();
        	glPopMatrix();
        
		
/*
		if (gl.keys[XK_Up] || g.mouseThrustOn) {
                	int i;
                	//draw thrust
                	Flt rad = ((g.player1.angle+90.0) / 360.0f) * PI * 2.0;
                	//convert angle to a vector
                	Flt xdir = cos(rad);
                	Flt ydir = sin(rad);
                	Flt xs,ys,xe,ye,r;
                	glBegin(GL_LINES);
                        	for (i=0; i<16; i++) {
                                	xs = -xdir * 11.0f + rnd() * 4.0 - 2.0;
                                	ys = -ydir * 11.0f + rnd() * 4.0 - 2.0;
                                	r = rnd()*40.0+40.0;
                                	xe = -xdir * r + rnd() * 18.0 - 9.0;
                                	ye = -ydir * r + rnd() * 18.0 - 9.0;
                                	glColor3f(rnd()*.3+.7, rnd()*.3+.7, 0);
                                	glVertex2f(g.player1.pos[0]+xs,g.player1.pos[1]+ys);
                                	glVertex2f(g.player1.pos[0]+xe,g.player1.pos[1]+ye);
                        	}
                	glEnd();

        	}
*/
        	//------------------
        	//----------------
        	//Draw the bullets
        	Bullet *b = &g.barr[0];
        	for (int i=0; i<g.nbullets; i++) {
                	//Log("draw bullet...\n");
                	glColor3f(1.0, 1.0, 1.0);
                	glBegin(GL_POINTS);
                        	glVertex2f(b->pos[0],      b->pos[1]);
                        	glVertex2f(b->pos[0]-1.0f, b->pos[1]);
                        	glVertex2f(b->pos[0]+1.0f, b->pos[1]);
                        	glVertex2f(b->pos[0],      b->pos[1]-1.0f);
                        	glVertex2f(b->pos[0],      b->pos[1]+1.0f);
                        glColor3f(0.8, 0.8, 0.8);
                        	glVertex2f(b->pos[0]-1.0f, b->pos[1]-1.0f);
                        	glVertex2f(b->pos[0]-1.0f, b->pos[1]+1.0f);
                        	glVertex2f(b->pos[0]+1.0f, b->pos[1]-1.0f);
                        	glVertex2f(b->pos[0]+1.0f, b->pos[1]+1.0f);
                	glEnd();
                	++b;
        	}
		// Draw all existing zombies
		drawZombies(g);
	}
	
	else if (gl.menuState == HELP) {
		showTutorial(gl);
	}
	
	else if (gl.menuState == SCORES) {
		showScores(gl, g);
	}
	
	else if (gl.menuState == END) {
		showEndScreen(gl, g);
	}

	else if (gl.menuState == NEWSCORE) {
		enterScore(gl, g);
	}
}

