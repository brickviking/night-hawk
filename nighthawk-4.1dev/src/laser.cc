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
* laser object routines
*
****************************************************************************
****************************************************************************
***************************************************************************/
extern "C" {
#include <math.h>
#include "defs.h"
#include "misc_externs.h"
#include "sprite_loader_externs.h"
#include "opengl_externs.h"
}
#include "tlaser.h"
#include "tdroid.h"
#include "tparadroid.h"
#include "tship.h"
#include "game_externs.h"

/***************************************************************************
*
***************************************************************************/
tlaser::tlaser(void)
{
}

/*
 * Converted the sin() and tan() functions to float sinf and tanf() funcs.
 * JN, 04SEP20.
 * Made laser angles freely rotating. JN, 02OCT20
 */
void tlaser::init(
  tfloor *f, tentity **dp, float px, float py, int vx, int vy, tentity *o)
{
	laser_angle = atan3f((float)vy, (float)vx);
	pos_dx = cosf(laser_angle) * anim_speed; // Reverted, JN, 04SEP20
	pos_dy = sinf(laser_angle) * anim_speed;
	laser_angle = RAD2DEG(laser_angle);

	owner = o;
	tentity::init(f, &laser_l_bm, dp, 7, px, py);
	if (owner == *droids) {
		buzzed = 1;
	} else {
		calc_buzz();
		old_pos_x_sign = pos_x_sign;
		old_pos_y_sign = pos_y_sign;
		buzzed = 0;
	}
}

/*
 * New for 4.0... but old. tentity has improved door colision detection, but
 * for lasers, they don't have a droid footprint, they go fast anyway, and
 * their appearance in the game doesn't look bad or not noticable. So
 * i'll keep the colision detection for doors the same as in 2.2. For this,
 * we need to overload this method, hence below. JN, 26SEP20
 */
int tlaser::colision_doors(void)
{
	int i;

	for (i = 0; i < MAX_DOORS; i++) {
		int ix, iy;

		if (floor_ptr->door[i] == NULL)
			return 0;

		ix = rintf(fabsf(pos_x - floor_ptr->door[i]->pos_x));
		iy = rintf(fabsf(pos_y - floor_ptr->door[i]->pos_y));
		if (ix < SPRITE_SIZE_X && iy < SPRITE_SIZE_Y) {
			if (!floor_ptr->door[i]->state) {
				open_door(floor_ptr->door[i]);
				return 1;
			}
		}
	}

	return 0;
}

/*
 * Slightly improved this detection. Droid to droid colision detection still
 * used box area like in 1.x and 2.x except I have made the box area slightly
 * bigger to make collisions look for realistic. JN, 26SEP20
 */
int tlaser::colision_droids(void)
{
	int i;

	for (i = 0; i < MAX_DROIDS; i++) {
		int ix, iy;

		if (droids[i] == NULL)
			return 0;
		if (droids[i]->floor_ptr != floor_ptr)
			continue;
		if (droids[i]->state)
			continue;
		if (droids[i] == owner)
			continue;

		ix = rintf(fabsf(pos_x - droids[i]->pos_x));
		iy = rintf(fabsf(pos_y - droids[i]->pos_y));
		if (ix < DROID_COL_BXL_X && iy < DROID_COL_BXL_Y) {
          		((tdroid *)droids[i])->hit(this);
			return 1;
		}
	}

	return 0;
}

/*
 * Like it's counterpart in entity.cc, I've reworked this code to only
 * detect/process 1 colision at a time. See notes in entity.cc. JN, 27SEP20
 */
void tlaser::colision_detect(void)
{
	/*
	 * Test for floor and doors colisions. (takes priority)
	 */
	if (colision_floor() || colision_doors()) {
		state = 1;
		do_sound(FX_EXPLOSION_1, 1.0, 1.0);
		return;
	}

	/*
	 * Test for droids
	 * Lasers don't explode on droids. Go directly to finish state.
	 */
	if (colision_droids()) {
		state = 2;
		return;
	}

	/*
	 * No colision. Laser goes on it's merry way..
	 */
	pos_x += pos_dx;
	pos_y += pos_dy;
}

void tlaser::calc_buzz(void)
{
  if((*droids)->pos_x < pos_x)
    pos_x_sign = 0;
  else
    if((*droids)->pos_x > pos_x)
      pos_x_sign = 1;

  if((*droids)->pos_y < pos_y)
    pos_y_sign = 0;
  else
    if((*droids)->pos_y > pos_y)
      pos_y_sign = 1;
}

