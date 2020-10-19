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
* intro.cc - Game intro routines
*
****************************************************************************
****************************************************************************
***************************************************************************/
extern "C" {
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <GL/glut.h>
#include <GL/glx.h>
#include "defs.h"
#include "linked_data_externs.h"
#include "misc_externs.h"
#include "sprite_loader_externs.h"
#include "opengl_externs.h"
#include "scores_externs.h"
}
#include "tship.h"
#include "game_externs.h"

static int	intro_count, // Fixed. intro_count. JN, 08SEP20
		intro_sequence_no,
		intro_back_bm_no;

void intro_start_reinit(void);

/***************************************************************************
* Put into it's own function as used by more than one function. JN, 06SEP20
***************************************************************************/
static void reset_game(void)
{
	if (verbose_logging == TRUE)
		printf("Running reset_game().\n");

	sscore = score = last_score = 0;
	free_ship_table();
	load_ship_table();
	if (ship_start[0])
		ship_ptr = find_entry_in_ship_table(ship_start);
	else
		ship_ptr = 0;
}

/***************************************************************************
* New v4 function. Fleet select. Called only once as the start of the
* game. JN, 08SEP20
***************************************************************************/
#define FS_TITLE_Y	5
#define FS_INSTRCT	(SCREEN_SIZE_Y - 30)
#define FS_ARROWS_X_OFS	15
#define FS_ARROWS_Y	70
#define FS_LOGO_Y	30
#define FS_LOGO_X_START	40
#define FS_LOGO_X_SPC	120
#define FS_COMMENT_Y	135
#define FS_LINE_WIDTH	4
#define FS_LOGO_SIZE	100

static int flt_p, flt_hl;
static float flashing_alpha;

static void fleet_select_calc(void)
{
	flashing_alpha += DEG2RAD(9.0);
	flashing_alpha = fmodf(flashing_alpha, 2.0 * M_PI);
}

static void fleet_selection(int ptr)
{
	if (ptr >= fleet_table_size)
		return;

	fleet_ptr = &fleet_table[ptr];
	if (verbose_logging == TRUE)
		printf("Selected fleet %s.\n", fleet_ptr->name);
	reset_game();
	intro_start_reinit();
}

static void fleet_select_draw(void)
{
	const char
		*mess1 = "Welcome to Nighthawk. Select mission:\n",
		*mess2 = "Mouse/Space to select, 'q' to Quit.\n";
	int i;
	float alpha_pulse;

	alpha_pulse = 0.5 * fabsf(sinf(flashing_alpha));
	alpha_pulse += 0.5;

	glColor3f(0.0, 0.8, 0.0);
	display_message((char *)mess1, FS_TITLE_Y);

	glColor3f(0.0, 0.3, 0.0);
	display_message((char *)mess2, FS_INSTRCT);

	glColor3f(0.3, 0.3, 0.3);
	print_str_len((char *)"<", 1, FS_ARROWS_X_OFS, FS_ARROWS_Y);
	print_str_len((char *)">", 1,
		SCREEN_SIZE_X - FS_ARROWS_X_OFS - FONT_SIZE_X, FS_ARROWS_Y);

	for (i = 0; i < 2; i++) {
		if (i + flt_p >= fleet_table_size)
			continue;

		glColor3fv(white_pixel[7]);
		blit_bm_scale(
			&fleet_table[i + flt_p].logo,
			i * FS_LOGO_X_SPC + FS_LOGO_X_START,
			FS_LOGO_Y,
			-2);
		if (i == flt_hl) {
			glColor4f(1.0, 1.0, 1.0, alpha_pulse);
			blit_rect_hollow(
				i * FS_LOGO_X_SPC + FS_LOGO_X_START - (FS_LINE_WIDTH >> 1),
				FS_LOGO_Y - (FS_LINE_WIDTH >> 1),
				FS_LOGO_SIZE + FS_LINE_WIDTH,
				FS_LOGO_SIZE + FS_LINE_WIDTH);
		}

		glColor3f(0.4, 0.4, 0.4);
		print_str(fleet_table[i + flt_p].comment,
			i * FS_LOGO_X_SPC + (FS_LOGO_X_START >> 1),
			FS_COMMENT_Y);

	}

	/*
	 * Pull input events of the tkevent buffer
	 */
	for (;;) {
		tkevent k;

		if (!get_kb_event(&k))
			break;

		if (k.type == KEYBOARD_EVENT) {
			switch (tolower(k.val)) {
				case KEY_SELECT:
				fleet_selection(flt_p + flt_hl);
				break;

				case KEY_QUIT:
				end_game(0);
			}

		} else if (k.type == SPECIAL_EVENT) {
			switch(k.val) {
				case GLUT_KEY_LEFT:
				flt_hl--;
				if (flt_hl < 0) {
					flt_hl = 0;

					flt_p--;
					if (flt_p < 0)
						flt_p = 0;
				}
				break;

				case GLUT_KEY_RIGHT:
				if (flt_p + flt_hl >= (fleet_table_size - 1))
					break;
				flt_hl++;
				if (flt_hl > 1) {
					flt_hl = 1;
					flt_p++;
				}
				break;
			}
		} else if (k.type == MOUSE_EVENT) {
			if (k.val == GLUT_LEFT_BUTTON) {
				for (int i = 0; i < 2; i++) {
					if (test_mouse_pos(
							&k,
							i * FS_LOGO_X_SPC + FS_LOGO_X_START - (FS_LINE_WIDTH >> 1),
							FS_LOGO_Y - (FS_LINE_WIDTH >> 1),
							FS_LOGO_SIZE + FS_LINE_WIDTH,
							FS_LOGO_SIZE + FS_LINE_WIDTH)) {
						fleet_selection(flt_p + i);
						break;
					}
				}
			}
		}
	}
}

