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
* Generic droid object routines
*
****************************************************************************
****************************************************************************
***************************************************************************/
extern "C" {
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "defs.h"
#include "misc_externs.h"
#include "sprite_loader_externs.h"
#include "opengl_externs.h"
#include "scores_externs.h"
}
#include "tdroid.h"
#include "tparadroid.h"
#include "tship.h"
#include "game_externs.h"

/***************************************************************************
*
***************************************************************************/
tdroid::tdroid(void)
{
  int x;

  for(x = 0; x < MAX_LASERS; x++)
    laser_list[x] = NULL;

  expl_anim_ptr = 0; // Reverted to 2.2. JN, 04SEP20
  expl_anim_speed = 10;
  expl_anim_len = 7;
  shield_wave_inc = 0.0;
  captured = 0;
  recharge_delay = 0;
  boast_msg_f = FALSE;  // New, JN, 08SEP20
}

void tdroid::init(tfloor *f, tentity **dp, float px, float py)
{
  tentity::init(f, &droid_bm[0], dp, 10, px, py);
}

int tdroid::create_laser(int vx, int vy)
{
  int x, status = 0;

  for(x = 0; x < MAX_LASERS; x++)
    if(laser_list[x] == NULL)
    {
      if(verbose_logging == TRUE)
        printf("new() laser object (%d)\n", stats.armament);

      switch(stats.armament)
      {
        case 1:
          laser_list[x] = new tlaser0();
          break;
        case 2:
          laser_list[x] = new tlaser1();
          break;
        case 3:
          laser_list[x] = new tlaser2();
          break;
        case 4:
          laser_list[x] = new tlaser3();
          break;
      }

      if(laser_list[x] == NULL)
        print_new_error(__func__);

      laser_list[x]->init(
        floor_ptr, droids, pos_x, pos_y, vx, vy, this);

      break;
    }

  return status;
}

void tdroid::free_all_laser(void)
{
  int x;

  for(x = 0; x < MAX_LASERS; x++)
    if(laser_list[x] != NULL)
    {
      if(verbose_logging == TRUE)
        printf("delete() laser object (%d)\n",
          laser_list[x]->owner->stats.armament);
      delete(laser_list[x]);
      laser_list[x] = NULL;
    }
}

void tdroid::snap_shot(void)
{
  int x;

  tentity::snap_shot();

  ss_expl_anim_ptr = expl_anim_ptr;

  for(x = 0; x < MAX_LASERS; x++)
    if(laser_list[x] != NULL)
      laser_list[x]->snap_shot();
}

void tdroid::draw_laser(tfloor *c_f_ptr, int px, int py)
{
  int x;

  for(x = 0; x < MAX_LASERS; x++)
    if(laser_list[x] != NULL)
      laser_list[x]->draw(c_f_ptr, px, py);
}

void tdroid::bg_calc_laser(void)
{
  int x;

  for(x = 0; x < MAX_LASERS; x++)
    if(laser_list[x] != NULL)
    {
      laser_list[x]->bg_calc();
      if(laser_list[x]->state == 2)
      {
        if(verbose_logging == TRUE)
          printf("delete() laser object (%d)\n",
			laser_list[x]->owner->stats.armament);
        delete(laser_list[x]);
        laser_list[x] = NULL;
      }
    }
}


void tdroid::draw_droid_ls(int ix, int iy)
{
  int        i;
  static int pos[4][2] =
    {{-2, -19}, {-2, 16}, {-17, -2}, {14, -2}};

  for(i = 0; i < 4; i++)
    blit_rect(ix + pos[i][X], iy + pos[i][Y], 3, 3);
}

void tdroid::draw_droid(int ix, int iy)
{
  int x;

  if(stats.shielding <= (stats.max_shielding >> 3))
  {
    glColor4f(1.0, 0.0, 0.0, 0.5);
    draw_droid_ls(ix, iy);
    glColor4f(1.0, 0.0, 0.0, fabsf(sinf(shield_wave_inc))); // Fixed. 04SEP20
  }
  else
    if(stats.shielding <= (stats.max_shielding >> 2))
    {
      glColor4f(0.0, 1.0, 0.0, 0.5);
      draw_droid_ls(ix, iy);
      glColor4f(0.0, 1.0, 0.0, fabsf(sinf(shield_wave_inc))); // Fixed. 04SEP20
    }
    else
      glColor4fv(bm_colour);

  blit_bm(&bm[ss_anim_ptr >> 4], ix - SPRITE_HSIZE_X, iy - SPRITE_HSIZE_Y); // JN, 04SEP20

  glColor4fv(bm_colour2);
  for(x = 0; x < 3; x++)
    blit_bm(
      &digit_bm[(int)(stats.type[x] - '0')], ix - 13 + (x << 3), iy - 4);
}

