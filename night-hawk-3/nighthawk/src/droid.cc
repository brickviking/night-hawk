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
* daddy droid object routines
*
****************************************************************************
****************************************************************************
***************************************************************************/
#include "defs.h"
#include "protos.p"

#include "tdroid.h"
#include "tparadroid.h"

/***************************************************************************
*
***************************************************************************/
tdroid::tdroid(void)
{
  int x;

  for(x = 0; x < MAX_LASERS; x++)
    laser_list[x] = NULL;

  expl_anim_ptr = 0.0;
  expl_anim_speed = 0.3;
  expl_anim_len = 7.0;
  shield_wave_inc = 0.0;
  captured = 0;
  recharge_delay = 0.0;
}

void tdroid::init(tfloor *f, tentity **dp, float px, float py)
{
  tentity::init(f, &droid_bm[0], dp, 10, px, py);
}

int tdroid::create_laser(int vx, int vy)
{
  int x, status = 0;

  for(x = 0; x < MAX_LASERS; x++)
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

      if(laser_list[x] == NULL)
        malloc_error_h();

      laser_list[x]->init(
        floor_ptr, droids, pos_x, pos_y, vx, vy, this);

      break;
    }

  return status;
}

void tdroid::free_all_laser(void)
{
  int x;
     
  for(x = 0; x < MAX_LASERS; x++)
    if(laser_list[x] != NULL)
    {
      delete(laser_list[x]);
      laser_list[x] = NULL;
    }
}

void tdroid::snap_shot(void)
{
  int x;

  tentity::snap_shot();

  ss_expl_anim_ptr = expl_anim_ptr;

  for(x = 0; x < MAX_LASERS; x++)
    if(laser_list[x] != NULL)
      laser_list[x]->snap_shot();
}

void tdroid::draw_laser(tfloor *c_f_ptr, int px, int py)
{
  int x;

  for(x = 0; x < MAX_LASERS; x++)
    if(laser_list[x] != NULL)
      laser_list[x]->draw(c_f_ptr, px, py);
}

