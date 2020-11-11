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
* Entity object routines.. this is the daddy class for all players.
* (This object got a significant rework in 4.0 - JN, 27SEP20)
*
****************************************************************************
****************************************************************************
***************************************************************************/
extern "C" {
#include <math.h>
#include "defs.h"
#include "linked_data_externs.h"
#include "misc_externs.h"
#include "sprite_loader_externs.h"
#include "opengl_externs.h"
}
#include "tentity.h"

/***************************************************************************
*
***************************************************************************/
tentity::tentity(void)
{
	state = 0;
	anim_len = anim_ptr = 0; // Reverted 2.2. JN, 04SEP20
	pos_dx = pos_dy = 0.0;
	anim_speed = GENERAL_SPD;
}

tentity::~tentity(void)
{
}

void tentity::init(
  tfloor *f, bm_t *bm_p, tentity **dp, int alen, float px, float py)
{
	floor_ptr = f;
	bm = bm_p;
	droids = dp;
	anim_len = alen;
	spos_x = old_pos_x = pos_x = px; // Added spos_*. JN, 18SEP20
	spos_y = old_pos_y = pos_y = py;
}

/*
 * This takes variables from the background context and transfers their
 * values to the foreground (draw()) context. To do it properly, you need
 * inter process communication control (semphores, spinlocks etc) when
 * different contexts access the same data. Back in 1997 I made the design
 * decision to have a simple "one way" copy like the below method. In 1997,
 * it was a good assumption that the variable transfers below are atomic.
 * These variables fitted into a 32bit CPU register in 1997 and now days,
 * everything is 64bit. This assumption hows with with some careful handling
 * of the data.
 * JN, 18SEP20
 */
void tentity::snap_shot(void)
{
	spos_x = (int)rintf(pos_x);
	spos_y = (int)rintf(pos_y);
	ss_anim_ptr = anim_ptr;
}

void tentity::do_sound(int fx_no, float vol, float freq)
{
	if (floor_ptr != (*droids)->floor_ptr)
		return;

	sound_cmd(	// New 4.0. JN, 17SEP20
		SND_CMD_FX_MIXED,
		fx_no,
		vol,
		freq,
		spos_x - (*droids)->spos_x,
		spos_y - (*droids)->spos_y);
}

/*
 * New method for 4.0. Droid circumference detection.
 * With the entity object being the reference point, this method tests a
 * colision condition bounded by the area x1, y1, and x2, y2. JN, 26SEP20
 */
int tentity::droid_circum_box_detect(int x1, int y1, int x2, int y2)
{
	int i;

	for (i = 0; i < COL_DROID_PER_SIZE; i++) {
		col_droid_perimeter_st *p = &col_droid_perimeter[i];
		if (p->x >= x1 && p->x < x2 && p->y >= y1 && p->y < y2)
			return 1;
	}

	return 0;
}

/*
 * New method for 4.0. JN, 26SEP20
 */
void tentity::open_door(tdoor *d)
{
	if ((d->anim_ptr >> 4) >= DOOR_STATE_THRES)
		return;

	if (d->open())
		do_sound(FX_OPEN_DOOR, 0.3, 1.0);
}

/*
 * Test for a floor colision.
 * Since floor sprites take up nearly all of the space of it's rendering area,
 * the most efficient/effective method is to simply test if droid is over a
 * floor sprite that is set to bounded. There is no value using circumference
 * detection. No change from previous versions. JN, 26SEP20
 */
int tentity::colision_floor(void)
{
	tflr_bm *spr;

	spr = flr_bm + *(floor_ptr->fmap +
		((int)(rintf(pos_y) / SPRITE_SIZE_Y) * floor_ptr->fmap_x_size) +
		(int)(rintf(pos_x) / SPRITE_SIZE_X));
	if (spr->bound == 'y') {
		pos_dx = old_pos_x - pos_x;
		pos_dy = old_pos_y - pos_y;
		return 1;
	}

	old_pos_x = pos_x;
	old_pos_y = pos_y;
	return 0;
}

/*
 * Gave this method a bit of love. Mostly reworked door colision detection.
 * JN, 26SEP20
 *
 * Minor tweaking of this method. Found a bug in doors closed collision
 * detector. Made the doors open detect areas smaller. JN, 16OCT20
 */