void fleet_select_init(void)
{
	/*
	 * If fleet is specified on cmdline, then load fleet based on
	 * cmdline setting.
	 */
	if (fleet_start[0]) {
		printf("Fleet specified. Skipping fleet selection screen.\n");
		find_fleet_entry(fleet_start);
		reset_game();
		intro_start_reinit();
		return;
	}

	if (verbose_logging == TRUE)
		printf("Running fleet_select_init().\n");

	glLineWidth(FS_LINE_WIDTH);
        draw_hook = fleet_select_draw;
	bg_calc_hook = fleet_select_calc;
}

/***************************************************************************
* New v4 function. Game continuance. Called only once as the start of the
* game. JN, 08SEP20
***************************************************************************/
#define	GAME_CONT_NOT_SEL	0
#define	GAME_CONT_RESTART	1
#define	GAME_CONT_CONT		2

static int	game_cont_exit;
static float	game_cont_fade;

static void game_cont_calc(void)
{
	if (game_cont_exit == GAME_CONT_NOT_SEL)
		return;

	game_cont_fade -= 0.1;
	if (game_cont_fade < 0.0)
		game_cont_fade = 0.0;
}

static void game_cont_draw(void)
{
	const char *mess =
		"You have already played Nighthawk.\n"
		"Would you like to continue from\n"
		"where you left ? (y/n)\n";
	char str[STR_LEN + 1];

	glColor4f(0.0, 0.7, 0.0, game_cont_fade);
	display_message((char *)mess, 20);
	snprintf(str, STR_LEN,
			"Name: %s\n"
			"Ship completed: %s\n"
			"Score: %u\n"
			"Date: %s\n",
			preserved_session->name,
			preserved_session->highest_ship,
			preserved_session->score,
			preserved_session->time);
	glColor4f(0.0, 1.0, 0.0, game_cont_fade);
	display_message(str, 80);

	/*
	 * Pull input events of the tkevent buffer
	 */
	for (;;) {
		tkevent k;

		if (!get_kb_event(&k))
			break;

		if (k.type != KEYBOARD_EVENT)
			continue;

		switch (tolower(k.val)) {
			case KEY_YES:
			game_cont_exit = GAME_CONT_CONT;
			return;

			case KEY_NO:
			game_cont_exit = GAME_CONT_RESTART;
			return;

			case KEY_QUIT:
			end_game(0);
		}
	}

	/*
	 * If screen is fading, return..
	 */
	if (game_cont_fade != 0.0)
		return;

	switch (game_cont_exit) {
		case GAME_CONT_RESTART:
		fleet_select_init();
		break;

		case GAME_CONT_CONT:
		sscore = score = last_score = preserved_session->score;
		find_fleet_entry(preserved_session->fleet);
		strncpy2(ship_start, preserved_session->highest_ship, SCORES_HSHIP_LEN);
		load_ship_table();
		ship_ptr = find_entry_in_ship_table(ship_start);
		intro_start_reinit();
		break;
	}

	/*
	 * free and clear preserved session, so that continuance screen
	 * isn't run again when intro_start_init() is executed again.
	 * JN, 02OCT20
	 */
	free(preserved_session);
	preserved_session = NULL;
}

