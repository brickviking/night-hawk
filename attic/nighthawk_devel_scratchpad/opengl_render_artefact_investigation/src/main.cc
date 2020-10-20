/*
 * OpenGL Glitch fault find
 * Jason Nunn
*/
extern "C" {
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <math.h>
#include "defs.h"
#include "misc_externs.h"
#include "opengl_externs.h"
}

/***************************************************************************
* This game state is the static noise you see when you get killed.
***************************************************************************/
static void game_noise_draw(void)
{
  int x, y;

  for(y = 0; y < 200; y += 10)
  {
    for(x = 0; x < 320; x += 10)
    {
//      glColor3fv(white_pixel[random() & 0x7]);
      glColor4f(1.0, 1.0, 1.0, ((float)(random() & 0x7)) / 8.0);
      blit_rect(x, y, 10, 10);
    }
  }
}

void game_noise_init(void)
{
	draw_hook = game_noise_draw;
//	set_background_hook(game_noise_calc); // JN, 03SEP20
}

/***************************************************************************
*
***************************************************************************/
static void init(int argc, char *argv[])
{
	srandom(12338);
	opengl_init(argc, argv);

        /*
         * init colours
         */
        ramp_colour(
                0.4375, 0.4375, 0.4375, 1.0   ,    1.0,    1.0,
                (GLfloat *)white_pixel, 8);

	game_noise_init();
}

/***************************************************************************
*
***************************************************************************/
static void print_help_and_quit(void)
{
	printf(
"\n"
"Usage: nighthawk <options>\n"
" -h             Display this help info.\n"
" -u <s>         Display Delay (seconds) (Developer function).\n"
" -v             Verbose logging (Developer function).\n"
"\n"
	);

	exit(0);
}

/***************************************************************************
*
***************************************************************************/
int main(int argc, char *argv[])
{
	for (int i = 1; i < argc; i++) {
		int i2;

		if (argv[i][0] != '-') // JN, 04SEP20
			continue;

		i2 = i + 1;
		switch (argv[i][1]) {
			case 'h':
			print_help_and_quit();

			case 'u':
			if (i2 == argc) {
				printf("Error: Missing display delay parameter.\n");
				print_help_and_quit();
			}
			display_delay = fabs(atof(argv[i2]));
			if (display_delay < 0.001)
				display_delay = 0.001;
			break;

			case 'v':
			verbose_logging = TRUE; // JN, 23AUG20
			break;
		}
	}

	init(argc, argv);
	printf("Entering Glitch..\n");
	glutMainLoop();
	end_game(0);
	return 0;
}