int tentity::colision_doors(void)
{
	int i;

	/*
	 * Test for doors. Enhanced using driod circumference test.
	 */
	for (i = 0; i < MAX_DOORS; i++) {
		int dstx, dsty;

		/*
		 * End of door list ?. We're done.
		 */
		if (floor_ptr->door[i] == NULL)
			break;

		/*
		 * Calc x and y distances between door and droid. The droid
		 * is the reference position.
		 */
		dstx = rintf(floor_ptr->door[i]->pos_x - pos_x);
		dsty = rintf(floor_ptr->door[i]->pos_y - pos_y);

		/*
		 * Test for closed door state. Simple distance test. This
		 * doesn't have to be technical.
		 */
		if (!floor_ptr->door[i]->state) {
			int dax, day;

			dax = abs(dstx);
			day = abs(dsty);
			if(dax < (SPRITE_HSIZE_X + (DROID_COL_BXL_X >> 1)) &&
				day < SPRITE_HSIZE_Y + (DROID_COL_BXL_Y >> 1)) {
				open_door(floor_ptr->door[i]);
				return 1;
			}
			continue; // Not near door. Next..
		}

		/*
		 * Test for open door state. This will be two small areas, and
		 * the areas will vary according to if they are a vertical of
		 * horizontal door.
		 */
		if (floor_ptr->door[i]->type == DOOR_HORIZONTAL) {
			/*
			 * Test area 1
			 */
			if (droid_circum_box_detect(
						dstx - (SPRITE_HSIZE_X + 1),
						dsty - (SPRITE_HSIZE_Y - 6),
						dstx - SPRITE_HSIZE_X,
						dsty + (SPRITE_HSIZE_Y - 6))) {
				open_door(floor_ptr->door[i]);
				calc_norm_accel(
					&pos_dx,
					pos_x,
					floor_ptr->door[i]->pos_x,
					DOOR_ADJ_DIV,
					DOOR_ADJ_LIM);
				return 1;

			/*
			 * Test area 2 (if area one didn't pass)
			 */
			} else if (droid_circum_box_detect(
						dstx + (SPRITE_HSIZE_X - 1),
						dsty - (SPRITE_HSIZE_Y - 6),
						dstx + SPRITE_HSIZE_X,
						dsty + (SPRITE_HSIZE_Y - 6))) {
				open_door(floor_ptr->door[i]);
				calc_norm_accel(
					&pos_dx,
					pos_x,
					floor_ptr->door[i]->pos_x,
					DOOR_ADJ_DIV,
					DOOR_ADJ_LIM);
				return 1;
			}

		} else {  // Vertical door state
			/*
			 * Test area 1
			 */
			if (droid_circum_box_detect(
						dstx - (SPRITE_HSIZE_X - 6),
						dsty - (SPRITE_HSIZE_Y + 1),
						dstx + (SPRITE_HSIZE_X - 6),
						dsty - SPRITE_HSIZE_Y)) {
				open_door(floor_ptr->door[i]);
				calc_norm_accel(
					&pos_dy,
					pos_y,
					floor_ptr->door[i]->pos_y,
					DOOR_ADJ_DIV,
					DOOR_ADJ_LIM);
				return 1;

			/*
			 * Test area 2
			 */
			} else if (droid_circum_box_detect(
						dstx - (SPRITE_HSIZE_X - 6),
						dsty + (SPRITE_HSIZE_Y - 1),
						dstx + (SPRITE_HSIZE_X - 6),
						dsty + SPRITE_HSIZE_Y)) {
				open_door(floor_ptr->door[i]);
				calc_norm_accel(
					&pos_dy,
					pos_y,
					floor_ptr->door[i]->pos_y,
					DOOR_ADJ_DIV,
					DOOR_ADJ_LIM);
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
int tentity::colision_droids(void)
{
	int i;

	for (i = 0; i < MAX_DROIDS; i++) {
		float ix, iy;

		if (droids[i] == NULL)
			return 0;
		if (droids[i]->floor_ptr != floor_ptr)
			continue;
		if (droids[i]->state)
			continue;
		if (droids[i] == this)
			continue;

		ix = fabsf(pos_x - droids[i]->pos_x);
		iy = fabsf(pos_y - droids[i]->pos_y);
		if (ix < DROID_COL_BXL_X && iy < DROID_COL_BXL_Y) {
			ix = (droids[i]->pos_x - pos_x) / 4.0;
			iy = (droids[i]->pos_y - pos_y) / 4.0;
			pos_dx = -ix;
			pos_dy = -iy;
			droids[i]->pos_dx = ix;
			droids[i]->pos_dy = iy;

			// Don't break here. Col detect all droids
		}
	}

  return 1;
}

/*
 * Slightly modified this method. If driod colides with floor or door, then it
 * won't test for other droids. The reason for this is because this sub-methods
 * change the droid's pos_* and pos_d* variables with feedback. Making multiple
 * changes to pos_* and pos_d* variables (with feedback) at the same time without
 * letting the background code process the change will produce chaotic results.
 * This by itself could cause droids to be jammed together if floor/door colisions
 * take precedence, however the droid avoidance code should remove that condition.
 * JN, 27SEP20
 */
void tentity::colision_detect(void)
{
	if (colision_floor() || colision_doors())
		return;

	colision_droids();
}

/*
 * Reverted from 2.2. JN, 04SEP20
 */
void tentity::bg_calc(void)
{
	anim_ptr += anim_speed;
	if ((anim_ptr >> 4) == anim_len)
		anim_ptr = 0;
	colision_detect();
	pos_x += pos_dx;
	pos_y += pos_dy;
}