static void game_cont_init(void)
{
	if (verbose_logging == TRUE)
		printf("Running game_cont_init().\n");

	game_cont_exit = GAME_CONT_NOT_SEL;
	game_cont_fade = 1.0;
        draw_hook = game_cont_draw;
	bg_calc_hook = game_cont_calc;
}

/***************************************************************************
* Pause calculation background functions
***************************************************************************/
static void pause_calc_sub(void)
{
	intro_count = 0;
	intro_sequence_no++;
	intro_back_bm_no = random() & 0x3;
}

static void pause_1_calc(void)
{
	intro_count++;  // JN, 03SEP20
	if (intro_count > 73)
		pause_calc_sub();
}

static void pause_2_calc(void)
{
	intro_count++; // JN, 03SEP30
	if (intro_count > 35)
		pause_calc_sub();
}

static void pause_3_calc(void)
{
	intro_count++; // JN, 03SEP30
	if (intro_count > 80)
		pause_calc_sub();
}

/*
 * New 4.0. JN, 14SEP20
 */
static void pause_4_calc(void)
{
	intro_count++;
	if (intro_count > 125)
		pause_calc_sub();
}

/***************************************************************************
* Converted strings from static char * to const char * as requested by g++
* JN, 29AUG20
***************************************************************************/
static void credit_1_fg(void)
{
	const char
	*mess1 = "Based on Andrew Braybrook's",
	*mess2 = "Paradroid.";

	print_str_hazey((char *)mess1, 56, 44, intro_haze[0], intro_haze[1]);
	print_str_hazey((char *)mess2, 56, 60, intro_haze[0], intro_haze[1]);
}

static void credit_2_fg(void)
{
	const char
	*mess1 = "By Jason Nunn",
	*mess2 = "(and contributors)";

	print_str_hazey((char *)mess1, 108, 44, intro_haze[0], intro_haze[1]);
	print_str_hazey((char *)mess2, 90, 60, intro_haze[0], intro_haze[1]);
}

static void main_logo_fg(void)
{
	const char *mess1 = "Copyright \"1997 Jason Nunn (JsNO)";
	const char *mess2 = "night-hawk.sourceforge.net";
	const char *mess3 = "Rescue from Vega (C) 1998 Vincent Voois";
	const char *mess4 = "www.vincentvoois.com";

	glColor4fv(bm_colour);
	blit_bm(&credit_6_bm, SCREEN_HSIZE_X - (credit_6_bm.size_x >> 1), 20);
	print_str_hazey((char *)mess1, 30, 55, intro_haze[0], intro_haze[1]);
	print_str_hazey((char *)mess2, 52, 76, intro_haze[0], intro_haze[1]);
	print_str_hazey((char *)mess3, 3, 107, intro_haze[0], intro_haze[1]);
	print_str_hazey((char *)mess4, 80, 128, intro_haze[0], intro_haze[1]);
}

static void main_logo_fg2(void)
{
	const char *mess = "Space to play, 'q' to Quit.\n";

	main_logo_fg();

	glColor3fv(white_pixel[2]);
	display_message((char *)mess, 160);
}

static void credit_6_fg(void)
{
  main_logo_fg();
}

static void credit_7_fg(void)
{
  main_logo_fg2();
}

/***************************************************************************
* Optimised this code while looking for an unrelated bug. JN, 09SEP20
***************************************************************************/
#define SCROLL_LIMIT 1730
static int scroll_story_len;

static void draw_scroll_calc_init(void)
{
  intro_count = -(210 << 2);
  intro_sequence_no++;
  scroll_story_len = strlen((char *)fleet_ptr->scroll_story); // JN, 09SEP20
}

static void draw_scroll_calc(void)
{
  intro_count += 3; // Back to 2.2 timing. JN, 03SEP30
  if(intro_count > SCROLL_LIMIT)
  {
    intro_count = 0;
    intro_sequence_no++;
    intro_back_bm_no = 0;
  }
}

