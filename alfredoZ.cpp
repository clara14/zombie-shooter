#include "zlib.h"

void hud(Global &gl, Game &g) {
	Rect r;
        glClear(GL_COLOR_BUFFER_BIT);
        //
        r.bot = gl.yres - 20;
        r.left = 10;
        r.center = 0;
        ggprint8b(&r, 16, 0x00ff0000, "-------------Player1 HUD-------------");
        ggprint8b(&r, 16, 0x00ffff00, "| \t PlayerName: ");
        //printf(" %s ", g.player1.name);
        ggprint8b(&r, 16, 0x00ffff00, "| \t Health: %i", g.player1.health);
        ggprint8b(&r, 16, 0x00ffff00, "| \t Weapon: 9mm Handgun");
        ggprint8b(&r, 16, 0x00ff0000, "---------------------------------------");
}

