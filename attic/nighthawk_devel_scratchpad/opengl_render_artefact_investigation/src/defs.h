#include <GL/glut.h>
#include <GL/glx.h>
#include "../version.h"

#define STR_LABEL_LEN 40
#define STR_LEN       256
#define STR_LEN_HUGE  1024

#define DISPLAY_DELAY	0.025 /*s*/

#define TRUE		1
#define FALSE		0

#define X              0
#define Y              1
#define Z              2

#define R              0
#define G              1
#define B              2
#define A              3

/*
 * Don't change these defs. They are the original screen dimensions of
 * the game. OpenGL uses these to scale the OpenGL screen to other sizes.
 * JN
 */
#define SCR_ASPECT_43	(4.0 / 3.0)  /*JN, 04SEP20*/
#define SCR_ASPECT_43_TOLERANCE 0.01
#define SCREEN_INSANE_SIZE      10000 /*For screen size discovery. JN, 05SEP20*/
#define SCREEN_SIZE_X  320
#define SCREEN_SIZE_Y  200
#define SCREEN_HSIZE_X (SCREEN_SIZE_X >> 1)
#define SCREEN_HSIZE_Y (SCREEN_SIZE_Y >> 1)
#define SPRITE_SIZE_X  32
#define SPRITE_SIZE_Y  32
#define SPRITE_HSIZE_X (SPRITE_SIZE_X >> 1)
#define SPRITE_HSIZE_Y (SPRITE_SIZE_Y >> 1)