static void draw_scroll_fg(void)
{
  int sic;
  int i, a, b, c, p, xp;

  sic = intro_count;
  i = sic >> 2; // Fix. JN, 14SEP20
  b = i / 16;
  c = i % 16;
  for(a = 0; a < 16; a++)
  {
    p = (a + b) * 36;
    if((p >= 0) && (p < scroll_story_len)) // JN, 09SEP20
    {
      xp = (a << 4) - c;

      glColor3f(0.0, (100 - (GLfloat)abs(100 - xp)) / 100, 0.0);
      print_str_len((char *)fleet_ptr->scroll_story + p, 36, 16, xp - 8);
    }
  }
}

/***************************************************************************
* Updated. JN, 30AUG20
***************************************************************************/
static void credit_8_fg(void)
{
	int		sibbn;
	const char
	*mess1 =	"= Contributors =\n",
	*mess2 =	"Vincent Voois (Music)\n"
			"Adrian Bridgett (Code)\n"
			"Eero Tamminen (Manual)\n"
			"Wolfgang Scherer (Code)\n"
			"Nelson Minar (Code)\n"
			"Rafael Laboissiere (Code)\n"
			"Tanel Kulaots (Code)\n"
			"Eric Gillespie (Various)\n";

	sibbn = intro_back_bm_no; // JN, 14SEP20

	glColor4fv(bm_colour);
	blit_bm(&intro_back_bm[sibbn], 0, 0);

	glColor3fv(yellow_pixel[0]);
	display_message((char *)mess1, 20);

	glColor3fv(white_pixel[5]);
	display_message((char *)mess2, 40);
}

/***************************************************************************
*
***************************************************************************/
static void contacts_1_fg(void)
{
  int sibbn;
  const char *mess1 = "=GPL=\n",
             *mess2 =
    "\n"
    "Nighthawk   comes   under  the   GNU\n"
    "general public license agreement,and\n"
    "comes  with  ABSOLUTELY NO WARRANTY.\n"
    "This is free software,  and  you are\n"
    "welcome  to  redistribute  it  under\n"
    "certain conditions.\n";

  sibbn = intro_back_bm_no; // JN, 14SEP20

  glColor4fv(bm_colour);
  blit_bm(&intro_back_bm[sibbn], 0, 0);

  glColor3fv(yellow_pixel[0]);
  display_message((char *)mess1, 20);

  glColor3fv(white_pixel[5]);
  display_message((char *)mess2, 40);
}

static void contacts_2_fg(void) // updated. JN, 30AUG20
{
	int		sibbn;
	const char
	*mess1 =	"-Repositories-\n",
	*mess2 =	"night-hawk.sourceforge.net\n"
			"github.com/brickviking/night-hawk\n"
			"Eric Gillespie (BrickViking)\n",
	*mess3 =	"-Author-\n",
	*mess4 =	"Jason Nunn (JsNO)\n"
			"jsno8192@gmail.com\n";

	sibbn = intro_back_bm_no; // JN, 14SEP20

	glColor4fv(bm_colour);
	blit_bm(&intro_back_bm[sibbn], 0, 0);

	glColor3fv(yellow_pixel[0]);
	display_message((char *)mess1, 20);
	display_message((char *)mess3, 91);

	glColor3fv(white_pixel[5]);
	display_message((char *)mess2, 36);
	display_message((char *)mess4, 107);
}

static void contacts_4_fg(void) // Updated. JN, 30AUG20
{
	int	sibbn;
	const char *mess =
		"Thanks to the  many  people who have\n"
		"emailed  feedback  about  Nighthawk.\n"
		"Support Free Software !\n"
		"\n"
		"Thanks also to Sourceforge.net\n"
		"for hosting this fantastic project\n"
		"and many others.\n"
		"\n"
		"Hit Space to Start.\n";

	sibbn = intro_back_bm_no; // JN, 14SEP20

	glColor4fv(bm_colour);
	blit_bm(&intro_back_bm[sibbn], 0, 0);

	glColor3fv(white_pixel[5]);
	display_message((char *)mess, 10);
}

/***************************************************************************
*
***************************************************************************/
static void display_scores_calc(void)
{
  intro_count++;  // JN, 03SEP30
  if(intro_count > 80)
  {
    intro_count = 0;
    intro_sequence_no++;
    intro_back_bm_no = random() & 0x3;
  }
}

