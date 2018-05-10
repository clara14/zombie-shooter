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
extern void renderHealthBarHUD(Global &gl);


// Alfredo's Functions---------------------------------------------------------
extern void displayHUD(Global &gl, Game &g);
extern void displayAlfredo(int botPos, int leftPos, int centerPos,
		int textColor, const char* textName); 
extern void draw_player1(Global &gl, Game &g);
extern void drawMainRoad(Global &gl, Game &g);
extern void displayGridTest (); 
extern void renderMovingBackground(Global &gl, Game &g);
extern void physicsScrollingBackground(Global &gl);
extern void renderForegroundArena(Global  &gl, Game &g); 
extern void renderScoreHUD(Global &gl, Game &g);
extern void displyGameScore(Global &gl, Game &g);
extern void renderPlayerAvatarHUD(Global &gl);
extern void renderPlayerWeaponHUD(Global &gl);
extern void renderHealthBarHUD(Global &gl);
extern void renderHealthBar2HUD(Global &gl);
extern void renderHealthBar3HUD(Global &gl);

extern void renderGameOver(Global &gl);
//----------------------------------------------------------------------------

extern void showFloor(Global &gl, Game &g);
extern void show_jorge(Global &gl, Game &g);
extern void showMenu(Global &gl);
extern void showTutorial(Global &gl);
extern void showScores(Global &gl, Game &g);
extern void updateTime(Game &g);
extern void drawZombies(Global &gl, Game &g);
extern int checkQuad(Global &gl, Game &g);
extern void deleteZombies(Game &g);
extern void spawnWave(Global &gl, Game &g);
extern void enterScore(Global &gl, Game &g);
extern void showEndScreen(Global &gl, Game &g);
extern bool checkScore(Game &g);
extern void endGame(Global &gl, Game &g);
extern int xk_escape(Global &gl, Game &g);
extern int xk_e(Global &gl, Game &g);
extern void xk_w(Global &gl, Game &g);
extern void xk_s(Global &gl, Game &g);
extern void xk_q(Global &gl, Game &g);
extern void saveScores(Game &g);

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
			deleteZombies(g);
		}
	}	


	physicsScrollingBackground(gl);
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
			if (xk_escape(gl, g) == 1)
				return 1;
			break;
		case XK_w:
			xk_w(gl, g);
			break;
		case XK_a:
			g.player1.facingRight = false;
			cout << "facingRight = false" << endl;
			break;
		case XK_s:
			xk_s(gl, g);
			break;
		case XK_d:
			g.player1.facingRight = true;
			cout << "facingRight = true" << endl;
			break;
		case XK_e:
			if (xk_e(gl,g) == 1)
				return 1;
			break;
		case XK_q:
			xk_q(gl, g);
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

Image mainCharacter = "./images/doomguy.jpg";
Image mainRoad = "./images/darkRoad.png";
Image zombieCharacters = "./images/zombieCreature.jpg";
Image bulletProjectiles = "./images/bullet.png";

Image bgTexture = "./images/greenForest.jpg";
Image foreground = "./images/foreground.jpg";
Image gameScoreHUD = "./images/scoreHUD.png";
Image menuBg = "./images/menuback.jpg";
Image menuSelect = "./images/menuSelect.jpg";
Image gameOver = "./images/gameOver.png";

