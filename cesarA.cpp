//Cesar Aleman
//movement and boundaries of the play area
//this also includes the bullets currently we do have
//a cap on how many bullets there are but our basic requirement
//only involves infintite bullets once we get that working we will
//incorportate a mechanic that allows resupply and limits on bullers.

//21MAR2018 update: bug has been fixed explained below
//22MAR2018 update: tried to add another oject into the
//                  the screen but the game crashed. Also
//                  made the bug fix solution into a function
//23MAR2018 update: collision detection is being added, early stages
//06APR2018 update: collision dectection now working.
//09APR2018 update: collision with zombie is in beginning stages
//13APR2018 update: health is now being subtracted from the player at a
//		    appropriate rate once there is collision between
//		    player and zombie.
//27APR2018 update: changed the movement of the zombies to make it 
//					easier to manuever around them.
//30APR2019 update: made some changes to help remove some uneeded comments
//					and other lines


#include "zlib.h"
#include <time.h>       
#include <math.h>
#include <unistd.h>
#include </usr/include/AL/alut.h>

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>


extern struct timespec timeStart, timeCurrent;
extern double timeDiff(struct timespec *start, struct timespec *end);
extern void timeCopy(struct timespec *dest, struct timespec *source);
extern void removeZombie(Game &g, Zombie *z);
extern void normalize2d(Vec);

void bug_fix(Game &g);

double time_function1();
double time_function2();
double time_function3();
double time_function4();
void soundTesting (Global &gl, Game &g);

int BULLET_DAMAGE = 50;
int ZOMBIE_DAMAGE = 50;

void display_name_cesar(Global &gl, Game &g)
{
	static double time = 0.0;
    struct timespec timeStart, timeEnd;
	clock_gettime(CLOCK_REALTIME, &timeStart);

	Rect r2;
	Rect r1;
	    
	r1.bot = 750;
	r1.left = 100;
	r1.center = 80;
	ggprint8b(&r1, 16, 0x00ff0000, "Cesar Aleman");

	clock_gettime(CLOCK_REALTIME, &timeEnd);
	time += timeDiff(&timeStart, &timeEnd);

	r2.bot = 725;
	r2.left = 100;
	r2.center = 80;
	   
	ggprint8b(&r2, 20, 0x00fff000, "Display name func: %lf", (time * 1000));
	ggprint8b(&r2, 20, 0x00fff000, "using pow function: %lf", (time_function1() * 1000));
	ggprint8b(&r2, 20, 0x00fff000, "using asterisk: %lf", (time_function2() * 1000));
	ggprint8b(&r2, 20, 0x00fff000, "using sqrt: %lf", (time_function3() * 1000));
	ggprint8b(&r2, 20, 0x00fff000, "using pow with 0.5: %lf", (time_function4() * 1000));	 
	ggprint8b(&r2, 20, 0x00fff000, "y postion of player1: %lf", g.player1.pos[1]);
	ggprint8b(&r2, 20, 0x00fff000, "x postion of player1: %lf", g.player1.pos[0]);

}

double time_function1()
{
  //using pow function to calculate the 2^2 exponetially
	static double time = 0.0;
	struct timespec timeStart, timeEnd;
	clock_gettime(CLOCK_REALTIME, &timeStart);
	int i = 0;
	int a = 2;
	while (i < 100) {
		a = pow(a, 2);
		i++;
	}

	clock_gettime(CLOCK_REALTIME, &timeEnd);
	time += timeDiff(&timeStart, &timeEnd);
	return time; 
}

double time_function2()
{
  //using asterisk
	static double time = 0.0;
	struct timespec timeStart, timeEnd;
	clock_gettime(CLOCK_REALTIME, &timeStart);
	int i = 0;
	int a = 2;
	while (i < 100) {
		a = a * a;
		i++;
	}
	clock_gettime(CLOCK_REALTIME, &timeEnd);
	time += timeDiff(&timeStart, &timeEnd);
	return time; 
}

double time_function3()
{
	//using sqrt as compared to pow(x, 0.5)
	static double time = 0.0;
	struct timespec timeStart, timeEnd;
	clock_gettime(CLOCK_REALTIME, &timeStart);
	int i = 0;
	int a = 1000;
	while (i < 100) {
		a = sqrt(a);
		i++;
	}
	clock_gettime(CLOCK_REALTIME, &timeEnd);
	time += timeDiff(&timeStart, &timeEnd);
	return time; 
}

double time_function4()
{
	//using using pow(x,0.5) as compared to sqrt(x)
	static double time = 0.0;
	struct timespec timeStart, timeEnd;
	clock_gettime(CLOCK_REALTIME, &timeStart);
	int i = 0;
	int a = 1000;
	while (i < 100) {
		a = pow(a, 0.5);
		i++;
	}
	clock_gettime(CLOCK_REALTIME, &timeEnd);
	time += timeDiff(&timeStart, &timeEnd);
	return time; 
}

