/***************************************************************************
****************************************************************************
****************************************************************************
*
* NightHawk - By Jason Nunn - Oct 96 (jsno@downunder.net.au)
* Freeware
*
* ==================================================================
* object routines
*
****************************************************************************
****************************************************************************
***************************************************************************/
extern "C" {
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>
/******************
 * Added this ECG *
 ******************/
#include <string.h>
}
#include "nighthawk_defs.h"
#include "misc.h"
#include "tedit_floor.h"
#include "tparadroid.h"
#include "tdroid_1xx.h"
#include "tdroid_2xx.h"
#include "tdroid_3xx.h"
#include "tdroid_4xx.h"
#include "tdroid_5xx.h"
#include "tdroid_6xx.h"
#include "tdroid_7xx.h"
#include "tdroid_8xx.h"
#include "tdroid_9xx.h"

/***************************************************************************
*
***************************************************************************/
tpower_bay::tpower_bay(void)
{
  anim_len = 5;
  anim_ptr = 0;
  anim_speed = POWER_BAY_SPD;
  bm = &power_bay_bm;
}

void tpower_bay::init(int px,int py)
{
  pos_x = px; 
  pos_y = py;
}

void tpower_bay::draw(int px,int py)
{
  register int ix,iy;

  ix = (pos_x - px) + SCREEN_HSIZE_X - SPRITE_HSIZE_X;
  iy = (pos_y - py) + SCREEN_HSIZE_Y - SPRITE_HSIZE_Y;
  drawxpm_ani(bm,ix,iy,anim_ptr >> 4,anim_len);
}

void tpower_bay::bg_calc(void)
{
  anim_ptr += anim_speed;
  if((anim_ptr >> 4) == anim_len) anim_ptr = 0;
}     

/***************************************************************************
*
***************************************************************************/
tdoor::tdoor(void)
{
  anim_len = 9;
  anim_ptr = 0;
  anim_speed = DOOR_SPD;
  state = 0;
}

