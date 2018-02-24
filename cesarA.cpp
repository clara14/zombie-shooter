//Cesar Aleman
#include "zlib.h"

extern struct timespec timeStart, timeCurrent;
extern double timeDiff(struct timespec *start, struct timespec *end);
extern void timeCopy(struct timespec *dest, struct timespec *source);
extern void normalize2d(Vec;


void cesar_physics(Global &gl, Game &g)
{
        //end of wall
        if (g.ship.pos[0] < 0.0) {
		g.ship.pos[0] = 0;
        }
        else if (g.ship.pos[0] > (float)gl.xres) {
		g.ship.pos[0] = gl.xres;
        }
        else if (g.ship.pos[1] < 0.0) {
		g.ship.pos[1] = 0;
        }
        else if (g.ship.pos[1] > (float)gl.yres) {
		g.ship.pos[1] = gl.yres;
        }
        //Update bullet positions
        struct timespec bt;
        clock_gettime(CLOCK_REALTIME, &bt);
        int i = 0;
        while (i < g.nbullets) {
                Bullet *b = &g.barr[i];
                //How long has bullet been alive?
                double ts = timeDiff(&b->time, &bt);
                if (ts > 2.5) {
                        //time to delete the bullet.
                        memcpy(&g.barr[i], &g.barr[g.nbullets-1],
                                sizeof(Bullet));
                        g.nbullets--;
                        //do not increment i.
                        continue;
                }
                //move the bullet
                b->pos[0] += b->vel[0];
                b->pos[1] += b->vel[1];
                //window edge collision
                if (b->pos[0] < 0.0) {
                        b->pos[0] += 0;
                }
                else if (b->pos[0] > (float)gl.xres) {
                        b->pos[0] = gl.xres;
                }
                else if (b->pos[1] < 0.0) {
                        b->pos[1] = gl.xres;
                }
                else if (b->pos[1] > (float)gl.yres) {
                        b->pos[1] = gl.xres;
                }
                i++;
        }
        //---------------------------------------------------
        //check keys pressed now
	    //     LEFT
	    //g.ship.angle = 90;
        if (gl.keys[XK_a]) {
	       g.ship.pos[0]-=3.5;
           g.ship.angle = 90;
        }
	    //     RIGHT
        if (gl.keys[XK_d]) {
	       g.ship.pos[0]+=3.5;
           g.ship.angle = -90;
        }
	    //     UP
        if (gl.keys[XK_w]) {

            Flt speed = sqrt(g.ship.vel[0]*g.ship.vel[0]+
                            g.ship.vel[1]*g.ship.vel[1]);
          if (speed > 10.0f) {                    
                speed = 10.0f;
                normalize2d(g.ship.vel);
                g.ship.vel[0] *= speed;
                g.ship.vel[1] *= speed;                
            }
		    g.ship.pos[1]+=3.5;
        }
    //      DOWN
	if (gl.keys[XK_s]) {
		
	    g.ship.pos[1] -= 3.5;
	}	    
	if (gl.keys[XK_space]) {
                //a little time between each bullet
                struct timespec bt;
                clock_gettime(CLOCK_REALTIME, &bt);
                double ts = timeDiff(&g.bulletTimer, &bt);
                if (ts > 0.1) {
                        timeCopy(&g.bulletTimer, &bt);
                        if (g.nbullets < MAX_BULLETS) {
                                //shoot a bullet...
                                //Bullet *b = new Bullet;
                                Bullet *b = &g.barr[g.nbullets];
                                timeCopy(&b->time, &bt);
                                b->pos[0] = g.ship.pos[0];
                                b->pos[1] = g.ship.pos[1];
                                b->vel[0] = g.ship.vel[0];
                                b->vel[1] = g.ship.vel[1];
                                //convert ship angle to radians
                                Flt rad = ((g.ship.angle+90.0) / 360.0f) * PI * 2.0;
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
        
}