void cesar_physics_and_movement (Global &gl, Game &g)
{
	bug_fix(g);

	  //end of wall
	if (g.player1.pos[0] < 20.0) {
		g.player1.pos[0] = 20;
	}
	else if (g.player1.pos[0] > ((float)gl.xres - 20)) {
		g.player1.pos[0] = gl.xres - 20;
	}
	else if (g.player1.pos[1] > 315) {
		g.player1.pos[1] = 315;
	}
	else if (g.player1.pos[1] < 40) {
		g.player1.pos[1] = 40;
	}

	  //bullet positions
	struct timespec bt;
	clock_gettime(CLOCK_REALTIME, &bt);
	int i = 0;
	while (i < g.nbullets) {
		Bullet *b = &g.barr[i];
		double ts = timeDiff(&b->time, &bt);
		if (ts > 2.5) {
			//delete the bullet.
			memcpy(&g.barr[i], &g.barr[g.nbullets-1],
			sizeof(Bullet));
			g.nbullets--;
			continue;
	}
		//move the bullet
		b->pos[0] += b->vel[0];
		b->pos[1] += b->vel[1];
		//window edge collision
		//along with deletion of bullet once it has left the boundary
		if (b->pos[0] < 0.0) {
			b->pos[0] += 5;
			memcpy(&g.barr[i], &g.barr[g.nbullets-1], sizeof(Bullet));
			g.nbullets--;
		}
		else if (b->pos[0] > (float)gl.xres) {
			b->pos[0] = gl.xres + 5;
			memcpy(&g.barr[i], &g.barr[g.nbullets-1], sizeof(Bullet));
			g.nbullets--;
		}
		else if (b->pos[1] < 0.0) {
			b->pos[1] = gl.xres;
			memcpy(&g.barr[i], &g.barr[g.nbullets-1], sizeof(Bullet));
			g.nbullets--;
		}
		else if (b->pos[1] > (float)gl.yres) {
			b->pos[1] = gl.yres;
			memcpy(&g.barr[i], &g.barr[g.nbullets-1], sizeof(Bullet));
			g.nbullets--;
		}
		i++;
	} 
	//collision detection between zombie and bullet
	if (g.nzombies > 0)
		cout << "Before collision detection" << endl;
	Zombie *z = g.znext;
	while (z) {
		int w, h, i = 0;
		w = z->width;
		h = z->height;
		cout << "inside 'while(z)' function" << endl;
		//while (i < g.nbullets) {
		Bullet *b = &g.barr[i];
		//collision with bullet
		cout << "bfore long 'if' statement" << endl;
		cout << "value of x-coordinate of bullet: " << b->pos[0] << endl;
		cout << "value of y-coordinate of bullet: " << b->pos[1] << endl;
		cout << "value of x-coordinate of zombie: " << z->pos[0] << endl;
		cout << "value of x-coordinate of zombie: " << z->pos[1] << endl;
				
		//hit has been detected
		if (b->pos[0] >= z->pos[0] - w &&
			b->pos[0] <= z->pos[0] + w &&
			b->pos[1] <= z->pos[1] + h &&
			b->pos[1] >= z->pos[1] - h) {
				    	
			b->damage = BULLET_DAMAGE;	    
			z->health = z->health - b->damage;
			if (z->health < 0) {
				cout << "before if statment condidtions" << endl;
				removeZombie(g, z);
				g.score = g.score + 100;
			}
			memcpy(&g.barr[i], &g.barr[g.nbullets - 1], sizeof(Bullet));
			g.nbullets--;
		}
		//this will be for collision with the zombie
		//use the global timer to determine how long between 
		//hits so the zombie doesn't kill instantly
		
		if (g.player1.pos[0] >= z->pos[0] - w &&
			g.player1.pos[0] <= z->pos[0] + w &&
			g.player1.pos[1] <= z->pos[1] + h &&
			g.player1.pos[1] >= z->pos[1] - h) {
			
			double currentTime = g.player1.ptime;
			if (currentTime - z->lastAttack > z->dBuffer) {
				z->zdamage = ZOMBIE_DAMAGE;
				g.player1.health = g.player1.health - z->zdamage;
				z->lastAttack = currentTime;
			}
		}
		z = z->next;
	}
	     //---------------------------------------------------
	     //check keys pressed now
	//     LEFT	    
	if (gl.keys[XK_a]) {
		g.player1.pos[0]-=3.5;
		g.player1.angle = 90;
	}
	//     RIGHT
	if (gl.keys[XK_d]) {
		g.player1.pos[0]+=3.5;
		g.player1.angle = -90;
	}
	//     UP
	if (gl.keys[XK_w]) {
		Flt speed = sqrt(g.player1.vel[0]*g.player1.vel[0]+
					g.player1.vel[1]*g.player1.vel[1]);
		if (speed > 10.0f) {                    
			speed = 10.0f;
			normalize2d(g.player1.vel);
			g.player1.vel[0] *= speed;
			g.player1.vel[1] *= speed;                
		}
		g.player1.pos[1] += 3.5;
	}
	//      DOWN
	if (gl.keys[XK_s]) {
		g.player1.pos[1] -= 3.5;
	}	    
	if (gl.keys[XK_space]) {
		soundTesting(gl, g);
	     //a little time between each bullet
		struct timespec bt;
		clock_gettime(CLOCK_REALTIME, &bt);
		double ts = timeDiff(&g.bulletTimer, &bt);
		if (ts > 0.1) {
			timeCopy(&g.bulletTimer, &bt);
			if (g.nbullets < MAX_BULLETS) {
				//bullet...
	      
				Bullet *b = &g.barr[g.nbullets];
				timeCopy(&b->time, &bt);
				b->pos[0] = g.player1.pos[0];
				b->pos[1] = g.player1.pos[1];
				b->vel[0] = g.player1.vel[0];
				b->vel[1] = g.player1.vel[1];
				//convert player1 angle to radians
				Flt rad = ((g.player1.angle+90.0) / 360.0f) * PI * 2.0;
				//convert angle to a vector
				Flt xdir = cos(rad);
				Flt ydir = sin(rad);
				b->pos[0] += xdir*20.0f;
				//b->pos[1] += ydir*20.0f;
				b->vel[0] += xdir*6.0f + rnd()*0.1;
				b->vel[1] += ydir*6.0f + rnd()*0.1;
				b->color[0] = 1.0f;
				b->color[1] = 1.0f;
				b->color[2] = 1.0f;
				g.nbullets++;
			}
		}
	}
	//zombie being drawn to the player.
	Zombie *z1 = g.znext;
	while (z1) {

		if (z1->pos[0] > g.player1.pos[0]){
			z1->pos[0] -= 2.0;
		}
		if (z1->pos[0] < g.player1.pos[0]){
			z1->pos[0] += 2.0;
		}

		if (z1->pos[1] > g.player1.pos[1]){
			z1->pos[1] -= 1.0;
		}
		if (z1->pos[1] < g.player1.pos[1]){
			z1->pos[1] += 1.0;
		}

		z1 = z1->next;
	}
}
void bug_fix (Game &g) 
{
	   //the bug allowed the player to go outside the boundary of the game
	  //area. once the player got to any of the corners and held the appropriate
	  //keys to go diagnoally, the object would exit the boundary, below is the
	  //solution I found to this issue

	  //*****************bug fix*****************
	
	//top left corner
	while(((g.player1.pos[0] == 16.5) && (g.player1.pos[1] > 315))) {
		g.player1.pos[0] = 20;
		g.player1.pos[1] = 315;    
	}
	
	//top right corner
	while(((g.player1.pos[0] == 1233.5) && (g.player1.pos[1] > 315))) {
		g.player1.pos[0] = 1230;
		g.player1.pos[1] = 315;    
	}
	//bottom left corner
	while(((g.player1.pos[0] == 16.5) && (g.player1.pos[1] < 46.5))) {
		g.player1.pos[0] = 20;
		g.player1.pos[1] = 40;
	}
	//bottom right corner    
	while(((g.player1.pos[0] == 1233.5) && (g.player1.pos[1] < 16.5))) {
		g.player1.pos[0] = 1230;
		g.player1.pos[1] = 10;
	}

}

