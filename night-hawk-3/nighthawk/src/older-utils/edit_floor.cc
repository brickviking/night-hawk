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

#include "tedit_floor.h"

/***************************************************************************
*
***************************************************************************/
tedit_floor::tedit_floor(void)
{
  spos_x = SPRITE_HSIZE_X;
  spos_y = SPRITE_HSIZE_Y;
  blank = 0;
}

void tedit_floor::init_colours(void)
{
  ramp_colour(0x0, 0x0, 0x0, 0x0, 0x0, 0x0, &black_pixel, 1);
  ramp_colour(0xffff, 0xffff,0xffff,0xffff,0xffff,0xffff,&white_pixel,1);
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