Image playerAvatar = "./images/playerHUD.png";
Image healthBar = "./images/healthBar.png";
Image healthBar2 = "./images/healthBar2.png";
Image healthBar3 = "./images/healthBar3.png";
Image playerWeapon = "./images/assaultRifle.jpg";


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

	glGenTextures(1, &gl.mainCharacterTexture);
	glGenTextures(1, &gl.mainRoadTexture);
	glGenTextures(1, &gl.zombieTexture);
	glGenTextures(1, &gl.bulletProjectileTexture);
	glGenTextures(1, &gl.foregroundTexture);
	glGenTextures(1, &gl.gameScoreTexture);
	glGenTextures(1, &gl.menuBgTexture);
	glGenTextures(1, &gl.playerAvatarTexture);
	glGenTextures(1, &gl.healthBarTexture);
	glGenTextures(1, &gl.healthBar2Texture);
	glGenTextures(1, &gl.healthBar3Texture);
	glGenTextures(1, &gl.playerWeaponTexture);
	glGenTextures(1, &gl.menuSelectTexture);
	glGenTextures(1, &gl.gameOverTexture);

	// main character 
	int w = mainCharacter.width;
	int h = mainCharacter.height;

	// PLAYER 1 
	glBindTexture(GL_TEXTURE_2D, gl.mainCharacterTexture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, w, h, 0, 
			GL_RGB, GL_UNSIGNED_BYTE, mainCharacter.data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	unsigned char *silhouetteData = buildAlphaData(&mainCharacter);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mainCharacter.width,
			mainCharacter.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, silhouetteData);

	free(silhouetteData);
	//--------------------------------------------------------------------
	//

	// ZOMBIE CHARACTERS 
	glBindTexture(GL_TEXTURE_2D, gl.zombieTexture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, w, h, 0, 
			GL_RGB, GL_UNSIGNED_BYTE, zombieCharacters.data);


	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	unsigned char * zombieData = buildAlphaData(&zombieCharacters);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, zombieCharacters.width,
			zombieCharacters.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 
			zombieData);

	free(zombieData);
	//--------------------------------------------------------------------
	//

	// BULLET PROJECTILES
	glBindTexture(GL_TEXTURE_2D, gl.bulletProjectileTexture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, w, h, 0, 
			GL_RGB, GL_UNSIGNED_BYTE, bulletProjectiles.data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	unsigned char * bulletProjectileData = buildAlphaData(&bulletProjectiles);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bulletProjectiles.width,
			bulletProjectiles.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 
			bulletProjectileData);

	free(bulletProjectileData);

	//--------------------------------------------------------------------
	//

	// BACKGROUND TEXTURE
	gl.texture.backImage = &bgTexture;

	glGenTextures(1, &gl.texture.backTexture);                                      
	w = gl.texture.backImage->width;                                                
	h = gl.texture.backImage->height;                                               
	glBindTexture(GL_TEXTURE_2D, gl.texture.backTexture);                           
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_NEAREST);           
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_NEAREST);           
	glTexImage2D(GL_TEXTURE_2D, 0,3,w,h,0,                                      
			GL_RGB, GL_UNSIGNED_BYTE, gl.texture.backImage->data);                  
	gl.texture.xc[0] = 0.0;                                                         
	gl.texture.xc[1] = 0.25;                                                        
	gl.texture.yc[0] = 0.0;                                                         
	gl.texture.yc[1] = 1.0;                                                         

	//--------------------------------------------------------------------
	//

	// FOREGROUND TEXTURE
	glBindTexture(GL_TEXTURE_2D, gl.foregroundTexture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, w, h, 0, 
			GL_RGB, GL_UNSIGNED_BYTE, foreground.data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	unsigned char * fgData = buildAlphaData(&foreground);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, foreground.width,
			foreground.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 
			fgData);

	free(fgData);

	//--------------------------------------------------------------------
	//

	// GAMESCORE HUD TEXTURE
	w = gameScoreHUD.width;
	h = gameScoreHUD.height;

	glBindTexture(GL_TEXTURE_2D, gl.gameScoreTexture);


	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, w, h, 0,
			GL_RGB, GL_UNSIGNED_BYTE, gameScoreHUD.data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	unsigned char * gameScoreData = buildAlphaData(&gameScoreHUD);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, gameScoreHUD.width,
			gameScoreHUD.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 
			gameScoreData);

	free(gameScoreData);

	//--------------------------------------------------------------------
	//

	// GAME MENU BACKGROUND 
	w = menuBg.width;
	h = menuBg.height;

	glBindTexture(GL_TEXTURE_2D, gl.menuBgTexture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, w, h, 0,
			GL_RGB, GL_UNSIGNED_BYTE, menuBg.data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	unsigned char * menuBgData = buildAlphaData(&menuBg);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, menuBg.width,
			menuBg.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 
			menuBgData);

	free(menuBgData);

	//--------------------------------------------------------------------
	//

	// PLAYER AVATAR HUD DISPLAY 
	w = playerAvatar.width;
	h = playerAvatar.height;

	glBindTexture(GL_TEXTURE_2D, gl.playerAvatarTexture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, w, h, 0,
			GL_RGB, GL_UNSIGNED_BYTE, playerAvatar.data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	unsigned char * playerAvatarData = buildAlphaData(&playerAvatar);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, playerAvatar.width,
			playerAvatar.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 
			playerAvatarData);

	free(playerAvatarData);


	//--------------------------------------------------------------------
	//

	// PLAYER HEALTH BAR 
	w = healthBar.width;
	h = healthBar.height;

	glBindTexture(GL_TEXTURE_2D, gl.healthBarTexture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, w, h, 0,
			GL_RGB, GL_UNSIGNED_BYTE, healthBar.data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	unsigned char * healthBarData = buildAlphaData(&healthBar);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, healthBar.width,
			healthBar.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 
			healthBarData);

	free(healthBarData);

	//--------------------------------------------------------------------
	//

	// PLAYER HEALTH BAR 2 ( health is 50 )
	w = healthBar2.width;
	h = healthBar2.height;

	glBindTexture(GL_TEXTURE_2D, gl.healthBar2Texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, w, h, 0,
			GL_RGB, GL_UNSIGNED_BYTE, healthBar2.data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	unsigned char * healthBar2Data = buildAlphaData(&healthBar2);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, healthBar2.width,
			healthBar2.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 
			healthBar2Data);

	free(healthBar2Data);
	
	//--------------------------------------------------------------------
	//

	// PLAYER HEALTH BAR 3 ( health is 0 )
	w = healthBar3.width;
	h = healthBar3.height;

	glBindTexture(GL_TEXTURE_2D, gl.healthBar3Texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, w, h, 0,
			GL_RGB, GL_UNSIGNED_BYTE, healthBar3.data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	unsigned char * healthBar3Data = buildAlphaData(&healthBar3);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, healthBar3.width,
			healthBar3.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 
			healthBar3Data);

	free(healthBar3Data);



	//--------------------------------------------------------------------
	//

	// PLAYER WEAPON 
	w = playerWeapon.width;
	h = playerWeapon.height;

	glBindTexture(GL_TEXTURE_2D, gl.playerWeaponTexture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, w, h, 0,
			GL_RGB, GL_UNSIGNED_BYTE, playerWeapon.data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	unsigned char * playerWeaponData = buildAlphaData(&playerWeapon);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, playerWeapon.width,
			playerWeapon.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 
			playerWeaponData);

	free(playerWeaponData);

	//--------------------------------------------------------------------
	//

	// MEAN SELECT ICON  
	w = menuSelect.width;
	h = menuSelect.height;

	glBindTexture(GL_TEXTURE_2D, gl.menuSelectTexture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, w, h, 0,
			GL_RGB, GL_UNSIGNED_BYTE, menuSelect.data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	unsigned char * menuSelectData = buildAlphaData(&menuSelect);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, menuSelect.width,
			menuSelect.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 
			menuSelectData);

	free(menuSelectData);

	//--------------------------------------------------------------------
	//

	// GAME OVER END SCREEN  
	w = gameOver.width;
	h = gameOver.height;

	glBindTexture(GL_TEXTURE_2D, gl.gameOverTexture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, w, h, 0,
			GL_RGB, GL_UNSIGNED_BYTE, gameOver.data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	unsigned char * gameOverData = buildAlphaData(&gameOver);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, gameOver.width,
			gameOver.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 
			gameOverData);

	free(gameOverData);







}



