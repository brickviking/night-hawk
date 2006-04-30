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
* entity object routines.. this is the daddy class for all players
*
****************************************************************************
****************************************************************************
***************************************************************************/
#include "defs.h"
#include "protos.p"

#include "tentity.h"

/***************************************************************************
*
***************************************************************************/
tentity::tentity(void)
{
  state = 0;
  anim_len = anim_ptr = 0.0;
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
  old_pos_x = pos_x = px;
  old_pos_y = pos_y = py;
}

void tentity::snap_shot(void)
{
  spos_x = (int)rint(pos_x);
  spos_y = (int)rint(pos_y);
  ss_anim_ptr = anim_ptr;
}

/*
void tentity::do_sound(int fx_type,int grad)
{
  if(floor_ptr == (*droids)->floor_ptr)
  {
    float dx,dy;
    int vol;

    dx = pos_x - (*droids)->pos_x;
    dy = pos_y - (*droids)->pos_y;
    vol = (int)sqrt((dx * dx) + (dy * dy));
    vol = 255 - ((vol * grad) >> 8);
    if(vol > 0)
    {
      int bal;

      bal = (int)dx + 128;
      if(bal < 0) bal = 0;
      if(bal > 255) bal = 255;
      sound_engine_cmd(SND_CMD_FX,fx_type,vol,bal);
    }
  }
}
*/

int tentity::colision_other(float cx, float cy)
{
  int     x, status = 0;
  tflr_bm *spr;

  cx = rint(cx);
  cy = rint(cy);

// test for floor colision
  spr =
    flr_bm + *(floor_ptr->fmap + 
    ((int)(cy / SPRITE_SIZE_Y) * floor_ptr->fmap_x_size) +
     (int)(cx / SPRITE_SIZE_X));
  if(spr->bound == 'y')
    status = 1;

//test for doors
  for(x = 0; x < MAX_DOORS; x++)
    if(floor_ptr->door[x] != NULL)
    {
      float ix, iy;

      ix = cx - floor_ptr->door[x]->pos_x + SPRITE_HSIZE_X;
      iy = cy - floor_ptr->door[x]->pos_y + SPRITE_HSIZE_Y;
      if((ix >= 0) && (ix < SPRITE_SIZE_X) &&
         (iy >= 0) && (iy < SPRITE_SIZE_Y))
      {
        if(floor_ptr->door[x]->anim_ptr < 5.0)
        {
          floor_ptr->door[x]->open();
//          if(floor_ptr->door[x]->open())
//            do_sound(FX_OPEN_DOOR,400);
          status = 1;
        }
      }
    }
    else
      break;

  return status;
}

int tentity::colision_droids(float cx, float cy)
{
  int x;

  for(x = 0; x < MAX_DROIDS; x++)
    if(*(droids + x) != NULL)
    {
      if((*(droids + x) != this) && !(*(droids + x))->state &&
        ((*(droids + x))->floor_ptr == floor_ptr))
      {
        float ix, iy;

        ix = cx - (*(droids + x))->pos_x + SPRITE_HSIZE_X;
        iy = cy - (*(droids + x))->pos_y + SPRITE_HSIZE_Y;
        if((ix >= 0) && (ix < SPRITE_SIZE_X) &&
           (iy >= 0) && (iy < SPRITE_SIZE_Y))
        {
          ix = ((*(droids + x))->pos_x - pos_x) / 4;
          iy = ((*(droids + x))->pos_y - pos_y) / 4;
          pos_dx = -ix;
          pos_dy = -iy;
          (*(droids + x))->pos_dx = ix;
          (*(droids + x))->pos_dy = iy;
        }
      }
    }
    else
      break;

  return 1;
}

void tentity::colision(float cx, float cy)
{
//test other
  if(colision_other(cx, cy))
  {
    pos_dx = old_pos_x - pos_x;
    pos_dy = old_pos_y - pos_y;
  }
  else
  {
    old_pos_x = pos_x;
    old_pos_y = pos_y;
  }

//test for droids
  colision_droids(cx, cy);
}

void tentity::bg_calc(void)
{
  anim_ptr += anim_speed * get_time_lapse();
  anim_ptr = fmod(anim_ptr, anim_len);

  colision(pos_x, pos_y);

  pos_x += pos_dx * get_time_lapse();
  pos_y += pos_dy * get_time_lapse();
}
