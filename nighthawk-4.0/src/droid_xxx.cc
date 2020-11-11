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
* droid routines (for individual types) object routines
*
* 1xx Cleaning
* 2xx Logistic/Servant
* 3xx Messenger
* 4xx Maintenance
* 5xx Medical
* 6xx Sentinel
* 7xx Battle
* 8xx Navigational
* 9xx Command
*
****************************************************************************
****************************************************************************
***************************************************************************/
extern "C" {
#include <string.h>
#include "math.h"
#include "defs.h"
#include "linked_data_externs.h"
#include "misc_externs.h"
#include "sprite_loader_externs.h"
#include "opengl_externs.h"
#include "scores_externs.h"
}
#include "tdroid_1xx.h"
#include "tdroid_2xx.h"
#include "tdroid_3xx.h"
#include "tdroid_4xx.h"
#include "tdroid_5xx.h"
#include "tdroid_6xx.h"
#include "tdroid_7xx.h"
#include "tdroid_8xx.h"
#include "tdroid_9xx.h"

#define MASK ~0x3

/***************************************************************************
* Converted these methods from a recursive linked list to an iterative
* linked list design to improve code safety. Linked list is FIFO. JN, 30AUG20
*
* Added block detection code. If Droid is blocked by floor, droid etc. It will
* go to it's next instruction (hopefully this will unblock it and it will
* carry on. If it ends up in another room.. well, nothing I can do but that)
* JN, 30SEP20
***************************************************************************/
tdroid_108::tdroid_108(void)
{
	memcpy(&stats, &droid_stats[DROID_108], sizeof(tdroid_stats));
	count = 0;
	droid_cmd_list = NULL;
	droid_cmd_list_ptr = NULL;
	blk_detect_ptr = 0;
	blk_detect_count = 0;
}

tdroid_108::~tdroid_108(void)  // JN, 30AUG20
{
	free_all_cmd();
}

void tdroid_108::append_cmd(int cmd, int d1, int d2) // JN, 30AUG20
{
	tdroid_cmd_list **p;

	// Find end of list.
	p = &droid_cmd_list;
	while (*p != NULL)
		p = &((*p)->next);

	// Append entry and save record on end of linked list.
	*p = (tdroid_cmd_list *)alloc_mem(sizeof(tdroid_cmd_list));
	(*p)->cmd = cmd;
	(*p)->d1 = d1;
	(*p)->d2 = d2;
	(*p)->next = NULL;
}

/*
 * This is called after cmd list is build. JN, 30AUG20
 */
void tdroid_108::init_cmds(void)
{
	droid_cmd_list_ptr = droid_cmd_list;
}

void tdroid_108::free_all_cmd(void) // JN, 30AUG20
{
	tdroid_cmd_list **p;

	p = &droid_cmd_list;
	while (*p != NULL) {
		tdroid_cmd_list *tmp;

		tmp = (*p)->next;
		free(*p);
		*p = tmp;
	}
}

/*
 * A bunch of fix ups to this method. Used double precision sin()
 * abs() and tan functions. Using integer constants. Changed to
 * float functions and constants. JN, 04SEP20
 */
void tdroid_108::set_dir_vect(float vx, float vy)
{
  float d, s1, s2;

  s1 = fabsf(vx);
  s2 = fabsf(vy);
  if(s1 < s2)
    s1 = s2;

  if(s1 > stats.speed)
    s1 = stats.speed;
  else
    s1 /= 2.0;

  if(vy != 0.0)
    d = atanf(vx / vy);
  else
    d = atanf(vx);

  pos_dx = sinf(d) * s1;
  pos_dy = cosf(d) * s1;

  if(vy < 0.0)
  {
    pos_dx *= -1.0;
    pos_dy *= -1.0;
  }
}

void tdroid_108::next_cmd(void)
{
  if(droid_cmd_list_ptr->next == NULL)
    droid_cmd_list_ptr = droid_cmd_list;
  else
    droid_cmd_list_ptr = droid_cmd_list_ptr->next;
  count = 0;
}

