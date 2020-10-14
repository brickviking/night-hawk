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
* laser object routines
*
****************************************************************************
****************************************************************************
***************************************************************************/
#include "defs.h"
#include "protos.p"

#include "tlaser.h"
#include "tdroid.h"
#include "tparadroid.h"

/***************************************************************************
*
***************************************************************************/
tlaser::tlaser(void)
{
}

int tlaser::init_ssv(float d, int s1, int s2, int s3)
{
  if((d >= ((M_PI_2 / 3.0) * 1.0)) && (d < ((M_PI_2 / 3.0) * 2.0)))
    return s2;
  else if((d >= ((M_PI_2 / 3.0) * 2)) && (d < ((M_PI_2 / 3.0) * 3.0)))
    return s3;

  return s1;
}

void tlaser::init(
  tfloor *f, tentity **dp, float px, float py, int vx, int vy, tentity *o)
{
  float d, tvx;

  laser_bm_s = 0;
  tvx = (float)abs(vx);
  if(vy != 0)
    d = atan(tvx / (float)abs(vy));
  else
    d = atan(tvx);

  pos_dx = sin(d) * anim_speed * 12;
  pos_dy = cos(d) * anim_speed * 12;

  if    ((vx < 0) && (vy < 0))
  {
    pos_dx *= -1.0;
    pos_dy *= -1.0;
    laser_bm_s = init_ssv(d, 1, 2, 0);
  }
  else if((vx >= 0) && (vy < 0))
  {
    pos_dy *= -1.0;
    laser_bm_s = init_ssv(d, 1, 3, 0);
  }
  else if((vx >= 0) && (vy >= 0))
  {
    laser_bm_s = init_ssv(d, 1, 2, 0);
  }
  else if((vx < 0) && (vy >= 0))
  {
    pos_dx *= -1;
    laser_bm_s = init_ssv(d, 1, 3, 0);
  }
  owner = o;
  tentity::init(f, &laser_l_bm[0], dp, 7, px, py);
  if(owner == *droids)
    buzzed = 1;
  else
  {
    calc_buzz();
    old_pos_x_sign = pos_x_sign;
    old_pos_y_sign = pos_y_sign;
    buzzed = 0;
  }
}

int tlaser::colision_droids(float cx, float cy)
{
  int x;

  for(x = 0; x < MAX_DROIDS; x++)
    if(*(droids + x) != NULL)
    {
      if((*(droids + x) != owner) && !(*(droids + x))->state &&
         ((*(droids + x))->floor_ptr == floor_ptr))
      {
        float ix, iy;

        ix = cx - (*(droids + x))->pos_x + SPRITE_HSIZE_X;
        iy = cy - (*(droids + x))->pos_y + SPRITE_HSIZE_Y;
        if((ix >= 0) && (ix < SPRITE_SIZE_X) &&
           (iy >= 0) && (iy < SPRITE_SIZE_Y))
        {
          ((tdroid *)*(droids + x))->hit(this);
          return 1;
        }
      }
    }
    else
      break;

  return 0;
}

void tlaser::colision(float cx, float cy)
{
  int status = 0;

//test other
  if(colision_other(cx, cy))
  {
    state = 1;
    status = 1;
//    do_sound(FX_EXPLOSION_1,256);
  }

//test for droids
  if(colision_droids(cx, cy))
  {
    state = 2;
    status = 1;
  }

  if(!status)
  {
    pos_x += pos_dx * get_time_lapse();
    pos_y += pos_dy * get_time_lapse();
  }
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

void tlaser::buzz(void)
{
  if(!buzzed)
    if(!state)
    {
      calc_buzz();
      if((pos_x_sign != old_pos_x_sign) || (pos_y_sign != old_pos_y_sign))
      {
        float d1, d2;

        d1 = (*droids)->pos_x - pos_x;
        d2 = (*droids)->pos_y - pos_y;
        if(((d1 * d1) + (d2 * d2)) < (50 * 50))
        {
          buzzed = 1;
//          do_sound(FX_LASER_BUZZ,800);
          console_message_add("buzzed!");
        }
      }
      old_pos_x_sign = pos_x_sign;
      old_pos_y_sign = pos_y_sign;
    }
}

void tlaser::draw(tfloor *c_f_ptr, int ppx, int ppy)
{
  int ix, iy;

  if(floor_ptr != c_f_ptr)
    return;

  switch(state)
  {
    case 0:
      ix = (spos_x - ppx) + (SCREEN_HSIZE_X - 7);
      iy = (spos_y - ppy) + (SCREEN_HSIZE_Y - 7);
      glColor4fv(bm_colour);
      blit_bm(&bm[laser_bm_s], ix, iy);
      break;

    case 1:
      if(!reduced_f)
      {
        ix = (spos_x - ppx) + (SCREEN_HSIZE_X - 20);
        iy = (spos_y - ppy) + (SCREEN_HSIZE_Y - 20);
        glColor4fv(bm_colour);
        blit_bm(&explosion_bm[(int)ss_anim_ptr], ix, iy);
      }
      break;
  }
}

void tlaser::bg_calc(void)
{
  switch(state)
  {
    case 0:
      colision(pos_x, pos_y);
      buzz();
      break;

    case 1:
      if(!reduced_f)
      {
        anim_ptr += anim_speed * get_time_lapse();
        if(anim_ptr >= anim_len)
          state++;
      }
      else
        state++;
      break;
  }
}

/***************************************************************************
*
***************************************************************************/
tlaser0::tlaser0(void)
{
  anim_speed = 0.5;
  damage_factor = 4;
}

void tlaser0::init(
  tfloor *f, tentity **dp, float px, float py, int vx, int vy, tentity *o)
{
  tlaser::init(f, dp, px, py, vx, vy, o);
  bm = &laser_l_bm[0];
//  do_sound(FX_LASER_0,100);
}

/***************************************************************************
*
***************************************************************************/
tlaser1::tlaser1(void)
{
  anim_speed = 0.625;
  damage_factor = 10;
}

void tlaser1::init(
  tfloor *f, tentity **dp, float px, float py, int vx, int vy, tentity *o)
{
  tlaser::init(f, dp, px, py, vx, vy, o);
  bm = &laser_cb_bm[0];
//  do_sound(FX_LASER_1,100);
}

/***************************************************************************
*
***************************************************************************/
tlaser2::tlaser2(void)
{
  anim_speed = 0.75;
  damage_factor = 20;
}

void tlaser2::init(
  tfloor *f, tentity **dp, float px, float py, int vx, int vy, tentity *o)
{
  tlaser::init(f, dp, px, py, vx, vy, o);
  bm = &laser_uv_bm[0];
//  do_sound(FX_LASER_2,100);
}

/***************************************************************************
*
***************************************************************************/
tlaser3::tlaser3(void)
{
  anim_speed = 0.5625;
  damage_factor = 50;
}

void tlaser3::init(
  tfloor *f, tentity **dp, float px, float py, int vx, int vy, tentity *o)
{
  tlaser::init(f, dp, px, py, vx, vy, o);
  bm = &laser_te_bm[0];
//  do_sound(FX_LASER_3,100);
}
