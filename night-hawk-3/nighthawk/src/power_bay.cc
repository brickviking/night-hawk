/***************************************************************************
****************************************************************************
****************************************************************************
* Nighthawk
* Copyright (C) 1996-2004 Jason Nunn et al
* Developed by Jason Nunn et al (see README for Acknowledgements)
*
* Project head and maintainer--
*   Eric Gillespie (The Viking)
*   Email: viking667@users.sourceforge.net
*   http://night-hawk.sourceforge.net
*
* Original Author--
*   Jason Nunn
*   http://jsno.leal.com.au
*
*   C/- Jason Nunn
*   PO Box 15
*   Birdwood SA 5234
*   Australia
*
* This software is Open Source GPL. Please read COPYING notice.
* ==================================================================
* power bay routines
*
****************************************************************************
****************************************************************************
***************************************************************************/
#include "defs.h"
#include "protos.p"

#include "tfloor.h"

/***************************************************************************
*
***************************************************************************/
tpower_bay::tpower_bay(void)
{
  anim_len = 5.0;
  anim_ptr = 0.0;
  anim_speed = 0.2;
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
  blit_bm(&bm[(int)anim_ptr], ix, iy);
}

void tpower_bay::bg_calc(void)
{
  anim_ptr += anim_speed * get_time_lapse();
  anim_ptr = fmod(anim_ptr, anim_len);
}
