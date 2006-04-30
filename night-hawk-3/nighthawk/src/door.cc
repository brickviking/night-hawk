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
* door object routines
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
tdoor::tdoor(void)
{
  anim_len = 9.0;
  anim_ptr = 0;
  anim_speed = 1.0;
  state = 0;
}

void tdoor::init(int px, int py, int t)
{
  pos_x = px; 
  pos_y = py;
  if(t)
    bm = &doorh_bm[0];
  else
    bm = &doorv_bm[0];
}

int tdoor::open(void)
{
  if(!state)
  {
    state++;
    return 1;
  }
  return 0;
}

void tdoor::bg_calc(void)
{
  switch(state)
  {
    case 1:
      anim_ptr += anim_speed * get_time_lapse();
      if(anim_ptr >= anim_len)
      {
        anim_ptr = anim_len - 1.0;
        wait_t = 0.0;
        state++;
      }
      break;

    case 2:
      wait_t += get_time_lapse();
      if(wait_t > 50.0)
        state++;
      break;

    case 3:
      anim_ptr -= anim_speed * get_time_lapse();
      if(anim_ptr <= 0)
      {
        anim_ptr = 0.0;
        state = 0;
      }
  }
}
