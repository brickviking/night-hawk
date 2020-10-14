/***************************************************************************
****************************************************************************
****************************************************************************
* Nighthawk
* Copyright (C) 1996-2004 Jason Nunn  
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
*
* ==================================================================
* floor object
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
tfloor::tfloor(void)
{
  int x;

  fmap_x_size = fmap_y_size = 0;
  fmap = NULL;
  power_bay = NULL;

  for(x = 0; x < MAX_DOORS; x++)
    door[x] = NULL;

  for(x = 0; x < MAX_TRANSPORTS; x++)
    transport[x].x = -1;

  console.x = -1;
  colour = 1.0;
  down = 0;

//  ship_noise_no = -1;
//  ship_noise_vol = 0;
//  ship_noise_freq = 0;
}

void tfloor::create(int x_size, int y_size)
{
  fmap_x_size = x_size;
  fmap_y_size = y_size;
  fmap = (int *)alloc_mem(fmap_x_size * fmap_y_size * sizeof(int));
}

void tfloor::load(char *fn, char *fname)
{
  FILE *fp;
  char str[STR_LEN + 1];
  int  sx, sy, x, y, door_ptr = 0, transport_ptr = 0;

  map_filename = fn;
  strcpy(name, fname);

//load floor data
  strcpy(str, map_filename);
  strcat(str, ".f");
  fp = fopen(str, "r");
  if(fp == NULL)
  {
    fprintf(stderr, "Error: tfloor::load::fopen(map.f)\n");
    end_game(-1);
  }

  fgets(str, STR_LEN, fp);
  sscanf(str, "%d %d", &sx, &sy);
  create(sx, sy);
  for(y = 0; y < fmap_y_size; y++)
    for(x = 0; x < fmap_x_size; x++)
    {
      fgets(str, STR_LEN, fp);
      sscanf(str, "%d", (fmap + (y * fmap_x_size) + x));
    }
  fclose(fp);

//load misc data
  strcpy(str, map_filename);
  strcat(str, ".m");
  fp = fopen(str, "r");
  if(fp == NULL)
  {
    fprintf(stderr, "Error: tfloor::load::fopen(map.m)\n");
    end_game(-1);
  }

  while(!feof(fp))
  {
    char amble[STR_LABEL_LEN];

    fgets(str, STR_LEN, fp);
    if(!str[0])
      continue;

    sscanf(str, "%s", amble);
    if(!strcmp(amble, "door:"))
    {
      if(door_ptr < MAX_DOORS)
      {
        char t;

        sscanf(str, "%*s %d %d %c", &sx, &sy, &t);
        door[door_ptr] = new(tdoor);
        if(door[door_ptr] == NULL)
          malloc_error_h();
        door[door_ptr]->init(sx, sy, t == 'h' ? 1 : 0);
        door_ptr++;
      }
    }
    else if(!strcmp(amble, "power_bay:"))
    {
      if(power_bay == NULL)
      {
        sscanf(str, "%*s %d %d", &sx, &sy);
        power_bay = new(tpower_bay);
        if(power_bay == NULL)
          malloc_error_h();
        power_bay->init(sx, sy);
      }
    }
    else if(!strcmp(amble, "transport:"))
    {
      if(transport_ptr < MAX_TRANSPORTS)
      {
        sscanf(str, "%*s %d %d",
          &transport[transport_ptr].x,
          &transport[transport_ptr].y);
        transport_ptr++;
      }
    }
    else if(!strcmp(amble, "console:"))
    {
      sscanf(str, "%*s %d %d", &console.x, &console.y);
    }
//    else if(!strcmp(amble, "noise:"))
//    {
//      sscanf(str, "%*s %d %d %d",
//        &ship_noise_no, &ship_noise_vol, &ship_noise_freq);
//    }
  }

  fclose(fp);
}

void tfloor::set_pos(int x, int y)
{
  spos_x = x;
  spos_y = y;
}

void tfloor::reinit(void)
{
  int x;

  if(colour != 1.0)
    colour = 0.5;

  for(x = 0; x < MAX_DOORS; x++)
  {
    if(door[x] != NULL)
    {
      door[x]->state = 0;
      door[x]->anim_ptr = 0;
    }
    else
      break;
  }
}

void tfloor::draw(void)
{
  int x, y, ipx, ipx_r, ipy, adj_x, adj_y, i;

  x = spos_x - SCREEN_HSIZE_X;
  y = spos_y - SCREEN_HSIZE_Y;
  ipx_r = ipx = (x / SPRITE_SIZE_X) - 1;
  ipy = (y / SPRITE_SIZE_Y) - 1;
  adj_x = (x % SPRITE_SIZE_X) + SPRITE_SIZE_X;
  adj_y = (y % SPRITE_SIZE_Y) + SPRITE_SIZE_Y;

  glColor3f(colour, colour, colour);

  for(y = 0; y < ((SCREEN_SIZE_Y / SPRITE_SIZE_Y) + 3); y++)
  {
    for(x = 0; x < ((SCREEN_SIZE_X / SPRITE_SIZE_X) + 3); x++)
    {
      if((ipx >= 0) && (ipx < fmap_x_size) &&
         (ipy >= 0) && (ipy < fmap_y_size))
      {
        i = *(fmap + (ipy * fmap_x_size) + ipx);
        blit_bm(
          &flr_bm[i].bm,
          (x * SPRITE_SIZE_X) - adj_x, (y * SPRITE_SIZE_Y) - adj_y);
      }

      ipx++;
    }

    ipx = ipx_r;
    ipy++;
  }

  if(power_bay != NULL)
    power_bay->draw(spos_x, spos_y);

  for(x = 0; x < MAX_DOORS; x++)
    if(door[x] != NULL)
      door[x]->draw(spos_x, spos_y);
    else
      break;
}

void tfloor::bg_calc(void)
{
  int x;

  if(down)
  {
    colour -= 0.01 * get_time_lapse();
    if(colour < 0.5)
    {
      colour = 0.5;
      down = 0;
    }
  }

  if(power_bay != NULL)
    power_bay->bg_calc();

  for(x = 0; x < MAX_DOORS; x++)
    if(door[x] != NULL)
      door[x]->bg_calc();
    else
      break;
}

void tfloor::unload(void)
{
  int x;

  if(fmap != NULL)
    free(fmap);

  if(power_bay != NULL)
    delete(power_bay);

  for(x = 0; x < MAX_DOORS; x++)
    if(door[x] != NULL)
      delete(door[x]);
}
