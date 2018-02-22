#include "zlib.h"

extern struct timespec timeStart, timeCurrent;
extern double timeDiff(struct timespec *start, struct timespec *end);
extern void timeCopy(struct timespec *dest, struct timespec *source);
void normalize2d(Vec);


void physics(Global &gl, Game &g)
{
        //Update ship position
        if (g.ship.pos[1] > 100) {
                g.ship.pos[0] += g.ship.vel[0];
                g.ship.pos[1] += g.ship.vel[1];
        }
        //Check for collision with window edges
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
        //
        //Update bullet positions
        struct timespec bt;
        clock_gettime(CLOCK_REALTIME, &bt);
        int i=0;
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
                //Check for collision with window edges
                if (b->pos[0] < 0.0) {
                        //b->pos[0] += (float)gl.xres;
                        b->pos[0] += 0;
                }
                else if (b->pos[0] > (float)gl.xres) {
                        //b->pos[0] -= (float)gl.xres;
                        b->pos[0] = gl.xres;
                }
                else if (b->pos[1] < 0.0) {
                        //b->pos[1] += (float)gl.yres;
                        b->pos[1] = gl.xres;
                }
                else if (b->pos[1] > (float)gl.yres) {
                        //b->pos[1] -= (float)gl.yres;
                        b->pos[1] = gl.xres;
                }
                i++;
        }


        //---------------------------------------------------
        //check keys pressed now
	//       LEFT
	//g.ship.angle = -90;
        if (gl.keys[XK_a]) {
	   /* 
                g.ship.angle += 4.0;
                if (g.ship.angle >= 360.0f)
                        g.ship.angle -= 360.0f;
	   */
	    g.ship.pos[0]-=3.5;
        }
	//       RIGHT
        if (gl.keys[XK_d]) {
	   /* 
                g.ship.angle -= 4.0;
                if (g.ship.angle < 0.0f)
                        g.ship.angle += 360.0f;
           */

	    g.ship.pos[0]+=3.5;
        }
	//       UP
        if (gl.keys[XK_w]) {
                //apply thrust
                //convert ship angle to radians
                //Flt rad = ((g.ship.angle+90.0) / 360.0f) * PI * 2.0;
                //convert angle to a vector
                //Flt xdir = cos(rad);
                //Flt ydir = sin(rad);
                //g.ship.vel[0] += xdir*0.02f;
                //g.ship.vel[1] += ydir*0.02f;

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
	if(gl.keys[XK_s]) {
		
	    g.ship.pos[1]-=3.5;
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
        if (g.mouseThrustOn) {
                //should thrust be turned off
                struct timespec mtt;
                clock_gettime(CLOCK_REALTIME, &mtt);
                double tdif = timeDiff(&mtt, &g.mouseThrustTimer);
                if (tdif < -0.3)
                        g.mouseThrustOn = false;
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
                        return 1;
                case XK_w:
                        break;
                case XK_a:
                        break;
                case XK_s:
                        break;
                case XK_d:
                        break;
                case XK_equal:
                        break;
                case XK_minus:
                        break;
        }
        return 0;
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