/*
 * Read tail entry in blockage detect buffer. Write head entry. basically
 * we are reading the droid's pos_x and pos_y position BLK_DETECT_BUF_SIZE
 * entries ago.
 */
void tdroid_108::queue_dequeue_block_buffer(void)
{
	blk_detect_old_pos_x = blk_detect_buf[blk_detect_ptr].x;
	blk_detect_old_pos_y = blk_detect_buf[blk_detect_ptr].y;
	blk_detect_buf[blk_detect_ptr].x = pos_x;
	blk_detect_buf[blk_detect_ptr].y = pos_y;
	blk_detect_ptr++;
	blk_detect_ptr %= BLK_DETECT_BUF_SIZE;
}

/*
 * Turned this function into a status function. Action is performed in
 * bg_calc(). JN, 13OCT20
 */
int tdroid_108::block_detect(void)
{
	float dx, dy, dist;

	dx = pos_x - blk_detect_old_pos_x;
	dy = pos_y - blk_detect_old_pos_y;
	dist = sqrtf(dx * dx + dy * dy);
	if (dist > BLOCK_DETECT_DISTANCE) {
		blk_detect_count = 0;
		return FALSE;
	}

	blk_detect_count++;
	if (blk_detect_count < BLOCK_DETECT_WAIT)
		return FALSE;
	blk_detect_count = 0;
	return TRUE;
}

void tdroid_108::bg_calc(void)
{
  int ix, iy;

  queue_dequeue_block_buffer(); // Used by block_detect() JN, 13OCT20

 /*
  * If block detected, Goto next position. JN, 13OCT20
  */
  if(droid_cmd_list_ptr->cmd != 'w')
  {
    if(block_detect())
    {
      pos_dx = 0;
      pos_dy = 0;
      next_cmd();
    }
  }

  if(state == 0)
  {
    switch(droid_cmd_list_ptr->cmd)
    {
      case 'g':
        ix = ((int)rintf(pos_x)) & MASK;
        iy = ((int)rintf(pos_y)) & MASK;
        if((ix == (droid_cmd_list_ptr->d1 & MASK)) &&
           (iy == (droid_cmd_list_ptr->d2 & MASK)))
        {
          pos_dx = 0;
          pos_dy = 0;
          pos_x = droid_cmd_list_ptr->d1;
          pos_y = droid_cmd_list_ptr->d2;
          next_cmd();
        }
        else
          set_dir_vect(
            (float)droid_cmd_list_ptr->d1 - pos_x,
            (float)droid_cmd_list_ptr->d2 - pos_y);
        break;

      case 'w':
        friction();
        if(count >= droid_cmd_list_ptr->d1)
          next_cmd();
        else
          count++;
        break;
    }
  }

  tdroid::bg_calc();
}

/***************************************************************************
*
***************************************************************************/
tdroid_176::tdroid_176(void)
{
  memcpy(&stats, &droid_stats[DROID_176], sizeof(tdroid_stats));
}

/***************************************************************************
*
***************************************************************************/
tdroid_261::tdroid_261(void)
{
  memcpy(&stats, &droid_stats[DROID_261], sizeof(tdroid_stats));
}

tdroid_275::tdroid_275(void)
{
  memcpy(&stats, &droid_stats[DROID_275], sizeof(tdroid_stats));
}

/***************************************************************************
*
***************************************************************************/
tdroid_355::tdroid_355(void)
{
  memcpy(&stats, &droid_stats[DROID_355], sizeof(tdroid_stats));
}

tdroid_368::tdroid_368(void)
{
  memcpy(&stats, &droid_stats[DROID_368], sizeof(tdroid_stats));
}

/***************************************************************************
*
***************************************************************************/
tdroid_423::tdroid_423(void)
{
  memcpy(&stats, &droid_stats[DROID_423], sizeof(tdroid_stats));
}

