/***************************************************************************
****************************************************************************
****************************************************************************
*
* NightHawk - By Jason Nunn - Oct 96
* FREEWARE.
*
* Snail: 32 Rothdale Road, Moil, Darwin, NT, 0810, Australia
*
* ==================================================================
* floor object
*
****************************************************************************
****************************************************************************
***************************************************************************/
extern "C" {
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <math.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>
}
#include "nighthawk_defs.h"
#include "misc.h"
#ifdef DEVELOPMENT_MODE
#include "tedit_floor.h"
#else
#include "tfloor.h"
#endif

/***************************************************************************
*
***************************************************************************/
tfloor::tfloor(void)
{
  register int x;

  fmap_x_size = fmap_y_size = 0;
  fmap = NULL;
  power_bay = NULL;
  for(x = 0;x < MAX_DOORS;x++)
    door[x] = NULL;
  for(x = 0;x < MAX_TRANSPORTS;x++)
    transport[x].x = -1;
  console.x = -1;
  ship_noise_no = -1;
  ship_noise_vol = 0;
  ship_noise_freq = 0;
}

int tfloor::create(int x_size,int y_size)
{
  fmap_x_size = x_size;
  fmap_y_size = y_size;
  if((fmap = (int *)malloc(fmap_x_size * fmap_y_size * sizeof(int))) == NULL)
  {
    perror("tfloor::create() ");
    return 0;
  }
  return 1;
}

int tfloor::load(char *fn,char *fname)
{
  FILE *fp;
  char str[STR_LEN];
  int sx,sy;

  map_filename = fn;
  strcpy(name,fname);
//load floor data
  strcpy(str,map_filename);
  strcat(str,".f");
  if((fp = fopen(str,"r")) == NULL)
  {
    perror("tfloor::load() ");
    return 0;
  }
  str[0] = 0;
  fgets(str,STR_LEN,fp);
  sscanf(str,"%d %d",&sx,&sy);
  if(create(sx,sy))
  {
    register int x,y;

    for(y = 0;y < fmap_y_size;y++)
      for(x = 0;x < fmap_x_size;x++)
      {
        str[0] = 0;
        fgets(str,STR_LEN,fp);
        sscanf(str,"%d",(fmap + (y * fmap_x_size) + x));
      }
  }
  fclose(fp);

//load misc data
  strcpy(str,map_filename);
  strcat(str,".m");
  if((fp = fopen(str,"r")) != NULL)
  {
    register int door_ptr = 0,transport_ptr = 0;

    while(!feof(fp))
    {
      str[0] = 0;   
      fgets(str,STR_LEN,fp);
      if(str[0])
      {
        char amble[STR_LABEL_LEN];

        sscanf(str,"%s",amble);
        if(!strcmp(amble,"door:"))
        {
          if(door_ptr < MAX_DOORS)
          {
            char t;

            sscanf(str,"%*s %d %d %c",&sx,&sy,&t);
            door[door_ptr] = new(tdoor);
            if(door[door_ptr] != NULL)
            {
              door[door_ptr]->init(sx,sy,t == 'h' ? 1 : 0);
              door_ptr++;
            }
          }
        }
        else if(!strcmp(amble,"power_bay:"))
        {
          if(power_bay == NULL)
          {
            sscanf(str,"%*s %d %d",&sx,&sy);
            power_bay = new(tpower_bay);
            if(power_bay != NULL)
              power_bay->init(sx,sy);
          }
        }
        else if(!strcmp(amble,"transport:"))
        {
          if(transport_ptr < MAX_TRANSPORTS)
          {
            sscanf(str,"%*s %d %d",
              &transport[transport_ptr].x,
              &transport[transport_ptr].y);
            transport_ptr++;
          }
        }
        else if(!strcmp(amble,"console:"))
        {
          sscanf(str,"%*s %d %d",&console.x,&console.y);
        }
        else if(!strcmp(amble,"noise:"))
        {
          sscanf(str,"%*s %d %d %d",
            &ship_noise_no,&ship_noise_vol,&ship_noise_freq);
        }
      }
    }
    fclose(fp);
  }
  return 1;
}

void tfloor::set_pos(int x,int y)
{
  spos_x = x;
  spos_y = y;
}