void tdoor::init(int px,int py,int t)
{
  pos_x = px; 
  pos_y = py;
  if(t)
    bm = &doorh_bm;
  else
    bm = &doorv_bm;
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

/***************************************************************************
*
***************************************************************************/
tentity::tentity(void)
{
  state = 0;
  anim_len = anim_ptr = 0;
  pos_dx = pos_dy = 0;
  anim_speed = GENERAL_SPD;
}

tentity::~tentity(void)
{
}

void tentity::init(
  tfloor *f,tbm *bm_p,tentity **dp,int alen,float px,float py)
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

void tentity::do_sound(int fx_type,int grad)
{
  if(floor_ptr == (*droids)->floor_ptr)
  {
    float dx,dy;
    register int vol;

    dx = pos_x - (*droids)->pos_x;
    dy = pos_y - (*droids)->pos_y;
    vol = (int)sqrt((dx * dx) + (dy * dy));
    vol = 255 - ((vol * grad) >> 8);
    if(vol > 0)
    {
      register int bal;

      bal = (int)dx + 128;
      if(bal < 0) bal = 0;
      if(bal > 255) bal = 255;
      sound_engine_cmd(SND_CMD_FX,fx_type,vol,bal);
    }
  }
}

int tentity::colision_other(float cx,float cy)
{
  register int x,status = 0;

  cx = rint(cx);
  cy = rint(cy);

// test for floor colision
  tsprite *spr =
    flr_sprites + *(floor_ptr->fmap + 
    ((int)(cy / SPRITE_SIZE_Y) * floor_ptr->fmap_x_size) +
     (int)(cx / SPRITE_SIZE_X));
  if(spr->bound == 'y')
    status = 1;

//test for doors
  for(x = 0;x < MAX_DOORS;x++)
    if(floor_ptr->door[x] != NULL)
    {
      float ix,iy;

      ix = cx - floor_ptr->door[x]->pos_x + SPRITE_HSIZE_X;
      iy = cy - floor_ptr->door[x]->pos_y + SPRITE_HSIZE_Y;
      if((ix >= 0) && (ix < SPRITE_SIZE_X) &&
         (iy >= 0) && (iy < SPRITE_SIZE_Y))
        if((floor_ptr->door[x]->anim_ptr >> 4) < 5)
        {
          if(floor_ptr->door[x]->open())
            do_sound(FX_OPEN_DOOR,400);
          status = 1;
        }
    }
    else
      break;

  return status;
}

int tentity::colision_droids(float cx,float cy)
{
  register int x;

  for(x = 0;x < MAX_DROIDS;x++)
    if(*(droids + x) != NULL)
    {
      if((*(droids + x) != this) && !(*(droids + x))->state &&
        ((*(droids + x))->floor_ptr == floor_ptr))
      {
        float ix,iy;

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

void tentity::colision(float cx,float cy)
{
//test other
  if(colision_other(cx,cy))
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
  colision_droids(cx,cy);
}

void tentity::bg_calc(void)
{
  anim_ptr += anim_speed;
  if((anim_ptr >> 4) == anim_len) anim_ptr = 0;
  colision(pos_x,pos_y);
  pos_x += pos_dx;
  pos_y += pos_dy;
}

/***************************************************************************
*
***************************************************************************/
tlaser::tlaser(void)
{
}

int tlaser::init_ssv(float d,int s1,int s2,int s3)
{
  if((d >= ((M_PI_2 / 3) * 1)) && (d < ((M_PI_2 / 3) * 2)))
    return s2;
  else if((d >= ((M_PI_2 / 3) * 2)) && (d < ((M_PI_2 / 3) * 3)))
    return s3;
  return s1;
}

void tlaser::init(
  tfloor *f,tentity **dp,float px,float py,int vx,int vy,tentity *o)
{
  float d,tvx;

  laser_bm_s = 0;
  tvx = (float)abs(vx);
  if(vy != 0)
    d = atan(tvx / (float)abs(vy));
  else
    d = atan(tvx);
  pos_dx = sin(d) * anim_speed;
  pos_dy = cos(d) * anim_speed;
  if    ((vx < 0) && (vy < 0))
  {
    pos_dx *= -1;
    pos_dy *= -1;
    laser_bm_s = init_ssv(d,1,2,0);
  }
  else if((vx >= 0) && (vy < 0))
  {
    pos_dy *= -1;
    laser_bm_s = init_ssv(d,1,3,0);
  }
  else if((vx >= 0) && (vy >= 0))
  {
    laser_bm_s = init_ssv(d,1,2,0);
  }
  else if((vx < 0) && (vy >= 0))
  {
    pos_dx *= -1;
    laser_bm_s = init_ssv(d,1,3,0);
  }
  owner = o;
  tentity::init(f,&laser_l_bm,dp,7,px,py);
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

int tlaser::colision_droids(float cx,float cy)
{
  register int x;

  for(x = 0;x < MAX_DROIDS;x++)   
    if(*(droids + x) != NULL)
    {
      if((*(droids + x) != owner) && !(*(droids + x))->state &&
         ((*(droids + x))->floor_ptr == floor_ptr))
      {
        float ix,iy;

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

void tlaser::colision(float cx,float cy)
{
  register int status = 0;
//test other
  if(colision_other(cx,cy))
  {
    state = 1;
    status = 1;
    do_sound(FX_EXPLOSION_1,256);
  }
//test for droids
  if(colision_droids(cx,cy))
  {
    state = 2;
    status = 1;
  }
  if(!status)
  {
    pos_x += pos_dx;
    pos_y += pos_dy;
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
        register float d1,d2;

        d1 = (*droids)->pos_x - pos_x;
        d2 = (*droids)->pos_y - pos_y;
        if(((d1 * d1) + (d2 * d2)) < (50 * 50))
        {
          buzzed = 1;
          do_sound(FX_LASER_BUZZ,800);
          ((tparadroid *)droids[0])->add_message("buzzed!");
        }
      }
      old_pos_x_sign = pos_x_sign;
      old_pos_y_sign = pos_y_sign;
    }
}

void tlaser::draw(tfloor *c_f_ptr,int ppx,int ppy)
{
  if(floor_ptr == c_f_ptr)
  {
    register int ix,iy;

    switch(state)
    {
      case 0:
        ix = (spos_x - ppx) + (SCREEN_HSIZE_X - 7);
        iy = (spos_y - ppy) + (SCREEN_HSIZE_Y - 7);
        drawxpm_ani(bm,ix,iy,laser_bm_s,4);
        break;
#ifndef REDUCED_SPRITES
      case 1:
        ix = (spos_x - ppx) + (SCREEN_HSIZE_X - 20);
        iy = (spos_y - ppy) + (SCREEN_HSIZE_Y - 20);
        drawxpm_ani(&explosion_bm,ix,iy,ss_anim_ptr >> 4,anim_len);
        break;
#endif
    }
  }
}

void tlaser::bg_calc(void)
{
  switch(state)
  {
    case 0:
      colision(pos_x,pos_y);
      buzz();
      break;
    case 1:
#ifndef REDUCED_SPRITES
      anim_ptr += anim_speed;
      if((anim_ptr >> 4) == anim_len)
#endif
        state++;
      break;
  }
}

/***************************************************************************
*
***************************************************************************/
tlaser0::tlaser0(void)
{
  anim_speed = 8;
  damage_factor = 4;
}

void tlaser0::init(
  tfloor *f,tentity **dp,float px,float py,int vx,int vy,tentity *o)
{
  tlaser::init(f,dp,px,py,vx,vy,o);
  bm = &laser_l_bm;
  do_sound(FX_LASER_0,100);
}

/***************************************************************************
*
***************************************************************************/
tlaser1::tlaser1(void)
{
  anim_speed = 10;
  damage_factor = 10;
}

void tlaser1::init(
  tfloor *f,tentity **dp,float px,float py,int vx,int vy,tentity *o)
{
  tlaser::init(f,dp,px,py,vx,vy,o);
  bm = &laser_cb_bm;
  do_sound(FX_LASER_1,100);
}

/***************************************************************************
*
***************************************************************************/
tlaser2::tlaser2(void)
{
  anim_speed = 12;
  damage_factor = 20;
}

void tlaser2::init(
  tfloor *f,tentity **dp,float px,float py,int vx,int vy,tentity *o)
{
  tlaser::init(f,dp,px,py,vx,vy,o);
  bm = &laser_uv_bm;
  do_sound(FX_LASER_2,100);
}

/***************************************************************************
*
***************************************************************************/
tlaser3::tlaser3(void)
{
  anim_speed = 9;
  damage_factor = 50;
}

void tlaser3::init(
  tfloor *f,tentity **dp,float px,float py,int vx,int vy,tentity *o)
{
  tlaser::init(f,dp,px,py,vx,vy,o);
  bm = &laser_te_bm;
  do_sound(FX_LASER_3,100);
}

/***************************************************************************
* 1xx Cleaning
* 2xx Logistic/Servant
* 3xx Messenger
* 4xx Maintenance
* 5xx Medical
* 6xx Sentinel
* 7xx Battle
* 8xx Navigational
* 9xx Command
***************************************************************************/
tdroid::tdroid(void)
{
  register int x;

  for(x = 0;x < MAX_LASERS;x++)
    laser_list[x] = NULL;
  expl_anim_ptr = 0;
  expl_anim_speed = 10;
  expl_anim_len = 7;
  shield_wave_inc = 0;
  captured = 0;
}

void tdroid::init(tfloor *f,tentity **dp,float px,float py)
{
  tentity::init(f,&droid_bm,dp,10,px,py);
}

int tdroid::create_laser(int vx,int vy)
{
  register int x,status = 0;

  for(x = 0;x < MAX_LASERS;x++)
    if(laser_list[x] == NULL)
    {
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
      if(laser_list[x] != NULL)
        laser_list[x]->init(
          floor_ptr,droids,pos_x,pos_y,vx,vy,this);
      break;
    }
  return status;
}

void tdroid::free_all_laser(void)
{
  register int x;
     
  for(x = 0;x < MAX_LASERS;x++)
    if(laser_list[x] != NULL)
    {
      delete(laser_list[x]);
      laser_list[x] = NULL;
    }
}

void tdroid::snap_shot(void)
{
  register int x;

  tentity::snap_shot();
  ss_expl_anim_ptr = expl_anim_ptr;
  for(x = 0;x < MAX_LASERS;x++)
    if(laser_list[x] != NULL)
      laser_list[x]->snap_shot();
}


void tdroid::draw_laser(tfloor *c_f_ptr,int px,int py)
{
  register int x;

  for(x = 0;x < MAX_LASERS;x++)
    if(laser_list[x] != NULL)
      laser_list[x]->draw(c_f_ptr,px,py);
}

void tdroid::bg_calc_laser(void)
{
  register int x;

  for(x = 0;x < MAX_LASERS;x++)
    if(laser_list[x] != NULL)
    {
      laser_list[x]->bg_calc();
      if(laser_list[x]->state == 2)
      {
        delete(laser_list[x]);
        laser_list[x] = NULL;
      }
    }
}

void tdroid::shields_low(int ix,int iy)
{
  register int a;
  unsigned long *p;

  if(stats.shielding <= (stats.max_shielding >> 3))
    p = red_pixel;
  else
    if(stats.shielding <= (stats.max_shielding >> 2))
      p = green_pixel;
    else
      return;
  XSetForeground(display,gc_line,
    *(p + (int)((sin(shield_wave_inc) * (MAX_SCORE_TABLE >> 1)) +
    (MAX_SCORE_TABLE >> 1))));
  for(a = 0;a < 4;a++)
  {
    rech_tmp[a].x = rech_pos1[a].x + ix;
    rech_tmp[a].y = rech_pos1[a].y + iy;
  }
  XDrawRectangles(display,render_screen,gc_line,rech_tmp,4);
}

void tdroid::draw(tfloor *c_f_ptr,int px,int py)
{
  draw_laser(c_f_ptr,px,py);
  if(floor_ptr == c_f_ptr)
  {
    register int ix,iy,x;

    switch(state)
    {
      case 0:
        ix = (spos_x - px) + SCREEN_HSIZE_X;
        iy = (spos_y - py) + SCREEN_HSIZE_Y;
        drawxpm_ani(
          bm,ix - SPRITE_HSIZE_X,iy - SPRITE_HSIZE_Y,
          ss_anim_ptr >> 4,anim_len);
        for(x = 0;x < 3;x++)
          drawxpm_ani(
            &digit_bm,
            ix - 7 + (x * 5),
            iy - 3,(int)(stats.type[x] - '0'),10);
        shields_low(ix,iy);
        break;
      case 1:
        ix = (spos_x - px) + (SCREEN_HSIZE_X - 20);
        iy = (spos_y - py) + (SCREEN_HSIZE_Y - 20);
        drawxpm_ani(
          &explosion_bm,ix,iy,ss_expl_anim_ptr >> 4,expl_anim_len);
        break;
    }
  }
}

void tdroid::bg_calc(void)
{
  register int spin;

  spin = (stats.shielding * 15) / stats.max_shielding;
  switch(state)
  {
    case 0:
      anim_speed = spin;
      shield_wave_inc += 0.3;
      tentity::bg_calc();
      break;
    case 1:
      anim_speed = spin;
      tentity::bg_calc();
      expl_anim_ptr += expl_anim_speed;
      if((expl_anim_ptr >> 4) == expl_anim_len)
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

void tdroid::display_nd(void)
{
  char str[STR_LABEL_LEN];
  register int a,nd = 0;

  for(a = 1;a < MAX_DROIDS;a++)
    if(*(droids + a) != NULL)
      if(!(*(droids + a))->state)
        nd++;
  sprintf(str,"droids=%d",nd);
  ((tparadroid *)droids[0])->add_message(str);
}

void tdroid::hit(tlaser *l)
{
  char str[STR_LABEL_LEN];

  pos_dx = l->pos_dx / 4;
  pos_dy = l->pos_dy / 4;
  stats.shielding -= l->damage_factor;
  if(stats.shielding < 0)
  {
    state = 1;
    sprintf(str,"%s d %s",
      l->owner->stats.type,
      stats.type);
    ((tparadroid *)droids[0])->add_message(str);
    if(this != droids[0])
      score += stats.entry * 25;
    display_nd();
    if(random() & 1)
      do_sound(FX_EXPLOSION_1,40);
    else
      do_sound(FX_EXPLOSION_2,40);
  }
  else
  {
    attack_droid = l->owner;
    sprintf(str,"%s h %s(%d)",
      l->owner->stats.type,
      stats.type,
      stats.shielding);
    ((tparadroid *)droids[0])->add_message(str);
    if(this != droids[0])
      score += stats.entry;
    do_sound(FX_DROID_HIT_1,50);
  }
}

/***************************************************************************
*
***************************************************************************/
tparadroid::tparadroid(void)
{
  register int x;

  memcpy(&stats,&droid_stats[DROID_002],sizeof(tdroid_stats));
  transferring = 0;
  flr_move = 0;
  for(x = 0;x < MAX_MESSAGES;x++)
    *(message + x) = NULL;
  captured_droid = NULL;
  low_shields_trig = 0;
}

tparadroid::~tparadroid(void)
{ 
  register int x;

  for(x = 0;x < MAX_MESSAGES;x++)
    if(*(message + x) != NULL)
      free(*(message + x));
}

void tparadroid::init(tfloor *f,tentity **dp,float px,float py)
{
  tentity::init(f,&paradroid_bm,dp,10,px,py);
}

void tparadroid::draw(void)
{
  register int x;

  draw_laser(floor_ptr,(int)pos_x,(int)pos_y);
  switch(state)
  {
    case 0:
      drawxpm_ani(
        bm,
        SCREEN_HSIZE_X - SPRITE_HSIZE_X,
        SCREEN_HSIZE_Y - SPRITE_HSIZE_Y,
        ss_anim_ptr >> 4,anim_len);
      for(x = 0;x < 3;x++)
        drawxpm_ani(
          &digit_bm,
          SCREEN_HSIZE_X - 7 + (x * 5),
          SCREEN_HSIZE_Y - 3,(int)(stats.type[x] - '0'),10);
      shields_low(SCREEN_HSIZE_X,SCREEN_HSIZE_Y);
      if(transferring)
      {
        XSetForeground(display,gc_line,
          white_pixel[(int)((sin(shield_wave_inc) * (8 >> 1)) + (8 >> 1))]);
        XDrawRectangles(display,render_screen,gc_line,rech_pos2,4);
        if(captured_droid != NULL)
        {
          register int np,nv;

          np = SCREEN_HSIZE_X - (neg_bm.width >> 1);
          XSetForeground(display,gc_dline,red_pixel[0]);
          XDrawLine(display,render_screen,gc_dline,
            SCREEN_HSIZE_X,
            SCREEN_HSIZE_Y,
            SCREEN_HSIZE_X + captured_droid->spos_x - spos_x,
            SCREEN_HSIZE_Y + captured_droid->spos_y - spos_y);
          drawxpm_ani(&neg_bm,np,50,0,1);
          XSetForeground(display,gc_line,green_pixel[0]);
          nv = ((tug1 + -tug2) >> 2) + 20;
          if(nv < 0) nv = 0;
          if(nv > 40) nv = 40;
          nv += 2;
          XDrawLine(display,render_screen,gc_line,
            np + 2,62,np + nv,62);
        }
      }
      break;
    case 1:
      drawxpm_ani(
        &explosion_bm,
        SCREEN_HSIZE_X - 20,
        SCREEN_HSIZE_Y - 20,ss_expl_anim_ptr >> 4,expl_anim_len);
      break;
  }
}

int tparadroid::within_capt_dist(void)
{
  register int d1,d2;

  d1 = (int)(captured_droid->pos_x - pos_x);
  d2 = (int)(captured_droid->pos_y - pos_y);
  if(((d1 * d1) + (d2 * d2)) > (100 * 100))
  {
    captured_droid->captured = 0;
    captured_droid = NULL;
    return 0;
  }
  return 1;
}

void tparadroid::bg_calc(void)
{ 
  char Shield_str[STR_LABEL_LEN]; // Added by Dion Bonner, 14-Dec-2001 to feed Shields message
  tdroid::bg_calc();
  if(!state)
  {
    if(!flr_move)
      friction();
    if(stats.shielding <= (stats.max_shielding >> 2))
    {
      if(!low_shields_trig)
      {
        low_shields_trig = 25;
        sound_engine_cmd(SND_CMD_FX,FX_LOW_SHIELDS,0xff,0x80);
      }
      else
        low_shields_trig--;
    }
    if(captured_droid != NULL)
      if(!within_capt_dist())
        add_message("lost contact");
      else
        if(captured_droid->state)
        {
          add_message("Forced detach");
          captured_droid = NULL;
        }
        else
        {
          register int tt;
          float handicap[10] = {6.0,5.5,5.0,4.5,4.0,
                               3.5,3.2,3.0,2.7,2.5};
          tug1 +=
            (int)rint(handicap[stats.entry] *
            (float)stats.entry * ((float)random() / RAND_MAX));
          tug2 +=
            (int)rint(2.0 * (float)captured_droid->stats.entry *
           ((float)random() / RAND_MAX));
          tt = tug1 + -tug2;
          if(tt <= -80)
          {
            add_message("Burn out!");
            state = 1;
            captured_droid = NULL;
            do_sound(FX_EXPLOSION_1,40);
          }
          else
            if(tt >= 80)
            {
              float tx,ty,tdx,tdy;

              score += captured_droid->stats.entry * 50;
              memcpy(&stats,
                &(captured_droid->stats),sizeof(tdroid_stats));
              tx = pos_x;
              ty = pos_y;
              tdx = pos_dx;
              tdy = pos_dy;
              pos_x = captured_droid->pos_x;
              pos_y = captured_droid->pos_y;
              pos_dx = captured_droid->pos_dx;
              pos_dy = captured_droid->pos_dy;
              captured_droid->pos_x = tx;
              captured_droid->pos_y = ty;
              captured_droid->pos_dx = tdx;
              captured_droid->pos_dy = tdy;
              captured_droid->state = 1;
              captured_droid = NULL;
              do_sound(FX_EXPLOSION_1,40);
              do_sound(FX_POWER_UP,40);
              transferring = 0;
              add_message("Transferred!!");
							sprintf(Shield_str, "Shields: %d", stats.shielding); // Added by Dion Bonner, 14-Dec-2001
							add_message(Shield_str);
              tdroid::display_nd();
            }
        }
  }
}

void tparadroid::capture_droid(int cx,int cy)
{
  register int x;

  for(x = 0;x < MAX_DROIDS;x++)
    if(*(droids + x) != NULL)
    {
      if((*(droids + x) != this) && !(*(droids + x))->state &&
        ((*(droids + x))->floor_ptr == floor_ptr))
      {
        float ix,iy;

        ix = cx - (*(droids + x))->pos_x + SPRITE_HSIZE_X;
        iy = cy - (*(droids + x))->pos_y + SPRITE_HSIZE_Y;
        if((ix >= 0) && (ix < SPRITE_SIZE_X) &&
           (iy >= 0) && (iy < SPRITE_SIZE_Y))
        {
          char str[STR_LABEL_LEN];

          captured_droid = (tparadroid *)*(droids + x);
          captured_droid->captured = 1;
          tug1 = tug2 = 0;
          if(within_capt_dist())
            sprintf(str,"%s captured",captured_droid->stats.type);
          else
          {
            sprintf(str,"Out of range");
            sound_engine_cmd(SND_CMD_FX,FX_NO_WEAPON,0xff,0x80);
          }
          add_message(str);
          return;
        }
      }
    }
    else
      break;
  add_message("Empty Space");
  sound_engine_cmd(SND_CMD_FX,FX_NO_WEAPON,0xff,0x80);
}

int tparadroid::action(tkevent *kp)
{
  register int s = 0;
  char str[STR_LABEL_LEN];
  char *no_weapon_mess = "No weapon!";

  if(state)
    return 0;
  switch(kp->type)
  {
    case ButtonPress:
      if(kp->val == 1)
      {
        if(transferring)
        {
          if(captured_droid == NULL)
            capture_droid(
              kp->mouse_x + ((int)pos_x - SCREEN_HSIZE_X),
              kp->mouse_y + ((int)pos_y - SCREEN_HSIZE_Y));
        }
        else
          if(stats.armament)
            create_laser(
              (kp->mouse_x - SCREEN_HSIZE_X),
              (kp->mouse_y - SCREEN_HSIZE_Y));
          else
          {
            add_message(no_weapon_mess);
            sound_engine_cmd(SND_CMD_FX,FX_NO_WEAPON,0xff,0x80);
          }
      }
      else
        if(!transferring)
          if(stats.armament)
          {
            create_laser(-10000,     0);
            create_laser( 10000,     0);
            create_laser(     0,-10000);
            create_laser(     0, 10000);
            create_laser(-10000,-10000);
            create_laser( 10000,-10000);
            create_laser(-10000, 10000);
            create_laser( 10000, 10000);
          }
          else
            add_message(no_weapon_mess);
      break;
    case KeyPress:
      flr_move = 1;
      switch(kp->val)
      {
        case KEY_UP:
        case KEY_KP_UP:
          pos_dx = 0;
          pos_dy = -stats.speed;
          break;
        case KEY_DOWN:
        case KEY_KP_DOWN:
          pos_dx = 0;
          pos_dy = stats.speed;
          break;
        case KEY_LEFT:
        case KEY_KP_LEFT:
          pos_dx = -stats.speed;
          pos_dy = 0;
          break;
        case KEY_RIGHT:
        case KEY_KP_RIGHT:
          pos_dx = stats.speed;
          pos_dy = 0;
          break;
/* These next cases added to see if the diagonals will work reliably */
        case KEY_END:
        case KEY_KP_END:
          pos_dx = -stats.speed;
          pos_dy = stats.speed;
					break;
        case KEY_HOME:
        case KEY_KP_HOME:
          pos_dx = -stats.speed;
          pos_dy = -stats.speed;
          break;
        case KEY_NPAGE:
        case KEY_KP_NPAGE:
          pos_dx = stats.speed;
          pos_dy = stats.speed;
          break;
        case KEY_PPAGE:
        case KEY_KP_PPAGE:
          pos_dx = stats.speed;
          pos_dy = -stats.speed;
          break;
        case KEY_STATUS:
          display_nd();
          sound_engine_cmd(SND_CMD_FX,FX_SELECT,0xff,0x80);
          break;
        case KEY_TRANSFER_MODE:
        case KEY_KP_TRANSFER_MODE:
          if(captured_droid == NULL)
          {
            transferring ^= 1;
            if(transferring)
            {
              add_message("Transfer on");
              sound_engine_cmd(SND_CMD_FX,FX_SELECT,0xff,0x80);
            }
            else
            {
              add_message("Transfer off");
              sound_engine_cmd(SND_CMD_FX,FX_WEAPON_RELOAD,0xff,0x80);
            }
          }
          else
          {
            add_message("You're commited!");
            sound_engine_cmd(SND_CMD_FX,FX_NO_WEAPON,0xff,0x80);
          }
          break;
        case KEY_SELECT:
        case KEY_KP_SELECT:
/* End of added keycases */
          if(captured_droid != NULL)
          {
            add_message("Can't trans.");
            sound_engine_cmd(SND_CMD_FX,FX_NO_WEAPON,0xff,0x80);
          }
          else
          {
            register int x,rpx,rpy;

            rpx = spos_x + SPRITE_HSIZE_X;
            rpy = spos_y + SPRITE_HSIZE_Y;
            for(x = 0;x < MAX_TRANSPORTS;x++)
              if(floor_ptr->transport[x].x != -1)
              {
                register int ix,iy;

                ix = rpx - floor_ptr->transport[x].x;
                iy = rpy - floor_ptr->transport[x].y;
                if((ix >= 0) && (ix < SPRITE_SIZE_X) &&
                   (iy >= 0) && (iy < SPRITE_SIZE_Y))
                {
                  pos_dx = 0;
                  pos_dy = 0;
                  s = 1;
                  sound_engine_cmd(SND_CMD_FX,FX_SELECT,0xff,0x80);
                  break;
                }
              }
            if(floor_ptr->power_bay != NULL)
            {
              register int ix,iy;

              ix = rpx - floor_ptr->power_bay->pos_x;
              iy = rpy - floor_ptr->power_bay->pos_y;
              if((ix >= 0) && (ix < SPRITE_SIZE_X) &&
                 (iy >= 0) && (iy < SPRITE_SIZE_Y))
              {
                register int cf1,cf2;

                cf1 = stats.max_shielding - stats.shielding;
                cf2 = score / 10;
                if(cf1 > cf2)
                  cf1 = cf2;
                stats.shielding += cf1;
                score -= cf1 * 10;
                sprintf(str,"Boost=%d(%d+)",stats.shielding,cf1);
                add_message(str);
                if(cf1)
                  sound_engine_cmd(SND_CMD_FX,FX_POWER_UP,0xff,0x80);
                else
                  sound_engine_cmd(SND_CMD_FX,FX_NO_WEAPON,0xff,0x80);
                break;
              }
            }
            if(floor_ptr->console.x != -1)
            {
              register int ix,iy;

              ix = rpx - floor_ptr->console.x;
              iy = rpy - floor_ptr->console.y;
              if((ix >= 0) && (ix < SPRITE_SIZE_X) &&
                 (iy >= 0) && (iy < SPRITE_SIZE_Y))
              {
                pos_dx = 0;
                pos_dy = 0;
                s = 2;
                sound_engine_cmd(SND_CMD_FX,FX_SELECT,0xff,0x80);
                break;
              }
            }
          }
          break;
      }
      break;
    case KeyRelease:
      flr_move = 0;
      break;
  }
  return s;
}

void tparadroid::display_nd(void)
{
  char str[STR_LABEL_LEN];

  sprintf(str,"Shield=%d",stats.shielding);
  add_message(str);
  tdroid::display_nd();
}

void tparadroid::add_message(char *str)
{
  register int x;

  if(*(message + 0) != NULL)
    free(*(message + 0));
  for(x = 0;x < (MAX_MESSAGES - 1);x++)
    *(message + x) = *(message + x + 1);
  *(message + (MAX_MESSAGES - 1)) = (char *)malloc(strlen(str) + 1);
  if(*(message + (MAX_MESSAGES - 1)) != NULL)
    strcpy(*(message + (MAX_MESSAGES - 1)),str);
}

/***************************************************************************
*
***************************************************************************/
tdroid_108::tdroid_108(void) 
{
  memcpy(&stats,&droid_stats[DROID_108],sizeof(tdroid_stats));
  count = 0;
  droid_cmd_list = NULL;
  droid_cmd_list_ptr = NULL;
}

tdroid_108::~tdroid_108(void) 
{
  free_all_cmd(&droid_cmd_list);
}

int tdroid_108::append_cmd(tdroid_cmd_list **ptr,int cmd,int d1,int d2)
{
  register int r = 0;

  if(*ptr == NULL)
  {
    *ptr = (tdroid_cmd_list *)malloc(sizeof(tdroid_cmd_list));
    if(*ptr != NULL)
    {
      (*ptr)->cmd = cmd;
      (*ptr)->d1 = d1;
      (*ptr)->d2 = d2;
      (*ptr)->next = NULL;
      r = 1;
      droid_cmd_list_ptr = droid_cmd_list;
    }
  }
  else
    r = append_cmd(&((*ptr)->next),cmd,d1,d2);
  return r;
}

void tdroid_108::free_all_cmd(tdroid_cmd_list **ptr)
{
  if(*ptr != NULL)
  {
    free_all_cmd(&((*ptr)->next));
    free(*ptr);
    *ptr = NULL;
  }
}

void tdroid_108::set_dir_vect(float vx,float vy)
{
  float d,s1,s2;

  s1 = fabs(vx);
  s2 = fabs(vy);
  if(s1 < s2)
    s1 = s2;
  if(s1 > stats.speed)
    s1 = stats.speed;
  else
    s1 /= 2;
  if(vy != 0)
    d = atan(vx / vy);
  else
    d = atan(vx);
  pos_dx = sin(d) * s1;
  pos_dy = cos(d) * s1;
  if(vy < 0)
  {
    pos_dx *= -1;
    pos_dy *= -1;
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

void tdroid_108::bg_calc(void)
{
  if(state == 0)
  {
    register int ix,iy;
#define MASK ~0x3

    switch(droid_cmd_list_ptr->cmd)
    {
      case 'g':
        ix = (int)rint(pos_x) & MASK;
        iy = (int)rint(pos_y) & MASK;
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
  memcpy(&stats,&droid_stats[DROID_176],sizeof(tdroid_stats));
}

/***************************************************************************
*
***************************************************************************/
tdroid_261::tdroid_261(void) 
{
  memcpy(&stats,&droid_stats[DROID_261],sizeof(tdroid_stats));
}

tdroid_275::tdroid_275(void) 
{
  memcpy(&stats,&droid_stats[DROID_275],sizeof(tdroid_stats));
}

/***************************************************************************
*
***************************************************************************/
tdroid_355::tdroid_355(void) 
{
  memcpy(&stats,&droid_stats[DROID_355],sizeof(tdroid_stats));
}

tdroid_368::tdroid_368(void) 
{
  memcpy(&stats,&droid_stats[DROID_368],sizeof(tdroid_stats));
}

/***************************************************************************
*
***************************************************************************/
tdroid_423::tdroid_423(void) 
{
  memcpy(&stats,&droid_stats[DROID_423],sizeof(tdroid_stats));
}

tdroid_467::tdroid_467(void) 
{
  memcpy(&stats,&droid_stats[DROID_467],sizeof(tdroid_stats));
}

tdroid_489::tdroid_489(void) 
{
  memcpy(&stats,&droid_stats[DROID_489],sizeof(tdroid_stats));
}

/***************************************************************************
*
***************************************************************************/
tdroid_513::tdroid_513(void) 
{
  memcpy(&stats,&droid_stats[DROID_513],sizeof(tdroid_stats));
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
#ifndef GOD_MODE
  attack();
#endif
}

/***************************************************************************
*
***************************************************************************/
tdroid_520::tdroid_520(void)
{
  memcpy(&stats,&droid_stats[DROID_520],sizeof(tdroid_stats));
}

tdroid_599::tdroid_599(void)
{
  memcpy(&stats,&droid_stats[DROID_599],sizeof(tdroid_stats));
}

/***************************************************************************
*
***************************************************************************/
tdroid_606::tdroid_606(void) 
{
  memcpy(&stats,&droid_stats[DROID_606],sizeof(tdroid_stats));
  attack_wait = 0;
  voice_c = 0;
  voice_type = FX_6XX_VOICE;
}


int tdroid_606::target_not_hidden(tentity *d)
{
  float tmp,dx,dy,tx = pos_x,ty = pos_y;
  register int a,i;

  dx = d->pos_x - pos_x;
  dy = d->pos_y - pos_y;
  i = (int)rint(fabs(dx));
  tmp = fabs(dy);
  if(i < tmp)
    i = (int)rint(tmp);
  if(i == 0)
    return 0;
  i <<= 3;
  dx /= i;
  dy /= i;
  for(a = 0;a < i;a++)
  {  
    tsprite *spr;
    register int sx,sy;
   
    tx += dx;
    ty += dy;
    sx = (int)(rint(tx) / SPRITE_SIZE_X);
    sy = (int)(rint(ty) / SPRITE_SIZE_Y);
    if((sx >= 0) && (sx < floor_ptr->fmap_x_size) &&
       (sy >= 0) && (sy < floor_ptr->fmap_y_size))
    {
      spr = flr_sprites + *(floor_ptr->fmap +
        (sy * floor_ptr->fmap_x_size) + sx);
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
      if((floor_ptr == droids[0]->floor_ptr) && (!droids[0]->state))
        if(!attack_wait)
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
          attack_wait--;
  }
  if(voice_c)
  {
    if(voice_c == 1)
      do_sound(voice_type,256);
    voice_c--;
  }
}

/***************************************************************************
*
***************************************************************************/
tdroid_691::tdroid_691(void)
{
  memcpy(&stats,&droid_stats[DROID_691],sizeof(tdroid_stats));
}

tdroid_693::tdroid_693(void)
{
  memcpy(&stats,&droid_stats[DROID_693],sizeof(tdroid_stats));
}

/***************************************************************************
*
***************************************************************************/
tdroid_719::tdroid_719(void)
{
  memcpy(&stats,&droid_stats[DROID_719],sizeof(tdroid_stats));
  direction = 0;
  do_nothing_count = 0;
  voice_type = FX_7XX_VOICE;
}

void tdroid_719::move(void)
{
  float cpx,cpy;
  tsprite *spr;

  friction();
  cpx = pos_x;
  cpy = pos_y;
  switch(direction)
  {
    case 1:
      cpx += SPRITE_HSIZE_X;
      break;
    case 2:
      cpx -= SPRITE_HSIZE_X;
      break;
    case 3:
      cpy += SPRITE_HSIZE_Y;
      break;
    case 4:
      cpy -= SPRITE_HSIZE_Y;
      break;
  }
  spr =
    flr_sprites + *(floor_ptr->fmap +
    ((int)(rint(cpy) / SPRITE_SIZE_Y) * floor_ptr->fmap_x_size) +
     (int)(rint(cpx) / SPRITE_SIZE_X));
  if(spr->bound == 'y')
    direction = random() & 0x7;
  else
    switch(direction)
    {
      case 1:
        pos_dx = stats.speed;
        pos_dy = 0;
        break;
      case 2:
        pos_dx = -stats.speed;
        pos_dy = 0;
        break;
      case 3:
        pos_dx = 0;
        pos_dy = stats.speed;
        break;
      case 4:
        pos_dx = 0;
        pos_dy = -stats.speed;
        break;
      default:
        pos_dx = 0;
        pos_dy = 0;
        break;
    }
  if((pos_dx == 0) && (pos_dy == 0))
    if(!do_nothing_count)
    {
      do_nothing_count = 50;
      direction = random() & 0x7;
    }
    else
      do_nothing_count--;
}

void tdroid_719::bg_calc(void)
{
  move();
  tdroid::bg_calc();
#ifndef GOD_MODE
  attack();
#endif
}

/***************************************************************************
*
***************************************************************************/
tdroid_720::tdroid_720(void)
{
  memcpy(&stats,&droid_stats[DROID_720],sizeof(tdroid_stats));
}

tdroid_766::tdroid_766(void)
{
  memcpy(&stats,&droid_stats[DROID_766],sizeof(tdroid_stats));
}

tdroid_799::tdroid_799(void)
{
  memcpy(&stats,&droid_stats[DROID_799],sizeof(tdroid_stats));
}

/***************************************************************************
*
***************************************************************************/
tdroid_805::tdroid_805(void) 
{
  memcpy(&stats,&droid_stats[DROID_805],sizeof(tdroid_stats));
  evase = 0;
  voice_c = 80;
}

void tdroid_805::attack(void)
{
  if((state == 0) && (attack_droid != NULL))
  {
    if(attack_droid->state == 0)
    {
      if(!attack_wait)
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
          do_sound(FX_DROID_EVASE,40);
        }
      }
      else
      {
        attack_wait--;
        if(evase)
          if(!voice_c)
          {
            voice_c = 80;
            do_sound(FX_8XX_VOICE,40);
          }
          else
            voice_c--;
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
  memcpy(&stats,&droid_stats[DROID_810],sizeof(tdroid_stats));
}

tdroid_820::tdroid_820(void)
{
  memcpy(&stats,&droid_stats[DROID_820],sizeof(tdroid_stats));
}

tdroid_899::tdroid_899(void)
{
  memcpy(&stats,&droid_stats[DROID_899],sizeof(tdroid_stats));
}

/***************************************************************************
*
***************************************************************************/
tdroid_933::tdroid_933(void)
{
  memcpy(&stats,&droid_stats[DROID_933],sizeof(tdroid_stats));
}

tdroid_949::tdroid_949(void)
{
  memcpy(&stats,&droid_stats[DROID_949],sizeof(tdroid_stats));
}

tdroid_999::tdroid_999(void)
{
  memcpy(&stats,&droid_stats[DROID_999],sizeof(tdroid_stats));
}