tdroid_467::tdroid_467(void)
{
  memcpy(&stats, &droid_stats[DROID_467], sizeof(tdroid_stats));
}

tdroid_489::tdroid_489(void)
{
  memcpy(&stats, &droid_stats[DROID_489], sizeof(tdroid_stats));
}

/***************************************************************************
*
***************************************************************************/
tdroid_513::tdroid_513(void)
{
  memcpy(&stats, &droid_stats[DROID_513], sizeof(tdroid_stats));
  attack_droid = NULL;
}

void tdroid_513::attack(void)
{
  if((state == 0) && (attack_droid != NULL) && !captured)
  {
    if(attack_droid->state == 0)
      create_laser(
        (int)(attack_droid->pos_x - pos_x),
        (int)(attack_droid->pos_y - pos_y));

    attack_droid = NULL;
  }
}

void tdroid_513::bg_calc(void)
{
  tdroid_108::bg_calc();
  attack();
}

/***************************************************************************
*
***************************************************************************/
tdroid_520::tdroid_520(void)
{
  memcpy(&stats, &droid_stats[DROID_520], sizeof(tdroid_stats));
}

tdroid_599::tdroid_599(void)
{
  memcpy(&stats, &droid_stats[DROID_599], sizeof(tdroid_stats));
}

/***************************************************************************
*
***************************************************************************/
tdroid_606::tdroid_606(void)
{
  memcpy(&stats, &droid_stats[DROID_606], sizeof(tdroid_stats));
  attack_wait = 0;  // Reverted 2.2. JN, 04SEP20
  voice_c = 0;
  voice_type = FX_6XX_VOICE;
}

int tdroid_606::target_not_hidden(tentity *d)
{
  float tmp, dx, dy, tx = pos_x, ty = pos_y;
  int   a, i;

  dx = d->pos_x - pos_x;
  dy = d->pos_y - pos_y;
  i = (int)rintf(fabsf(dx));
  tmp = fabs(dy);

  if(i < tmp)
    i = (int)rintf(tmp);
  if(i == 0)
    return 0;

  i <<= 3;
  dx /= i;
  dy /= i;

  for(a = 0; a < i; a++)
  {  
    tflr_bm *spr;
    int     sx, sy;
   
    tx += dx;
    ty += dy;
    sx = (int)(rintf(tx) / SPRITE_SIZE_X);
    sy = (int)(rintf(ty) / SPRITE_SIZE_Y);

    if((sx >= 0) && (sx < floor_ptr->fmap_x_size) &&
       (sy >= 0) && (sy < floor_ptr->fmap_y_size))
    {
      spr =
        flr_bm + *(floor_ptr->fmap + (sy * floor_ptr->fmap_x_size) + sx);

      if(spr->bound == 'y')
        return 0;
    }
    else
      return 1;
  }

  return 1;
}

void tdroid_606::attack(void)
{
  if((state == 0) && !captured)
  {
    if(attack_droid != NULL)
    {
      if(attack_droid->state == 0)
        create_laser(
          (int)(attack_droid->pos_x - pos_x),
          (int)(attack_droid->pos_y - pos_y));

      attack_droid = NULL;
    }
    else
    {
      if((floor_ptr == droids[0]->floor_ptr) && (!droids[0]->state))
      {
        if(!attack_wait)  // Reverted 2.2. JN, 04SEP20
        {
          attack_wait = stats.attack_rate;
          if(target_not_hidden(droids[0]))
          {
            create_laser(
              (int)(droids[0]->pos_x - pos_x),
              (int)(droids[0]->pos_y - pos_y));

            if(!voice_c)
              voice_c = 40;
          }
        }
        else
          attack_wait--;  // Reverted 2.2. JN, 04SEP20
      }
    }
  }

  if(voice_c)
  {
    if(voice_c == 1)
      do_sound(voice_type, 1.0, 1.0);
    voice_c--;
  }
}