void tdroid::draw(tfloor *c_f_ptr, int px, int py)
{
  draw_laser(c_f_ptr, px, py);

  if(floor_ptr != c_f_ptr)
    return;

  switch(state)
  {
    case 0: /*running*/
      draw_droid(
        (spos_x - px) + SCREEN_HSIZE_X,
        (spos_y - py) + SCREEN_HSIZE_Y);
      break;

    case 1: /*dieing*/
      glColor4fv(bm_colour);
      blit_bm(
        &explosion_bm[ss_expl_anim_ptr >> 4], // JN, 04SEP20
        (spos_x - px) + (SCREEN_HSIZE_X - 20),
        (spos_y - py) + (SCREEN_HSIZE_Y - 20));
      break;
  }
}

void tdroid::recharge(void)
{
	int ix, iy;

	if (floor_ptr->power_bay == NULL)
		return; // If floor has no bay bay, then return.

	if ((recharge_delay++ % 2)) // JN, 09SEP20
		return;

	if (stats.shielding == stats.max_shielding)
		return; // If fully charged, then return.

	// Collision detection
	ix = (((int)rintf(pos_x)) + SPRITE_HSIZE_X) - floor_ptr->power_bay->pos_x;
	iy = (((int)rintf(pos_y)) + SPRITE_HSIZE_Y) - floor_ptr->power_bay->pos_y;
	if ((ix < 0) || (ix > SPRITE_SIZE_X) || (iy < 0) || (iy > SPRITE_SIZE_Y)) {

		boast_msg_f = FALSE;  // New, JN, 08SEP20
		return;
	}

	if (this == droids[0]) { // If it's us, then boast, but deduct score
		if (score) {
			score--;
			stats.shielding++;
			if (boast_msg_f == FALSE) { // Only prints msg once per boost. JN, 08SEP20
				boast_msg_f = TRUE;
				console_message_add((char *)"Boost!");
	                        do_sound(FX_POWER_UP, 1.0, 1.2);
			}
		}
	} else {
		stats.shielding++;
	}
}

void tdroid::bg_calc(void)
{
  int spin;

  spin = (stats.shielding * 15) / stats.max_shielding; // Reverted 2.2. JN, 04SEP20

  switch(state)
  {
    case 0: /*running*/
      anim_speed = spin;
      shield_wave_inc += 0.3; // Reverted 2.2. JN, 04SEP20
      tentity::bg_calc();
      recharge();
      break;

    case 1: /*dieing*/
      anim_speed = spin;
      tentity::bg_calc();
      expl_anim_ptr += expl_anim_speed;
      if((expl_anim_ptr >> 4) == expl_anim_len) // Reverted 2.2. JN, 04SEP20
        state++;
      break;
  }

  bg_calc_laser();
}

void tdroid::friction(void)
{
  float x;

  if(pos_dx > 0)
  {
    x = pos_dx;
    if(x > 1)
      x = 1;
    pos_dx -= x;
  }
  if(pos_dx < 0)
  {
    x = pos_dx;
    if(x < -1)
      x = -1;
    pos_dx += 1;
  }
  if(pos_dy > 0)
  {
    x = pos_dy;
    if(x > 1)
      x = 1;
    pos_dy -= x;
  }
  if(pos_dy < 0)
  {
    x = pos_dy;
    if(x < -1)
      x = -1;
    pos_dy += 1;
  }
}

/*
 * Status now displays alive droids on the floor. JN, 19SEP20
 */
void tdroid::display_nd(void)
{
	char str[STR_LABEL_LEN + 1];
	int i, nds = 0, ndf = 0;

	for (i = 1; i < MAX_DROIDS; i++) {
		if (droids[i] == NULL)
			continue;

		if (!droids[i]->state) {
			nds++;
			if (floor_ptr == droids[i]->floor_ptr)
				ndf++;
		}
	}

	snprintf(str, STR_LABEL_LEN, "droids=%d/%d", ndf, nds);
	console_message_add(str);
}

void tdroid::hit(tlaser *l)
{
  char str[STR_LABEL_LEN + 1];

  pos_dx = l->pos_dx / 4.0;
  pos_dy = l->pos_dy / 4.0;

  stats.shielding -= l->damage_factor;
  if(stats.shielding < 0)
  {
    state = 1;

    snprintf(str, STR_LABEL_LEN, "%s d %s", l->owner->stats.type, stats.type);
    console_message_add(str);

    if(this != droids[0])
      if(!god_mode)
        score += stats.entry * 50; // 2x score. JN, 10SEP20

    display_nd();

    if(random() & 1)
      do_sound(FX_EXPLOSION_1, 1.0, 1.0);
    else
      do_sound(FX_EXPLOSION_2, 1.0, 1.0);
  }
  else
  {
    attack_droid = l->owner;

    snprintf(
      str, STR_LABEL_LEN, "%s h %s(%d)", l->owner->stats.type, stats.type, stats.shielding);
    console_message_add(str);

    if(this != droids[0])
    {
      if(!god_mode)
        score += stats.entry * 2; // 2x score. JN, 10SEP20
    }
    else
    {
      // New. Flash screen when paradroid is hit. JN, 09SEP20
      ship->setup_flash_screen(1.0, 0.0, 0.0, 1.0);
    }

    if(random() & 1)
      do_sound(FX_DROID_HIT_1, 0.8, 1.0);
    else
      do_sound(FX_DROID_HIT_2, 0.8, 1.5);
  }
}
