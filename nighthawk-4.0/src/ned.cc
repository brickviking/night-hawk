/***************************************************************************
****************************************************************************
****************************************************************************
* Nighthawk Copyright (C) 1997 Jason Nunn
* Nighthawk is under the GNU General Public License.
*
* Sourceforge admin: Eric Gillespie
* night-hawk.sourceforge.net
*
* See README file for a list of contributors.
*
* ----------------------------------------------------------------
* ned - The floor editor (restored in 4.0. JN, 27SEP20)
*
****************************************************************************
****************************************************************************
***************************************************************************/
extern "C" {
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <AL/al.h>
#include <AL/alc.h>
#include "defs.h"
#include "misc_externs.h"
#include "sprite_loader_externs.h"
#include "opengl_externs.h"
}
#include "tedit_floor.h"

char			floor_relpath[STR_LEN],
			floor_name[STR_LABEL_LEN + 1];

static	int		new_f;
static	int		map_x_size = 2, map_y_size = 2;
static tedit_floor	edit_floor;

/***************************************************************************
*
***************************************************************************/
static void ned_draw(void)
{
	edit_floor.draw();
}

static void ned_calc(void)
{
	edit_floor.bg_calc();
}

/***************************************************************************
*
***************************************************************************/
static void init(int argc, char *argv[])
{
	verbose_logging = TRUE;
	srandom(12338);
	umask(~0666); // Set file permissions for files created by this game.

	create_filepaths();

	if (verbose_logging == TRUE)
		printf(	"      Data path: %s\n"
			"     Score path: %s\n"
			"  Preserve path: %s\n"
			"  Display Delay: %d ms\n",
			data_path, scores_path, preserve_path, display_delay);

	snprintf(floor_relpath, STR_LEN, "/%s/%s/%s",
		fleet_start, ship_start, floor_name);

	init_kb_event();
	init_background_timer(); /*realtime game calcs*/

	opengl_init(argc, argv);
	glutSetCursor(GLUT_CURSOR_INHERIT); // Requested by ECG. JN, 02OCT20
	load_flr_xpms();
	load_sprite_xpms();
	load_font();

	white_pixel[0][R] = 1.0;
	white_pixel[0][G] = 1.0;
	white_pixel[0][B] = 1.0;

	if (new_f)
		edit_floor.create(map_x_size, map_y_size);
	else
		edit_floor.load_floor_map(rel_to_abs_filepath(floor_relpath));

	/*
	 * kick things off.. Run the intro scene.
	 */
	draw_hook = ned_draw;
	bg_calc_hook = ned_calc;
}

/***************************************************************************
*
***************************************************************************/
static void print_help_and_quit(void)
{
	printf(
"\n"
"Usage: nighthawk_ned <options>\n"
" -h or -?                     Display this help info.\n"
" -u <ms>                      Display Delay (1 to 100ms).\n"
" -d <filepath>                Specify your own game data filepath.\n"
" -s <filepath>                Specify/override user account path (ie: $HOME).\n"
" -n <Width> <Height>\n"
"    <Default floor tile>\n"
"    <Fleet name> <Ship name> <Floor map>  New floor map.\n"
" -e <Fleet name> <Ship name> <floor name> Edit floor map.\n"
"\n");
	exit(0);
}

static void parse_parameters(int argc, char *argv[])
{
	for (int i = 1; i < argc; i++) {
		if (argv[i][0] != '-')
			continue;

		switch (argv[i][1]) {
			case 'h':
			case '?': /*Requested by ECG. JN, 02OCT20*/
			print_help_and_quit();

			case 'u':
			if ((i + 1) == argc) {
				printf("Error: Missing display delay parameter.\n");
				print_help_and_quit();
			}
			display_delay = abs(atoi(argv[i + 1]));
			if (display_delay < 1) display_delay = 1;
			if (display_delay > 1000) display_delay = 1000;
			i++;
			break;

			case 'd':
			if ((i + 1) == argc) {
				printf("Error: Missing data filepath.\n");
				print_help_and_quit();
			}

			if (!(argv[i + 1][0] == '/' || argv[i + 1][0] == '.'
						|| argv[i + 1][0] == '~')) {
				printf("Error: Garbage in data filepath.\n");
				print_help_and_quit();
			}

			data_path = alloc_mem_char(strlen(argv[i + 1]) + 1);
			strcpy(data_path, argv[i + 1]);
			i++;
			break;

			case 'n':
			if ((i + 6) >= argc) {
				printf("Error: Not enough '-n' parameters.\n");
				print_help_and_quit();
			}
			new_f = 1;
			sscanf(argv[i + 1],"%d", &map_x_size);
			sscanf(argv[i + 2],"%d", &map_y_size);
			sscanf(argv[i + 3],"%d", &edit_floor.default_map_value);
			strncpy2(fleet_start, argv[i + 4], SCORES_FLEET_LEN);
			strncpy2(ship_start, argv[i + 5], SCORES_HSHIP_LEN);
			strncpy2(floor_name, argv[i + 6], STR_LABEL_LEN);
			i += 6;
			break;

			case 'e':
			if ((i + 3) >= argc) {
				printf("Error: Not enough '-e' parameters.\n");
				print_help_and_quit();
			}
			strncpy2(fleet_start, argv[i + 1], SCORES_FLEET_LEN);
			strncpy2(ship_start, argv[i + 2], SCORES_HSHIP_LEN);
			strncpy2(floor_name, argv[i + 3], STR_LABEL_LEN);
			i += 3;
			break;
		}
	}

	if (fleet_start[0] == 0) {
		printf("Fleet name not specified.\n");
		exit(0);
	}

	if (ship_start[0] == 0) {
		printf("Ship and floor name not specified.\n");
		exit(0);
	}
}

/***************************************************************************
* ned main loop
***************************************************************************/
int main(int argc, char *argv[])
{
	printf(WELCOME);
	parse_parameters(argc, argv);
	init(argc, argv);
	if (verbose_logging == TRUE)
		printf("Entering floor editor..\n");
	glutMainLoop();
	end_game(0);
	return 0;
}