/***************************************************************************
*
***************************************************************************/
tdroid_691::tdroid_691(void)
{
  memcpy(&stats, &droid_stats[DROID_691], sizeof(tdroid_stats));
}

tdroid_693::tdroid_693(void)
{
  memcpy(&stats, &droid_stats[DROID_693], sizeof(tdroid_stats));
}

/***************************************************************************
* 7xx and 9xx classes avoid other droids now and stop those nasty knot ups
* that can develop and make droids get stuck on walls etc. JN, 30SEP20
***************************************************************************/
#define D7XX_DIR_RIGHT	1
#define D7XX_DIR_LEFT	2
#define D7XX_DIR_DOWN	3
#define D7XX_DIR_UP	4
#define D7XX_WAIT1	5
#define D7XX_WAIT2	6
#define D7XX_WAIT3	7
#define D7XX_DIR_AVOID	0xff

tdroid_719::tdroid_719(void)
{
	memcpy(&stats, &droid_stats[DROID_719], sizeof(tdroid_stats));
	direction = 0;
	do_nothing_count = 0;
	voice_type = FX_7XX_VOICE;

	blk_detect_ptr = 0;  // JN, 13OCT20
	blk_detect_count = 0;
}

/*
 * New for 4.0. 7xx avoidance. JN, 30SEP20
 */
void tdroid_719::detect_avoid_condition(void)
{
	int	i, avoid_f = FALSE;
	float	vx = 0.0,
		vy = 0.0;

	for (i = 0; i < MAX_DROIDS; i++) {
		float dx, dy, dist;

		if (droids[i] == NULL)
			break;
		if (droids[i]->floor_ptr != floor_ptr)
			continue;
		if (droids[i]->state)
			continue;
		if (droids[i] == this)
			continue;

		/*
		 * Calc distance between droid[this] and droid[other].
		 * Reference (for angle purposes) is droid[other].
		 */
		dx = pos_x - droids[i]->pos_x;
		dy = pos_y - droids[i]->pos_y;
		dist = sqrtf(dx * dx + dy * dy);

		/*
		 * If out of range of avoidance. Ignore. Next droid[other]..
		 */
		if(dist >= SPRITE_SIZE_X)
			continue;

		/*
		 * Accumulate avoidance vector.
		 */
		vx += dx;
		vy += dy;
		avoid_f = TRUE;
	}

	/*
	 * Angle of accumulative vector.
	 */
	if (avoid_f == FALSE)
		return;
	avoid_angle = atan3f(vy, vx);
	direction = D7XX_DIR_AVOID;
}

/*
 * Changed this code around again to stop two 9xx droids in the Mearkat
 * bridge getting into a knot. JN, 14OCT20
 */
void tdroid_719::move(void)
{
	float cpx, cpy;
	tflr_bm *spr;

	queue_dequeue_block_buffer(); // Used by block_detect() JN, 13OCT20

	friction();
	cpx = pos_x;
	cpy = pos_y;

	switch (direction) {
		case D7XX_DIR_RIGHT:
		cpx += SPRITE_HSIZE_X;
		break;

		case D7XX_DIR_LEFT:
		cpx -= SPRITE_HSIZE_X;
		break;

		case D7XX_DIR_DOWN:
		cpy += SPRITE_HSIZE_Y;
		break;

		case D7XX_DIR_UP:
		cpy -= SPRITE_HSIZE_Y;
		break;
	}

	spr =
		flr_bm + *(floor_ptr->fmap +
		((int)(rintf(cpy) / SPRITE_SIZE_Y) * floor_ptr->fmap_x_size) +
		(int)(rintf(cpx) / SPRITE_SIZE_X));

	if (spr->bound == 'y') { // Wall collisions has priority
		direction = random() & 0x7;
	} else {
		/*
		 * If block detection, change direction. If not, then detect
		 * droid avoid condition (no point doing both, as pos_d* needs
		 * time to react). JN, 13OCT20
		 */
		if (block_detect())
			direction = random() & 0x7;
		else
			detect_avoid_condition();
	}

	switch (direction) {
		case D7XX_DIR_RIGHT:
		pos_dx = stats.speed;
		pos_dy = 0;
		break;

		case D7XX_DIR_LEFT:
		pos_dx = -stats.speed;
		pos_dy = 0;
		break;

		case D7XX_DIR_DOWN:
		pos_dx = 0;
		pos_dy = stats.speed;
		break;

		case D7XX_DIR_UP:
		pos_dx = 0;
		pos_dy = -stats.speed;
		break;

		case D7XX_DIR_AVOID:
		pos_dx = stats.speed * cosf(avoid_angle);
		pos_dy = stats.speed * sinf(avoid_angle);
		break;

		default:
		pos_dx = 0.0;
		pos_dy = 0.0;

		if (!do_nothing_count) {
			do_nothing_count = 43 + (random() & 0xf); // JN, 13OCT20
			direction = random() & 0x7;
		} else {
			do_nothing_count--;
		}
		break;
	}
}