/*
 * Reworked this code to stop paradroid getting buzzed by lasers on a different
 * floor. JN, 18SEP20
 */
void tlaser::buzz(void)
{
	if (buzzed)
		return;

	if (state)
		return;

	if (floor_ptr != (*droids)->floor_ptr) // The new code. JN, 18SEP20
		return;

	calc_buzz();

	if ((pos_x_sign != old_pos_x_sign) || (pos_y_sign != old_pos_y_sign)) {
		float d1, d2;

		d1 = (*droids)->pos_x - pos_x;
		d2 = (*droids)->pos_y - pos_y;

		if (((d1 * d1) + (d2 * d2)) < (50 * 50)) {
			buzzed = 1;
			// New. Flash screen when paradroid is buzzed.
			// JN, 09SEP20
			ship->setup_flash_screen(0.5, 0.5, 0.5, 1.0);
			do_sound(FX_LASER_BUZZ, 1.0, 1.0);
			console_message_add((char *)"buzzed!");
		}
	}

	old_pos_x_sign = pos_x_sign;
	old_pos_y_sign = pos_y_sign;
}

void tlaser::draw(tfloor *c_f_ptr, int ppx, int ppy)
{
  int ix, iy;

  if(floor_ptr != c_f_ptr)
    return;

  switch(state)
  {
    case 0:
      ix = (spos_x - ppx) + SCREEN_HSIZE_X;
      iy = (spos_y - ppy) + SCREEN_HSIZE_Y;
      glColor4fv(bm_colour);
      glPushMatrix();
      glTranslatef(ix, iy, 0.0);
      glRotatef(laser_angle, 0.0, 0.0, 1.0);
      blit_bm(bm, -(LASER_SIZE_X >> 1), -(LASER_SIZE_Y >> 1));
      glPopMatrix();
      break;

    case 1:
      ix = (spos_x - ppx) + (SCREEN_HSIZE_X - (EXPLODE_SIZE_X >> 1));
      iy = (spos_y - ppy) + (SCREEN_HSIZE_Y - (EXPLODE_SIZE_Y >> 1));
      glColor4fv(bm_colour);
      blit_bm(&explosion_bm[ss_anim_ptr >> 4], ix, iy);  // JN, 04SEP20
      break;
  }
}

void tlaser::bg_calc(void)
{
  switch(state)
  {
    case 0:
      colision_detect();
      buzz();
      break;

    case 1:
      anim_ptr += anim_speed; // JN, 03SEP20
      if((anim_ptr >> 4) == anim_len)
        state++;
      break;
  }
}

/***************************************************************************
*
***************************************************************************/
tlaser0::tlaser0(void)
{
  anim_speed = 8;  // Reverted 2.2, JN, 04SEP20
  damage_factor = 4;
}

void tlaser0::init(
  tfloor *f, tentity **dp, float px, float py, int vx, int vy, tentity *o)
{
  tlaser::init(f, dp, px, py, vx, vy, o);
  bm = &laser_l_bm;
  do_sound(FX_LASER_LINARITE, 1.0, 1.0);
}

/***************************************************************************
*
***************************************************************************/
tlaser1::tlaser1(void)
{
  anim_speed = 10;  // Reverted 2.2, JN, 04SEP20
  damage_factor = 10;
}

void tlaser1::init(
  tfloor *f, tentity **dp, float px, float py, int vx, int vy, tentity *o)
{
  tlaser::init(f, dp, px, py, vx, vy, o);
  bm = &laser_cb_bm;
  do_sound(FX_LASER_CROC_BENZ, 1.0, 1.0);
}

/***************************************************************************
*
***************************************************************************/
tlaser2::tlaser2(void)
{
  anim_speed = 12;  // Reverted 2.2, JN, 04SEP20
  damage_factor = 20;
}

void tlaser2::init(
  tfloor *f, tentity **dp, float px, float py, int vx, int vy, tentity *o)
{
  tlaser::init(f, dp, px, py, vx, vy, o);
  bm = &laser_uv_bm;
  do_sound(FX_LASER_UVAROVITE, 1.0, 1.0);
}

/***************************************************************************
*
***************************************************************************/
tlaser3::tlaser3(void)
{
  anim_speed = 9;  // Reverted 2.2, JN, 04SEP20
  damage_factor = 50;
}

void tlaser3::init(
  tfloor *f, tentity **dp, float px, float py, int vx, int vy, tentity *o)
{
  tlaser::init(f, dp, px, py, vx, vy, o);
  bm = &laser_te_bm;
  do_sound(FX_LASER_TIGER_EYE, 1.0, 1.2);
}