static void display_scores_fg(void)
{
  int sibbn;
  char str[STR_LEN + 1];
  int  x, yp;

  sibbn = intro_back_bm_no; // JN, 14SEP20

  glColor4fv(bm_colour);
  blit_bm(&intro_back_bm[sibbn], 0, 0);
  blit_bm(&ntitle_bm, SCREEN_HSIZE_X - (ntitle_bm.size_x >> 1),5);

  snprintf(str, STR_LEN, "%-8s %-15s    Score", "Name", "Ship");
  glColor3fv(red_pixel[0]);
  print_str(str, 20, 30);

  for(x = 0, yp = 50; x < MAX_SCORE_TABLE; x++, yp += 16)
  {
    snprintf(str, STR_LEN, "%-8s %-15s %8u",
      score_table[x].name,
      score_table[x].highest_ship,
      score_table[x].score);

    glColor3fv(green_pixel[x]);
    print_str(str, 20, yp);
  }
}

/***************************************************************************
* OpenGL has a feature where it will suspect the glutDisplayFunc() hook.
* Therefore, you can't have any background or sequence execution in this
* context. See below table. I moved draw_scroll_calc_init() to background
* context. JN, 09SEP20
***************************************************************************/
static sequence_tab_t intro_tab[] = {
  {pause_4_calc,        NULL},
  {pause_1_calc,        credit_1_fg},
  {pause_2_calc,        NULL},
  {pause_1_calc,        credit_2_fg},
  {pause_2_calc,        NULL},
  {pause_1_calc,        credit_6_fg},
  {pause_1_calc,        credit_7_fg},
  {pause_1_calc,        credit_7_fg},
  {draw_scroll_calc_init, NULL}, /*restart. Modified. JN, 09SEP20*/
  {draw_scroll_calc,    draw_scroll_fg},
  {pause_2_calc,        NULL},
  {pause_3_calc,        credit_8_fg},
  {pause_3_calc,        contacts_1_fg},
  {pause_3_calc,        contacts_2_fg},
  {pause_3_calc,        contacts_4_fg},
  {display_scores_calc, display_scores_fg},
  {display_scores_calc, display_scores_fg},
  {display_scores_calc, display_scores_fg},
  {NULL,                NULL}
};

static void intro_calc(void)
{
  if(intro_tab[intro_sequence_no].bg != NULL)
    intro_tab[intro_sequence_no].bg();
  else
    intro_sequence_no = 8;
}

/*
 * This is main loop of the intro. Gave this function a touch up.
 * Start game is space bar now (rather than any input event). JN, 05SEP20
 */
static void intro_draw(void)
{
	int sisn;

	sisn = intro_sequence_no; /*get snapshot of background var. JN, 14SEP20*/
	if (intro_tab[sisn].fg != NULL)
		intro_tab[sisn].fg();

	/*
	 * Pull input events of the tkevent buffer
	 */
	for (;;) {
		tkevent k;

		if (!get_kb_event(&k))
			break;

		if (k.type == KEYBOARD_EVENT) {
			switch (tolower(k.val)) {
				case KEY_QUIT:
				end_game(0);

				case ' ':
				game_ship_start_init();
				return;
			}
		} else if (k.type == MOUSE_EVENT) {
			game_ship_start_init();
		}
	}
}

/***************************************************************************
* This is called through out the intro cycle.
***************************************************************************/
void intro_start_reinit(void)
{
	if (verbose_logging == TRUE)
		printf("Running intro_start_reinit().\n");

	intro_count = 0.0;
	intro_sequence_no = 0;
	intro_back_bm_no = 0;

	draw_hook = intro_draw;
	bg_calc_hook = intro_calc;

	sound_cmd_simple(SND_CMD_PLAY_SONG);
}

/***************************************************************************
* This is called once.
***************************************************************************/
void intro_start_init(void)
{
	if (verbose_logging == TRUE)
		printf("Running intro_start_init().\n");

	/*
	 * You're a dirty little cheat !. No game continuance for you sunshine !
	 */
	if (ship_start[0]) {
		fleet_select_init();
		return;
	}

	/*
	 * No preserved session. Normal game
	 */
	if (preserved_session == NULL) {
		fleet_select_init();
		return;
	}

	/*
	 * Preserves data found, haven't cheated. Start game continuance
	 * screen.
	 */
	game_cont_init();
}
