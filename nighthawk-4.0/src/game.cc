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
* game.cc. Root game functions.
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
#include "defs.h"
#include "linked_data_externs.h"
#include "misc_externs.h"
#include "sprite_loader_externs.h"
#include "opengl_externs.h"
#include "scores_externs.h"
}
#include "intro_externs.h"
#include "tship.h"

tship		*ship;

void		game_ship_start_init(void); // JN, 29AUG20
static void	game_ship_start(void);

static int	game_count;

/***************************************************************************
*
***************************************************************************/
void display_final_score(void)
{
  char str[STR_LEN + 1];

  snprintf(str, STR_LEN, "Your Final Score: %u", score);
  glColor3fv(green_pixel[2]);
  print_str(str, SCREEN_HSIZE_X - ((strlen(str) << 3) >> 1), 180);
}

/***************************************************************************
* Tanel Kulaots's level continuence patch melded into these routines
*
* Nb/ This sets your score back to the last ship you completed. If you die
* during a ship, you will loose the scoring for that level.
***************************************************************************/
static int game_term_dis_cont;

static void game_term_calc(void)
{
	game_count++; // JN, 03SEP20
	if(game_count > 100)
		game_term_dis_cont = 1;
}

/*
 * Added proper keyboard handling to Tanel's excellent work. JN, 05SEP20.
 */
static void game_term_draw(void)
{
	const char *mess = "Continue: y/n/q ?\n";

	glColor4fv(bm_colour);
	blit_bm(&trans_terminated_bm,
		SCREEN_HSIZE_X - (trans_terminated_bm.size_x >> 1), 50);

	display_final_score();

	if (game_term_dis_cont)
		display_message((char *)mess, 150);

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
			sound_cmd_simple(SND_CMD_FX_PRI_STOP);
			game_ship_start_init();
			return;

			case KEY_NO:
			sound_cmd_simple(SND_CMD_FX_PRI_STOP);
			save_scores(FALSE);
			ship_start[0] = 0; // Reset ship. JN, 08OCT20
			ship_start_cmdline_f = FALSE;
			fleet_select_init();
			return;

			case KEY_QUIT:
			save_scores(FALSE);
			end_game(0);
		}
	}
}

static void game_term_init(void)
{
	if (verbose_logging == TRUE)
		printf("Running game_term_init().\n");

	game_count = 0;
	game_term_dis_cont = 0;
	draw_hook = game_term_draw;
	bg_calc_hook = game_term_calc;
	sound_cmd_primary(FX_TRANS_TERM, 1.0, 1.0);
}

/***************************************************************************
* This game state is the static noise you see when you get killed.
***************************************************************************/
static void game_noise_calc(void)
{
  game_count++; // JN, 03SEP20
  if(game_count > 50)
    game_term_init();
}

static void game_noise_draw(void)
{
  int x, y;

  for(y = 0; y < 200; y += 10)
  {
    for(x = 0; x < 320; x += 10)
    {
      glColor3fv(white_pixel[random() & 0x7]);
      blit_rect(x, y, 10, 10);
    }
  }
}

static void game_noise_init(void)
{
	if (verbose_logging == TRUE)
		printf("Running game_noise_init().\n");

	game_count = 0;
	draw_hook = game_noise_draw;
	bg_calc_hook = game_noise_calc;
	shake_reset();  // New. JN, 06SEP20
	sound_cmd_simple(SND_CMD_FX_STOP_ALL);
	sound_cmd_simple(SND_CMD_RANDOM);
}

/***************************************************************************
* This (rarely executed) game state needed a bit of love. Thanks Eric for
* the bug pickup :). I *think* this is good now. JN, 12OCT20
***************************************************************************/
static char	award_str[STR_LEN + 1];

static void game_finale_finish(void)
{
	if (god_mode == TRUE || ship_start_cmdline_f == TRUE)
		save_scores(FALSE);
	else
		save_scores(TRUE);
	ship_start[0] = 0;
	ship_start_cmdline_f = FALSE;
	fleet_select_init();
}

static void game_finale_draw(void)
{
	const char *mess1 =
		"You have completed all levels.\n"
		"WELL DONE !\n"
		"(Send us a screen dump)\n";
	const char *mess2 =
		"Thank you for playing Nighthawk,\n"
		"Press Space to continue.\n";

	glColor3fv(white_pixel[7]);
	display_message((char *)mess1, 5);

	glColor3fv(yellow_pixel[0]);
	display_message(award_str, 70);

	glColor3fv(green_pixel[8]);
	display_message((char *)mess2, 125);

	display_final_score();

	for (;;) {
		tkevent k;

		if (!get_kb_event(&k))
			break;

		if (k.type == KEYBOARD_EVENT) {
			if (k.val == KEY_SELECT) {
				game_finale_finish();
				return;
			}
		} else if (k.type == MOUSE_EVENT) {
			game_finale_finish();
			return;
		}
	}
}

void game_finale_init(void)
{
	char award_name[SCORES_NAME_LEN + 1];

	if (verbose_logging == TRUE)
		printf("Running game_finale_init().\n");

	draw_hook = game_finale_draw;
	bg_calc_hook = do_nothing;
	shake_reset();
	sound_cmd_simple(SND_CMD_FX_STOP_ALL);

	get_score_username(award_name);
	snprintf(award_str, STR_LEN,
			"Award: %s\n"
			"Fleet: %s\n",
			award_name, fleet_ptr->name);
}

/***************************************************************************
* Game execution code.
***************************************************************************/
static void delete_ship(void)
{
	ship->unload();
	if(verbose_logging == TRUE)
		printf("delete() ship\n");
	usleep(100000); // New. A delay to allow any background code to
		        // finish running. JN, 18SEP20
	delete(ship);
}

static void game_run_calc(void)
{
	if(ship != NULL)
		ship->level_bg_calc();

	shake_calc(); // New. JN, 06SEP20

	// JN, 03SEP20
	if (score > sscore)
		sscore += ((score - sscore) >> 1) | 1;
	if(score < sscore)
		sscore -= ((sscore - score) >> 1) | 1;
}

/*
 * This is main loop of the game
 */
static void game_run_draw(void)
{
  ship->level_run();

  if(ship->state == STATE_END)
  {
    delete_ship();
    game_noise_init();
    return;
  }

  if(ship->state == STATE_COMPLETE)
  {
    delete_ship();

    ship_ptr++;
    if(ship_table[ship_ptr] != NULL)
    {
     /*
      * If there is another ship. Preserve score (Tanel's level continuence)
      * and save session state (my start of game continuence).
      * JN, 09SEP20
      */
      last_score = score;
      preserve_session();  // New. JN, 07SEP20

      game_ship_start();
      return;
    }

    ship_ptr--;
    game_finale_init();
  }
}

static void game_run_init(void)
{
	draw_hook = game_run_draw;
	bg_calc_hook = game_run_calc;
}

/***************************************************************************
*
***************************************************************************/
static void game_ship_start(void)
{
	if(verbose_logging == TRUE)
		printf("new() ship\n");
	ship = new(tship);
	if (ship == NULL)
		print_new_error(__func__);

	ship->load(ship_table[ship_ptr]);
	sound_cmd_simple(SND_CMD_FX_STOP_ALL);

	game_run_init();
}
void game_ship_start_init(void)
{
	if (verbose_logging == TRUE)
		printf("Running game_ship_start_init().\n");

	sound_cmd_simple(SND_CMD_STOP_SONG); // JN, 17SEP20
	score = last_score;
	draw_hook = game_ship_start;
	bg_calc_hook = do_nothing;
}
