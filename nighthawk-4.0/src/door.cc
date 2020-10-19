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
* door object routines
*
****************************************************************************
****************************************************************************
***************************************************************************/
extern "C" {
#include "defs.h"
#include "misc_externs.h"
#include "sprite_loader_externs.h"
#include "opengl_externs.h"
}
#include "tfloor.h"

/***************************************************************************
*
***************************************************************************/
tdoor::tdoor(void)
{
  anim_len = 9;  // Reverted 2.2. JN, 04SEP20
  anim_ptr = 0;
  anim_speed = DOOR_SPD;
  state = 0;
}

void tdoor::init(int px, int py, int t)
{
	pos_x = px;
	pos_y = py;
	type = t;
	if (t == DOOR_HORIZONTAL)
		bm = &doorh_bm[0];
	else
		bm = &doorv_bm[0];
}

int tdoor::open(void)
{
	if (!state) {
		state++;
		return 1;
	}

	return 0;
}

/*
 * Reverted method to 2.2. JN, 04SEP20
 */
void tdoor::bg_calc(void)
{
  switch(state)
  {
    case 1:
      if((anim_ptr >> 4) < (anim_len - 1))
        anim_ptr += anim_speed;
      else
      {
        wait_t = 0;
        state++;
      }
      break;
    case 2:
      wait_t++;
      if(wait_t > 50)
        state++;
      break;
    case 3:
      if((anim_ptr >> 4) > 0)
        anim_ptr -= anim_speed;
      else
        state = 0;
  }
}