void tfloor::shut_all_doors(void)
{
  register int x;
          
  for(x = 0;x < MAX_DOORS;x++)
    if(door[x] != NULL)
    {
      door[x]->state = 0;
      door[x]->anim_ptr = 0;
    }
    else
      break;
}

void tfloor::draw(void)
{
  register int x,y,ipx,ipx_r,ipy,adj_x,adj_y;

  x = spos_x - SCREEN_HSIZE_X;
  y = spos_y - SCREEN_HSIZE_Y;
  ipx_r = ipx = (x / SPRITE_SIZE_X) - 1;
  ipy = (y / SPRITE_SIZE_Y) - 1;
  adj_x = (x % SPRITE_SIZE_X) + SPRITE_SIZE_X;
  adj_y = (y % SPRITE_SIZE_Y) + SPRITE_SIZE_Y;
  XSetClipMask(display,gc_bgblt,None);
  for(y = 0;y < ((SCREEN_SIZE_Y / SPRITE_SIZE_Y) + 3);y++)
  {
    for(x = 0;x < ((SCREEN_SIZE_X / SPRITE_SIZE_X) + 3);x++)
    {
      tbm *bm;

      if((ipx >= 0) && (ipx < fmap_x_size) &&
         (ipy >= 0) && (ipy < fmap_y_size))
      {
        bm = &(flr_sprites + *(fmap + (ipy * fmap_x_size) + ipx))->bm_ptr;
        XCopyArea(
          display,
          bm->pixmap,
          render_screen,
          gc_bgblt,
          0,0,bm->width,bm->height,
          (x * SPRITE_SIZE_X) - adj_x,(y * SPRITE_SIZE_Y) - adj_y);
      }
      else
        XFillRectangle(display,render_screen,gc_bgblt,
          (x * SPRITE_SIZE_X) - adj_x,(y * SPRITE_SIZE_Y) - adj_y,
          SPRITE_SIZE_X,SPRITE_SIZE_Y);
      ipx++;
    }
    ipx = ipx_r;
    ipy++;
  }
  if(power_bay != NULL)
    power_bay->draw(spos_x,spos_y);
  for(x = 0;x < MAX_DOORS;x++)
    if(door[x] != NULL)
      door[x]->draw(spos_x,spos_y);
    else
      break;
}

void tfloor::bg_calc(void)
{
  register int x;

  if(power_bay != NULL)
    power_bay->bg_calc();
  for(x = 0;x < MAX_DOORS;x++)
    if(door[x] != NULL)
      door[x]->bg_calc();
    else
      break;
}     

void tfloor::unload(void)
{
  if(fmap != NULL)
    free(fmap);
  register int x;

  if(power_bay != NULL)
    delete(power_bay);
  for(x = 0;x < MAX_DOORS;x++)
    if(door[x] != NULL)
      delete(door[x]);
}

/***************************************************************************
*
***************************************************************************/
#ifdef DEVELOPMENT_MODE

tedit_floor::tedit_floor(void)
{
  spos_x = SPRITE_HSIZE_X;
  spos_y = SPRITE_HSIZE_Y;
  blank = 0;
}

void tedit_floor::init_colours(void)
{
  ramp_colour(0x0,0x0,0x0,0x0,0x0,0x0,&black_pixel,1);
  ramp_colour(0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,&white_pixel,1);
}

int tedit_floor::create(char *fn,int x_size,int y_size)
{
  register int x,y;

  map_filename = fn;
  if(tfloor::create(x_size,y_size) == 0) return 0;
  for(y = 0;y < fmap_y_size;y++)
    for(x = 0;x < fmap_x_size;x++)
      *(fmap + (y * fmap_x_size) + x) = default_map_value;
  sprite_sel_ptr = 0;
  return 1;
}

int tedit_floor::save(void)
{
  FILE *fp;
  register int x,y;
  char str[STR_LEN];

  printf("Saving floor\n");
  strcpy(str,map_filename);
  strcat(str,".f");
  if((fp = fopen(str,"w")) == NULL)
  {
    perror("tedit_floor::save() ");
    return 0;
  }
  fprintf(fp,"%d %d\n",fmap_x_size,fmap_y_size);
  for(y = 0;y < fmap_y_size;y++)
  {
    for(x = 0;x < fmap_x_size;x++)
      fprintf(fp,"%d\n",*(fmap + (y * fmap_x_size) + x));
  }
  fclose(fp);
  return 1;
}