void tdroid::bg_calc_laser(void)
{
  int x;

  for(x = 0; x < MAX_LASERS; x++)
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


void tdroid::draw_droid_ls(int ix, int iy)
{
  int        i;
  static int pos[4][2] =
    {{-2, -19}, {-2, 16}, {-17, -2}, {14, -2}};

  for(i = 0; i < 4; i++)
    blit_rect(ix + pos[i][X], iy + pos[i][Y], 3, 3);
}

void tdroid::draw_droid(int ix, int iy)
{
  int x;

  if(stats.shielding <= (stats.max_shielding >> 3))
  {
    glColor4f(1.0, 0.0, 0.0, 0.5);
    draw_droid_ls(ix, iy);
    glColor4f(1.0, 0.0, 0.0, fabs(sin(shield_wave_inc)));
  }
  else
    if(stats.shielding <= (stats.max_shielding >> 2))
    {
      glColor4f(0.0, 1.0, 0.0, 0.5);
      draw_droid_ls(ix, iy);
      glColor4f(0.0, 1.0, 0.0, fabs(sin(shield_wave_inc)));
    }
    else
      glColor4fv(bm_colour);

  blit_bm(
    &bm[(int)ss_anim_ptr], ix - SPRITE_HSIZE_X, iy - SPRITE_HSIZE_Y);

  glColor4fv(bm_colour2);
  for(x = 0; x < 3; x++)
    blit_bm(
      &digit_bm[(int)(stats.type[x] - '0')], ix - 13 + (x << 3), iy - 4);
}

void tdroid::draw(tfloor *c_f_ptr, int px, int py)
{
  draw_laser(c_f_ptr, px, py);

  if(floor_ptr != c_f_ptr)
    return;

  switch(state)
  {
    case 0: /*running*/
      draw_droid(
        (spos_x - px) + SCREEN_HSIZE_X,
        (spos_y - py) + SCREEN_HSIZE_Y);
      break;

    case 1: /*dieing*/
      glColor4fv(bm_colour);
      blit_bm(
        &explosion_bm[(int)ss_expl_anim_ptr], 
        (spos_x - px) + (SCREEN_HSIZE_X - 20),
        (spos_y - py) + (SCREEN_HSIZE_Y - 20));
      break;
  }
}

void tdroid::recharge(void)
{
  int ix, iy;

  if(floor_ptr->power_bay == NULL)
    return;

  recharge_delay += get_time_lapse();
  if(recharge_delay <= 1.0)
    return;
  recharge_delay =- 1.0;

  if(stats.max_shielding == stats.shielding)
    return;

  ix = ((int)rint(pos_x) + SPRITE_HSIZE_X) - floor_ptr->power_bay->pos_x;
  iy = ((int)rint(pos_y) + SPRITE_HSIZE_Y) - floor_ptr->power_bay->pos_y;
  if((ix < 0) || (ix > SPRITE_SIZE_X) || (iy < 0) || (iy > SPRITE_SIZE_Y))
    return;

  if(this == droids[0])
  {
    if(score > 0)
    {
      score--;
      stats.shielding++;
      console_message_add("Boost!");
    }
  }
  else
    stats.shielding++;

//  if(cf1)
//    sound_engine_cmd(SND_CMD_FX,FX_POWER_UP,0xff,0x80);
}

void tdroid::floor_down_test(void)
{
  int a, c;

  for(a = 1, c = 0; a < MAX_DROIDS; a++)
  {
    if(droids[a] == NULL)
      continue;

    if((floor_ptr == droids[a]->floor_ptr) && !droids[a]->state)
      c++;
  }

  if(!c)
  {
    floor_ptr->down = 1;
    console_message_add("Floor clear");
  }
}

void tdroid::bg_calc(void)
{
  double spin;

  spin = (stats.shielding * 0.35) / stats.max_shielding;

  switch(state)
  {
    case 0: /*running*/
      anim_speed = spin;
      shield_wave_inc += 0.05 * get_time_lapse();
      tentity::bg_calc();
      recharge();
      break;

    case 1: /*dieing*/
      anim_speed = spin;
      tentity::bg_calc();
      expl_anim_ptr += expl_anim_speed * get_time_lapse();
      if(expl_anim_ptr >= expl_anim_len)
      {
        state++;
        floor_down_test();
      }
      break;
  }

  bg_calc_laser();
}

void tdroid::friction(void)
{
  float x;

  if(pos_dx > 0.0)
  {
    x = pos_dx;
    if(x > 0.15)
      x = 0.15;
    pos_dx -= x;
  }

  if(pos_dx < 0.0)
  {
    x = pos_dx;
    if(x < -0.15)
      x = -0.15;
    pos_dx -= x;
  }

  if(pos_dy > 0.0)
  {
    x = pos_dy;
    if(x > 0.15)
      x = 0.15;
    pos_dy -= x;
  }

  if(pos_dy < 0.0)
  {
    x = pos_dy;
    if(x < -0.15)
      x = -0.15;
    pos_dy -= x;
  }
}

void tdroid::display_nd(void)
{
  char str[STR_LABEL_LEN + 1];
  int  a, nd = 0;

  for(a = 1; a < MAX_DROIDS; a++)
    if(*(droids + a) != NULL)
      if(!(*(droids + a))->state)
        nd++;

  sprintf(str, "droids=%d", nd);
  console_message_add(str);
}

void tdroid::hit(tlaser *l)
{
  char str[STR_LABEL_LEN + 1];

  pos_dx = l->pos_dx / 4.0;
  pos_dy = l->pos_dy / 4.0;

  stats.shielding -= l->damage_factor;
  if(stats.shielding < 0)
  {
    state = 1;

    sprintf(str, "%s d %s", l->owner->stats.type, stats.type);
    console_message_add(str);

    if(this != droids[0])
      if(!god_mode)
        score += stats.entry * 25;

    display_nd();

//    if(random() & 1)
//      do_sound(FX_EXPLOSION_1,40);
//    else
//      do_sound(FX_EXPLOSION_2,40);
  }
  else
  {
    attack_droid = l->owner;

    sprintf(
      str, "%s h %s(%d)", l->owner->stats.type, stats.type, stats.shielding);
    console_message_add(str);

    if(this != droids[0])
      if(!god_mode)
        score += stats.entry;

//    do_sound(FX_DROID_HIT_1,50);
  }
}