void soundTesting (Global &gl, Game &g)
{
	//Player *player;
	//double currentTime = g.player1.ptime;
	
	//Get started right here.
	alutInit(0, NULL);
	if (alGetError() != AL_NO_ERROR) {
		printf("ERROR: alutInit()\n");
	}
	//Clear error state.
	alGetError();
	//
	//Setup the listener.
	//Forward and up vectors are used.
	float vec[6] = {0.0f,0.0f,1.0f, 0.0f,1.0f,0.0f};
	alListener3f(AL_POSITION, 0.0f, 0.0f, 0.0f);
	alListenerfv(AL_ORIENTATION, vec);
	alListenerf(AL_GAIN, 1.0f);
	//
	
	//Buffer holds the sound information.
	ALuint alBuffer;
	alBuffer = alutCreateBufferFromFile("./9mm.wav");
	//
	
	//Source refers to the sound.
	ALuint alSource;
	//Generate a source, and store it in a buffer.
	alGenSources(1, &alSource);
	alSourcei(alSource, AL_BUFFER, alBuffer);
	//Set volume and pitch to normal, no looping of sound.
	alSourcef(alSource, AL_GAIN, 1.0f);
	alSourcef(alSource, AL_PITCH, 1.0f);
	alSourcei(alSource, AL_LOOPING, AL_FALSE);
	if (alGetError() != AL_NO_ERROR) {
		printf("ERROR: setting s ource\n");
	}
	for (int i=0; i<1; i++) {
		//if (currentTime + player->lastPlayed > player->sBuffer) {
			alSourcePlay(alSource);
			usleep(25000);
		//}

	}



}