extern void displayCesarL(int, int, int, int, const char*);

void render(Global &gl, Game &g)
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);
	//glColor3f( 1.0, 1.0, 1.0);


	// Show the menu, game, tutorial, or high scores
	if (gl.menuState == MENU) {
		showMenu(gl);
	} else if (gl.menuState == GAME) {
		glClear(GL_COLOR_BUFFER_BIT);

		//====================================================================
#ifdef PROFILING_ON
		// Displaying group names for lab5 assignment
		//
		if ( displayNames == false ) {	
			displayAlfredo( 500, 100, 150, 0x0079ccb3, 
					"Press 'n' to display names");	
		} else {
			displayAlfredo(500,100,150,0x0079ccb3,"AlfredoZavala");	
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
		//====================================================================

		renderMovingBackground(gl,g);

		renderScoreHUD(gl, g); 

		displyGameScore(gl, g);

		renderForegroundArena(gl, g); 

		renderPlayerAvatarHUD(gl);
		
		if (g.player1.health == 100) {
			renderHealthBarHUD(gl);
		} 
		if (g.player1.health == 50) {
			renderHealthBar2HUD(gl);
		}
		if (g.player1.health < 50) {	
			renderHealthBarHUD(gl);
		}

		renderPlayerWeaponHUD(gl);

		draw_player1(gl, g);

		// USED FOR DISPLAYING PLAYER INFORMATION
		//displayHUD(gl, g);
		// USED FOR DISPLAY GRID POSITONS FOR VERTICES
		//displayGridTest (); 

		//------------------
		//----------------
		//Draw the bullets
		Bullet *b = &g.barr[0];
		for (int i=0; i<g.nbullets; i++) {
			//Log("draw bullet...\n");

			//glColor3fv(g.barr->color);
			glPushMatrix();
			glBindTexture(GL_TEXTURE_2D, gl.bulletProjectileTexture);

			glEnable(GL_ALPHA_TEST);
			glAlphaFunc(GL_GREATER, 0.0f);
			glColor4ub(255,255,255,255);


			glBegin(GL_QUADS);
			glTexCoord2f(b->pos[0],      b->pos[1]);
			glTexCoord2f(b->pos[0]-1.0f, b->pos[1]);
			glTexCoord2f(b->pos[0]+1.0f, b->pos[1]);
			glTexCoord2f(b->pos[0],      b->pos[1]-1.0f);
			glTexCoord2f(b->pos[0],      b->pos[1]+1.0f);
			//glColor3f(0.8, 0.8, 0.8);


			glVertex2i(b->pos[0], b->pos[1]);
			glVertex2i(b->pos[0], b->pos[1] + 3);
			glVertex2i(b->pos[0] + 15, b->pos[1]+ 3);
			glVertex2i(b->pos[0] + 15, b->pos[1]);

			glEnd();
			++b;
		}
		// Draw all existing zombies
		drawZombies(gl, g);
	} else if (gl.menuState == HELP) {
		showTutorial(gl);
	} else if (gl.menuState == SCORES) {
		showScores(gl, g);
	} else if (gl.menuState == END) {
		showEndScreen(gl, g);
	} else if (gl.menuState == NEWSCORE) {
		enterScore(gl, g);
	}
}   
