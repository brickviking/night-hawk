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
* power bay routines
*
****************************************************************************
****************************************************************************
***************************************************************************/
extern "C" {
#include "defs.h"
#include "sprite_loader_externs.h"
#include "opengl_externs.h"
}
#include "tfloor.h"

/***************************************************************************
*
***************************************************************************/
tpower_bay::tpower_bay(void)
{
  anim_len = 5;  // Reverted 2.2. JN, 04SEP20
  anim_ptr = 0;
  anim_speed = POWER_BAY_SPD;

  bm = &power_bay_bm[0];
}

void tpower_bay::init(int px, int py)
{
  pos_x = px;
  pos_y = py;
}

void tpower_bay::draw(int px, int py)
{
  int ix, iy;

  ix = (pos_x - px) + SCREEN_HSIZE_X - SPRITE_HSIZE_X;
  iy = (pos_y - py) + SCREEN_HSIZE_Y - SPRITE_HSIZE_Y;

 /*
  * don't need to set colour, set in floor draw
  */
  blit_bm(&bm[anim_ptr >> 4], ix, iy); // JN, 04SEP20
}

void tpower_bay::bg_calc(void)
{
  anim_ptr += anim_speed; // Reverted 2.2. JN, 04SEP20
  if((anim_ptr >> 4) == anim_len) anim_ptr = 0;
}
