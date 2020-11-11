/***************************************************************************
****************************************************************************
****************************************************************************
*
* Nighthawk Copyright (C) 1997 Jason Nunn
* Nighthawk is under the GNU General Public License.
*
* Sourceforge admin: Eric Gillespie
* night-hawk.sourceforge.net
*
* See README file for a list of contributors.
*
* ----------------------------------------------------------------
* Main
*
****************************************************************************
****************************************************************************
***************************************************************************/
extern "C" {
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <AL/al.h>
#include <AL/alc.h>
#include "defs.h"
#include "misc_externs.h"
#include "sprite_loader_externs.h"
#include "opengl_externs.h"
#include "scores_externs.h"
#include "openal_externs.h"
}
#include "tship.h"
#include "intro_externs.h"
#include "game_externs.h"

/***************************************************************************
*
***************************************************************************/
static void init(int argc, char *argv[])
{
	struct sigaction kill_action, segfault_action;

	kill_action.sa_handler = kill_proc;
	sigemptyset(&kill_action.sa_mask);
	kill_action.sa_flags = 0;

	segfault_action.sa_handler = seg_fault;
	sigemptyset(&segfault_action.sa_mask);
	segfault_action.sa_flags = 0;

	/*
	 * signal() is deprecated. See man 2 signal. using POSIX sigaction()
	 * instead. JN, 02OCT20
	 */
	sigaction(SIGINT, &kill_action, NULL);
	sigaction(SIGKILL, &kill_action, NULL);
	sigaction(SIGTERM, &kill_action, NULL);
	sigaction(SIGHUP, &kill_action, NULL);
	sigaction(SIGQUIT, &kill_action, NULL); // Added. JN, 28OCT20
	sigaction(SIGABRT, &kill_action, NULL); // Added. JN, 28OCT20
	sigaction(SIGSEGV, &segfault_action, NULL);

	/*
	 * Add atexit to fix close window bug. See notes in misc.c. JN, 28OCT20
	 */
	if (atexit(atexit_h) != 0)
		printf_error("Error running %s:atexit()", __func__);

	srandom(12338);
	umask(~0666); // Set file permissions for files created by this game.

	create_filepaths();

	if (verbose_logging == TRUE)
		printf(	"      Data path: %s\n"
			"     Score path: %s\n"
			"  Preserve path: %s\n"
			"  Display Delay: %d ms\n",
			data_path, scores_path, preserve_path, display_delay);

	load_fleet_table();
	init_scores();
	load_scores();
	if (verbose_logging == TRUE)
		dump_scores();
	load_preserved_session();

	init_kb_event();
	init_background_timer(); /*realtime game calcs*/

	opengl_init(argc, argv);
	load_fleet_logos();
	load_flr_xpms();
	load_sprite_xpms();
	load_font();

	/*
	 * init colours
	 */
	ramp_colour(
		0.4375, 0.4375, 0.4375, 1.0   ,    1.0,    1.0,
		(GLfloat *)white_pixel, 8);

	ramp_colour(
		0.0   , 0.0   , 0.6,    0.0   ,    0.0,    1.0,
		(GLfloat *)blue_pixel, 8);

	ramp_colour(
		0.4   , 1.0   , 0.4   , 0.0   ,    0.5,    0.0,
		(GLfloat *)green_pixel, MAX_SCORE_TABLE);

	ramp_colour(
		1.0   , 0.5   , 0.5   , 0.9375, 0.0625, 0.0625,
		(GLfloat *)red_pixel, MAX_SCORE_TABLE);

	init_sound();

	/*
	 * kick things off.. Run the intro scene.
	 */
	intro_start_init();
}

/***************************************************************************
* Cmd line processing and cmd line help.
*
* I've deleted the developer options in help. Users don't need to know/
* won't care about them. Keeps the man page simple etc. JN, 14OCT20
***************************************************************************/
static void print_help_and_quit(void)
{
	printf(
"\n"
"Usage: nighthawk <options>\n"
" -h or -?        Display this help info.\n"
" -f <Fleet name> Start at a given fleet.\n"
" -c <Ship name>  Start at a given ship.\n"
"\n"
	);
	exit(0);
}

static void parse_parameters(int argc, char *argv[])
{
	for (int i = 1; i < argc; i++) {
		int i2;

		if (argv[i][0] != '-') // JN, 04SEP20
			continue;

		i2 = i + 1;
		switch (argv[i][1]) {
			case 'h':
			case '?': /*Requested by ECG. JN, 02OCT20*/
			print_help_and_quit();

			case 'u': // Undocumented developer cmd
			if (i2 == argc) {
				printf("Error: Missing display delay parameter.\n");
				print_help_and_quit();
			}
			display_delay = abs(atoi(argv[i2]));
			if (display_delay < 1) display_delay = 1;
			if (display_delay > 1000) display_delay = 1000;
			i = i2;
			break;

			case 'f':
			if (i2 == argc) {
				printf("Error: Missing ship filepath.\n");
				print_help_and_quit();
			}
			strncpy2(fleet_start, argv[i2], SCORES_FLEET_LEN);
			i = i2;
			printf("Selected fleet '%s'.\n", fleet_start);
			i = i2;
			break;

			case 'c': //Undocumented developer feature
			if (i2 == argc) {
				printf("Error: Missing ship filepath.\n");
				print_help_and_quit();
			}
			strncpy2(ship_start, argv[i2], SCORES_HSHIP_LEN);
			ship_start_cmdline_f = TRUE;
			i = i2;
			printf(
"Starting at ship '%s'. (Nb/ High scores will indicate that you have cheated)\n",
					ship_start);
			i = i2;
			break;

			case 'g': // Undocumented developer cmd
			god_mode = TRUE;
			break;

			case 'd': // This was requested by Eric. JN, 06SEP20
			if (i2 == argc) {
				printf("Error: Missing data filepath.\n");
				print_help_and_quit();
			}
			data_path = alloc_mem_char(strlen(argv[i2]) + 1);
			strcpy(data_path, argv[i2]);
			i = i2;
			break;

			case 's': // Undocumented developer cmd
			if (i2 == argc) {
				printf("Error: Missing scores filepath.\n");
				print_help_and_quit();
			}
			home_override_path = alloc_mem_char(strlen(argv[i2]) + 1);
			strcpy(home_override_path, argv[i2]);
			i = i2;
			break;

			case 'v': // Undocumented developer cmd
			verbose_logging = TRUE; // JN, 23AUG20
			break;
		}
	}
}

/***************************************************************************
*
***************************************************************************/
int main(int argc, char *argv[])
{
	printf(WELCOME);

	printf(
"\n****************************************************************************\n"
"NOTICE: This game was developed in the era of CRT monitors and has only\n"
"recently been ported for modern hardware. Therefore, the game's screen aspect\n"
"is 4:3. This game enforces this aspect whenever the game window is resized.\n"
"However, if the game window is maximised, then the game has no control over\n"
"the window size, and the operation of the game will become indeterminate. This\n"
"issue will be addressed in future versions, however for the time being, please\n"
"avoid maximising window of the game.\n"
"****************************************************************************\n\n");

	parse_parameters(argc, argv);
	init(argc, argv);
	if (verbose_logging == TRUE)
		printf("Entering Nighthawk..\n");
	glutMainLoop();
	end_game(0);
	return 0;
}