void tedit_floor::draw(void)
{
  register int ipx,ipy,ssp_block;

  tfloor::draw();
  if(blank)
    return;
  ipx = spos_x / SPRITE_SIZE_X;
  ipy = spos_y / SPRITE_SIZE_Y;
  if((ipx >= 0) && (ipx < fmap_x_size) &&
     (ipy >= 0) && (ipy < fmap_y_size))
  {
    register int x1,y1,x2,y2;
    char str[128];

    x1 = SCREEN_HSIZE_X - (spos_x % SPRITE_SIZE_X);
    y1 = SCREEN_HSIZE_Y - (spos_y % SPRITE_SIZE_Y);
    x2 = x1 + SPRITE_SIZE_X;
    y2 = y1 + SPRITE_SIZE_Y;
    XSetForeground(display,gc_dline,white_pixel);
    XDrawLine(display,render_screen,gc_dline,
      x1,y1,x2,y1);
    XDrawLine(display,render_screen,gc_dline,
      x2,y1,x2,y2);
    XDrawLine(display,render_screen,gc_dline,
      x1,y2,x2,y2);
    XDrawLine(display,render_screen,gc_dline,
      x1,y1,x1,y2);
    sprintf(str,"%d,%d",spos_x,spos_y);
    XDrawString(display,render_screen,gc_dline,10,10,str,strlen(str));
  }
  XSetForeground(display,gc_dline,black_pixel);
  XFillRectangle(display,render_screen,gc_dline,
    SCREEN_SIZE_X - 40,0,40,((SPRITE_SIZE_Y + 10) * 4) + 10);
  ssp_block = sprite_sel_ptr & ~0x3;
  for(ipy = 0;ipy < 4;ipy++)
    if((ipy + ssp_block) < sprites_size)
    {
      tbm *bm = &(flr_sprites + ipy + ssp_block)->bm_ptr;
      XCopyArea(
        display,
        bm->pixmap,
        render_screen,
        gc_bgblt,
        0,0,bm->width,bm->height,
        SCREEN_SIZE_X - 34,(ipy * (SPRITE_SIZE_Y + 10)) + 10);
      if((sprite_sel_ptr & 0x3) == ipy)
        XSetForeground(display,gc_dline,white_pixel);
      else
        XSetForeground(display,gc_dline,black_pixel);
      XDrawLine(display,render_screen,gc_dline,
        SCREEN_SIZE_X - 38,(ipy * (SPRITE_SIZE_Y + 10)) + 10,
        SCREEN_SIZE_X - 38,(ipy * (SPRITE_SIZE_Y + 10)) + 10 + SPRITE_SIZE_Y);
    }
}

void tedit_floor::action(XEvent *event)
{
  register int ipx,ipy;

  switch(event->type)
  {
    case KeyPress:
      switch(XLookupKeysym((XKeyEvent *)event,0))
      {
        case KEY_UP:
          spos_y -= SPRITE_SIZE_Y;
          break;
        case KEY_DOWN:
          spos_y += SPRITE_SIZE_Y;
          break;
        case KEY_LEFT:
          spos_x -= SPRITE_SIZE_X;
          break;
        case KEY_RIGHT:
          spos_x += SPRITE_SIZE_X;
          break;
        case XK_Page_Up:
          if(sprite_sel_ptr > 0) sprite_sel_ptr--;
          break;
        case XK_Page_Down:
          if(sprite_sel_ptr < (sprites_size - 1)) sprite_sel_ptr++;
          break;
        case XK_Return:
          ipx = spos_x / SPRITE_SIZE_X;
          ipy = spos_y / SPRITE_SIZE_Y;
          if((ipx >= 0) && (ipx < fmap_x_size) &&
            (ipy >= 0) && (ipy < fmap_y_size))
            *(fmap + (ipy * fmap_x_size) + ipx) = sprite_sel_ptr;
          break;
        case KEY_STATUS:
          save();
          break;
        case 'b':
          blank ^= 1;
          break;
      }
      break;
  }
}

#endif