void tdroid_719::bg_calc(void)
{
	move();
	tdroid::bg_calc();
	attack();
}

/***************************************************************************
*
***************************************************************************/
tdroid_720::tdroid_720(void)
{
  memcpy(&stats, &droid_stats[DROID_720], sizeof(tdroid_stats));
}

tdroid_766::tdroid_766(void)
{
  memcpy(&stats, &droid_stats[DROID_766], sizeof(tdroid_stats));
}

tdroid_799::tdroid_799(void)
{
  memcpy(&stats, &droid_stats[DROID_799], sizeof(tdroid_stats));
}

/***************************************************************************
*
***************************************************************************/
tdroid_805::tdroid_805(void) 
{
  memcpy(&stats, &droid_stats[DROID_805], sizeof(tdroid_stats));
  evase = 0;
  voice_c = 80;
}

void tdroid_805::attack(void)
{
  if((state == 0) && (attack_droid != NULL))
  {
    if(attack_droid->state == 0)
    {
      if(!attack_wait)  // Reverted 2.2. JN, 04SEP20
      {
        if(evase)
        {
          attack_wait = stats.attack_rate;

          if(target_not_hidden(attack_droid))
            create_laser(
              (int)(attack_droid->pos_x - pos_x),
              (int)(attack_droid->pos_y - pos_y));
        }
        else
        {
          evase = 1;
          do_sound(FX_DROID_EVASE, 1.0, 1.0);
        }
      }
      else
      {
        attack_wait--;  // Reverted 2.2. JN, 04SEP20

        if(evase)
        {
          if(!voice_c)
          {
            voice_c = 80;
            do_sound(FX_8XX_VOICE, 1.0, 1.2);
          }
          else
            voice_c--;
        }
      }
    }
    else
      attack_droid = NULL;
  }
}

/***************************************************************************
*
***************************************************************************/
tdroid_810::tdroid_810(void)
{
  memcpy(&stats, &droid_stats[DROID_810], sizeof(tdroid_stats));
}

tdroid_820::tdroid_820(void)
{
  memcpy(&stats, &droid_stats[DROID_820], sizeof(tdroid_stats));
}

tdroid_899::tdroid_899(void)
{
  memcpy(&stats, &droid_stats[DROID_899], sizeof(tdroid_stats));
}

/***************************************************************************
*
***************************************************************************/
tdroid_933::tdroid_933(void)
{
	memcpy(&stats, &droid_stats[DROID_933], sizeof(tdroid_stats));
	voice_type = FX_9XX_VOICE;
}

tdroid_949::tdroid_949(void)
{
	memcpy(&stats, &droid_stats[DROID_949], sizeof(tdroid_stats));
	voice_type = FX_9XX_VOICE;
}

tdroid_999::tdroid_999(void)
{
	memcpy(&stats, &droid_stats[DROID_999], sizeof(tdroid_stats));
	voice_type = FX_9XX_VOICE;
}
