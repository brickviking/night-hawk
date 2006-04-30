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
#include "defs.h"
#include "protos.p"

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
*
***************************************************************************/
tdroid_108::tdroid_108(void) 
{
  memcpy(&stats, &droid_stats[DROID_108], sizeof(tdroid_stats));
  count = 0;
  droid_cmd_list = NULL;
  droid_cmd_list_ptr = NULL;
}

tdroid_108::~tdroid_108(void) 
{
  free_all_cmd(&droid_cmd_list);
}

int tdroid_108::append_cmd(tdroid_cmd_list **ptr, int cmd, int d1, int d2)
{
  int r = 0;

  if(*ptr == NULL)
  {
    *ptr = (tdroid_cmd_list *)alloc_mem(sizeof(tdroid_cmd_list));
    (*ptr)->cmd = cmd;
    (*ptr)->d1 = d1;
    (*ptr)->d2 = d2;
    (*ptr)->next = NULL;
    r = 1;
    droid_cmd_list_ptr = droid_cmd_list;
  }
  else
    r = append_cmd(&((*ptr)->next), cmd, d1, d2);

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

void tdroid_108::set_dir_vect(float vx, float vy)
{
  float d, s1, s2;

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
  int ix, iy;

  if(state == 0)
  {
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
  attack_wait = 0.0;
//  voice_c = 0;
//  voice_type = FX_6XX_VOICE;
}


int tdroid_606::target_not_hidden(tentity *d)
{
  float tmp, dx, dy, tx = pos_x, ty = pos_y;
  int   a, i;

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

  for(a = 0; a < i; a++)
  {  
    tflr_bm *spr;
    int     sx, sy;
   
    tx += dx;
    ty += dy;
    sx = (int)(rint(tx) / SPRITE_SIZE_X);
    sy = (int)(rint(ty) / SPRITE_SIZE_Y);

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
      if((floor_ptr == droids[0]->floor_ptr) && (!droids[0]->state))
        if(attack_wait <= 0.0)
        {
          attack_wait = stats.attack_rate;
          if(target_not_hidden(droids[0]))
          {
            create_laser(
              (int)(droids[0]->pos_x - pos_x),
              (int)(droids[0]->pos_y - pos_y));

//            if(!voice_c)
//              voice_c = 40;
          }
        }
        else
          attack_wait -= get_time_lapse();
  }

//  if(voice_c)
//  {
//    if(voice_c == 1)
//      do_sound(voice_type,256);
//    voice_c--;
//  }
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
*
***************************************************************************/
tdroid_719::tdroid_719(void)
{
  memcpy(&stats, &droid_stats[DROID_719], sizeof(tdroid_stats));
  direction = 0;
  do_nothing_count = 0;
//  voice_type = FX_7XX_VOICE;
}

void tdroid_719::move(void)
{
  float   cpx, cpy;
  tflr_bm *spr;

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
    flr_bm + *(floor_ptr->fmap +
    ((int)(rint(cpy) / SPRITE_SIZE_Y) * floor_ptr->fmap_x_size) +
     (int)(rint(cpx) / SPRITE_SIZE_X));

  if(spr->bound == 'y')
    direction = random() & 0x7;
  else
  {
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
  }

  if((pos_dx == 0) && (pos_dy == 0))
  {
    if(!do_nothing_count)
    {
      do_nothing_count = 50;
      direction = random() & 0x7;
    }
    else
      do_nothing_count--;
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
//  voice_c = 80;
}

void tdroid_805::attack(void)
{
  if((state == 0) && (attack_droid != NULL))
  {
    if(attack_droid->state == 0)
    {
      if(attack_wait <= 0.0)
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
//          do_sound(FX_DROID_EVASE,40);
        }
      }
      else
      {
        attack_wait -= get_time_lapse();

//        if(evase)
//          if(!voice_c)
//          {
//            voice_c = 80;
//            do_sound(FX_8XX_VOICE,40);
//          }
//          else
//            voice_c--;
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
}

tdroid_949::tdroid_949(void)
{
  memcpy(&stats, &droid_stats[DROID_949], sizeof(tdroid_stats));
}

tdroid_999::tdroid_999(void)
{
  memcpy(&stats, &droid_stats[DROID_999], sizeof(tdroid_stats));
}
