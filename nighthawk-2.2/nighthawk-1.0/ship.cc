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
* ship object
*
****************************************************************************
****************************************************************************
***************************************************************************/
extern "C" {
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <malloc.h>
#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/xpm.h>
#include <X11/keysym.h>
}
#include "defs.h"
#include "misc.h"
#include "tship.h"

/***************************************************************************
*
***************************************************************************/
tship::tship(void)
{
  register int x;

  floor_list = NULL;
  transport_list = NULL;
  curr_location = NULL;
  for(x = 0;x < MAX_DROIDS;x++)
    droids[x] = NULL;
  ship_complete_f = 0;
  ship_end_cd = 100;
}

int tship::create_floor(tfloor_list **ptr,char *fname)
{
  register int status = 0;

  if(*ptr == NULL)
  {
    *ptr = (tfloor_list *)malloc(sizeof(tfloor_list));
    if(*ptr != NULL)
    {
      (*ptr)->next = NULL;
      (*ptr)->curr = new tfloor();
      if((*ptr)->curr != NULL)
      {
        char str[STR_LEN];

        strcpy(str,INSTALL_LIB_DIR "/");
        strcat(str,name);
        strcat(str,"/");
        strcat(str,fname);
        if((*ptr)->curr->load(str,fname))
          status = 1;
      }
    }
  }
  else
    create_floor(&((*ptr)->next),fname);
  return status;
}

tfloor *tship::find_floor(char *name)
{
  tfloor_list *p = floor_list;   
  tfloor *s = NULL;
 
  while(p != NULL)
  {
    if(!strcmp(name,p->curr->name))
    {
      s = p->curr;
      break;
    }
    p = p->next;
  }
  return s;
}

ttransport_list *tship::create_transport(
  ttransport_list **ptr,char *fn,int px,int py,int fx,int fy)
{
  ttransport_list *r = NULL;

  if(*ptr == NULL)
  {
    *ptr = (ttransport_list *)malloc(sizeof(ttransport_list));
    if(*ptr != NULL)
    {
      (*ptr)->floor = find_floor(fn);
      (*ptr)->diagram_px = px;
      (*ptr)->diagram_py = py;
      (*ptr)->floor_pos_x = fx;
      (*ptr)->floor_pos_y = fy;
      (*ptr)->transport_list_link = NULL;
      (*ptr)->next = NULL;
      r = *ptr;
    }
  }
  else
    r = create_transport(&((*ptr)->next),fn,px,py,fx,fy);
  return r;
}

ttransport_list *tship::find_transport(char *name,int x,int y)
{
  ttransport_list *p = transport_list;   
  ttransport_list *s = NULL;
 
  while(p != NULL)
  {
    if(!strcmp(name,p->floor->name) &&
       (p->floor_pos_x == x) &&
       (p->floor_pos_y == y))
    {
      s = p;
      break;
    }
    p = p->next;
  }
  return s;
}

int tship::create_transport_link(
  ttransport_list_link **ptr,char *fn,int x,int y)
{
  register int status = 0;

  if(*ptr == NULL)
  {
    *ptr = (ttransport_list_link *)malloc(sizeof(ttransport_list_link));
    if(*ptr != NULL)
    {
      strcpy((*ptr)->floor,fn);
      (*ptr)->floor_pos_x = x;
      (*ptr)->floor_pos_y = y;
      (*ptr)->next = NULL;
    }
  }
  else
    create_transport_link(&((*ptr)->next),fn,x,y);
  return status;
}

void tship::free_all_floor(tfloor_list **ptr)
{
  if(*ptr != NULL)
  {
    free_all_floor(&((*ptr)->next));
    (*ptr)->curr->unload();
    delete((*ptr)->curr);
    free(*ptr);
    *ptr = NULL;
  }
}

void tship::free_all_transport_links(ttransport_list_link **ptr)
{
  if(*ptr != NULL)
  {
    free_all_transport_links(&((*ptr)->next));
    free(*ptr);
    *ptr = NULL;
  }
}

void tship::free_all_transport(ttransport_list **ptr)
{
  if(*ptr != NULL)
  {
    free_all_transport(&((*ptr)->next));
    free_all_transport_links(&((*ptr)->transport_list_link));
    free(*ptr);
    *ptr = NULL;
  }
}

int tship::load(char *shipname)
{
  FILE *fp;
  char str[STR_LEN];

  strcpy(name,shipname);
  strcpy(str,INSTALL_LIB_DIR "/");
  strcat(str,name);
  strcat(str,"/chars");
  if((fp = fopen(str,"r")) != NULL)
  {
    register int droid_ptr = 1;

    while(!feof(fp))
    {
      str[0] = 0;   
      fgets(str,STR_LEN,fp);
      if(str[0])
      {
        char amble[STR_LABEL_LEN];
        char floor_n[STR_LABEL_LEN];

        sscanf(str,"%s",amble);
        if(!strcmp(amble,"type:"))
        {
          strncpy(type,str + 6,STR_LABEL_LEN);
          type[strlen(type) - 1] = 0;
        }
        else if(!strcmp(amble,"floor:"))
        {
          sscanf(str,"%*s %s",floor_n);
          create_floor(&floor_list,floor_n);
        }
        else if(!strcmp(amble,"transport:"))
        {
          int px,py,fx,fy;
          ttransport_list *p;

          sscanf(str,"%*s %s %d %d %d %d",
            floor_n,&px,&py,&fx,&fy);
          p = create_transport(
            &transport_list,floor_n,px,py,fx,fy);
          if(p != NULL)
            while(!feof(fp))
            {
              long lpos;

              lpos = ftell(fp);
              str[0] = 0;
              fgets(str,STR_LEN,fp);
              if(str[0] == '*')
              {
                sscanf(str,"%*c %s %d %d",amble,&fx,&fy);
                create_transport_link(
                  &(p)->transport_list_link,amble,fx,fy);
              }
              else
              {
                fseek(fp,lpos,SEEK_SET);
                break;
              }
            }
        }
        else if(!strcmp(amble,"init:"))
        {
          int x,y;

          sscanf(str,"%*s %s %d %d",floor_n,&x,&y);
          curr_location = find_transport(floor_n,x,y);
          if(curr_location != NULL)
          {
            droids[0] = new(tparadroid);
            if(droids[0] != NULL)
              ((tparadroid *)droids[0])->init(
              curr_location->floor,(tentity **)&droids,x,y);
          }
        }
        else if(!strcmp(amble,"droid:"))
        {
          if(droid_ptr < MAX_DROIDS)
          {
            int x,y;
            tfloor *f;

            sscanf(str,"%*s %s %s %d %d",amble,floor_n,&x,&y);
            f = find_floor(floor_n);
            if(f != NULL)
            {
              if(!strcmp(amble,"108"))
                droids[droid_ptr] = new(tdroid_108);
              else if(!strcmp(amble,"176"))
                droids[droid_ptr] = new(tdroid_176);
              else if(!strcmp(amble,"261"))
                droids[droid_ptr] = new(tdroid_261);
              else if(!strcmp(amble,"275"))
                droids[droid_ptr] = new(tdroid_275);
              else if(!strcmp(amble,"355"))
                droids[droid_ptr] = new(tdroid_355);
              else if(!strcmp(amble,"368"))
                droids[droid_ptr] = new(tdroid_368);
              else if(!strcmp(amble,"423"))
                droids[droid_ptr] = new(tdroid_423);
              else if(!strcmp(amble,"467"))
                droids[droid_ptr] = new(tdroid_467);
              else if(!strcmp(amble,"489"))
                droids[droid_ptr] = new(tdroid_489);
              else if(!strcmp(amble,"513"))
                droids[droid_ptr] = new(tdroid_513);
              else if(!strcmp(amble,"520"))
                droids[droid_ptr] = new(tdroid_520);
              else if(!strcmp(amble,"599"))
                droids[droid_ptr] = new(tdroid_599);
              else if(!strcmp(amble,"606"))
                droids[droid_ptr] = new(tdroid_606);
              else if(!strcmp(amble,"691"))
                droids[droid_ptr] = new(tdroid_691);
              else if(!strcmp(amble,"693"))
                droids[droid_ptr] = new(tdroid_693);
              else if(!strcmp(amble,"719"))
                droids[droid_ptr] = new(tdroid_719);
              else if(!strcmp(amble,"720"))
                droids[droid_ptr] = new(tdroid_720);
              else if(!strcmp(amble,"766"))
                droids[droid_ptr] = new(tdroid_766);
              else if(!strcmp(amble,"799"))
                droids[droid_ptr] = new(tdroid_799);
              else if(!strcmp(amble,"805"))
                droids[droid_ptr] = new(tdroid_805);
              else if(!strcmp(amble,"810"))
                droids[droid_ptr] = new(tdroid_810);
              else if(!strcmp(amble,"820"))
                droids[droid_ptr] = new(tdroid_820);
              else if(!strcmp(amble,"899"))
                droids[droid_ptr] = new(tdroid_899);
              else if(!strcmp(amble,"933"))
                droids[droid_ptr] = new(tdroid_933);
              else if(!strcmp(amble,"949"))
                droids[droid_ptr] = new(tdroid_949);
              else if(!strcmp(amble,"999"))
                droids[droid_ptr] = new(tdroid_999);
              if(droids[droid_ptr] != NULL)
              {
                droids[droid_ptr]->init(f,(tentity **)&droids,x,y);
                while(!feof(fp))
                {
                  long lpos;

                  lpos = ftell(fp);
                  str[0] = 0;
                  fgets(str,STR_LEN,fp);
                  if(str[0] == '*')
                  {
                    char c;

                    sscanf(str,"%*c %s %d %d",&c,&x,&y);
                    ((tdroid_108 *)droids[droid_ptr])->append_cmd(
                      &(((tdroid_108 *)droids[droid_ptr])->droid_cmd_list),
                      c,x,y);
                  }
                  else
                  {
                    fseek(fp,lpos,SEEK_SET);
                    break;
                  }
                }
                droid_ptr++;
              }
            }
          }
        }
      }
    }
    fclose(fp);
    strcpy(str,INSTALL_LIB_DIR "/");
    strcat(str,name);
    strcat(str,"/map.xpm");
    if(!loadxpm(str,&map_bm))
    {
      map_bm.width = -1;
      return 0;
    }
    sprintf(str,"SS %s",shipname);
    ((tparadroid *)droids[0])->add_message(str);
    droids[0]->display_nd();
    return 1;
  }
  return 0;
}

void tship::unload(void)
{
  register int x;

  free_all_floor(&floor_list);
  free_all_transport(&transport_list);
  if(map_bm.width != -1)
    free_bm(&map_bm);
  for(x = 0;x < MAX_DROIDS;x++)
    if(droids[x] != NULL)
      delete(droids[x]);
}

#define LEVEL_DRAW_SIZE_X 260
#define LEVEL_DRAW_SIZE_Y 150

void tship::diagram_draw(void)
{
  register int ox,oy,sprtx,sprty;
  char str[STR_LEN];

  XFillRectangle(display,render_screen,gc_bgblt,
    0,0,SCREEN_SIZE_X,SCREEN_SIZE_Y);
  drawxpm_ani(&ntitle_bm,0,0,0,1);
  print_score();
  ox = SCREEN_HSIZE_X - (LEVEL_DRAW_SIZE_X >> 1);
  oy = SCREEN_HSIZE_Y - (LEVEL_DRAW_SIZE_Y >> 1) + 10;
  XSetForeground(display,gc_line,blue_pixel[1]);
  XFillRectangle(display,render_screen,gc_line,
    ox,oy,LEVEL_DRAW_SIZE_X,LEVEL_DRAW_SIZE_Y);
  XSetForeground(display,gc_line,blue_pixel[2]);
  XDrawRectangle(display,render_screen,gc_line,
    ox,oy,LEVEL_DRAW_SIZE_X,LEVEL_DRAW_SIZE_Y);
  XSetForeground(display,gc_dline,blue_pixel[7]);
  sprintf(str,"StarShip %s (%s)",name,type);
  XDrawString(display,render_screen,gc_dline,
    ox + 10,oy + 20,str,strlen(str));
  sprtx = ox + (LEVEL_DRAW_SIZE_X >> 1) - (map_bm.width >> 1);
  sprty = oy + (LEVEL_DRAW_SIZE_Y >> 1) - (map_bm.height >> 1) + 10;
  drawxpm_ani(&map_bm,sprtx,sprty,0,1);
  XSetForeground(display,gc_line,yellow_pixel);
  XDrawRectangle(
    display,render_screen,gc_line,
    sprtx + curr_location->diagram_px,
    sprty + curr_location->diagram_py,1,1);
  XCopyArea(display,render_screen,window,gc_bgblt,0,0,
    window_width,window_height,0,0);
}

void tship::transport_set(void)
{
  ttransport_list *p = transport_list;

  while(p != NULL)
  {
    if(!strcmp(curr_location->floor->name,p->floor->name))
    {
      register int ix,iy;

      ix = p->floor_pos_x - droids[0]->spos_x + SPRITE_HSIZE_X;
      iy = p->floor_pos_y - droids[0]->spos_y + SPRITE_HSIZE_Y;
      if((ix >= 0) && (ix < SPRITE_SIZE_X) &&
         (iy >= 0) && (iy < SPRITE_SIZE_Y))
        curr_location = p;
    }
    p = p->next;
  }
}

void tship::transport_change(void)
{
  ttransport_list_link *p;

  p = curr_location->transport_list_link;
  while(p != NULL)
  {
    if(!strcmp(p->floor,curr_location->floor->name))
    {
      if(p->next != NULL)
      {
        ttransport_list *x;

        x = find_transport(
          p->next->floor,p->next->floor_pos_x,p->next->floor_pos_y);
        if(x != NULL)
        {
          curr_location = x;
          ((tparadroid *)droids[0])->init(
            curr_location->floor,(tentity **)&droids,
            curr_location->floor_pos_x,curr_location->floor_pos_y);
          return;
        }
      }
    }
    p = p->next;
  }
}

void tship::console_draw(void)
{
  register int ox,oy;
  char str[STR_LEN];
  char *header_mess = "Ship Computer- Droid Records";

  XFillRectangle(display,render_screen,gc_bgblt,
    0,0,SCREEN_SIZE_X,SCREEN_SIZE_Y);
  drawxpm_ani(&ntitle_bm,0,0,0,1);
  XSetForeground(display,gc_dline,white_pixel[7]);
  XDrawString(display,render_screen,gc_dline,
    90,24,header_mess,strlen(header_mess));
  ox = SCREEN_HSIZE_X - (LEVEL_DRAW_SIZE_X >> 1);
  oy = SCREEN_HSIZE_Y - (LEVEL_DRAW_SIZE_Y >> 1) + 10;
  XSetForeground(display,gc_line,green_pixel[7]);
  XFillRectangle(display,render_screen,gc_line,
    ox,oy,LEVEL_DRAW_SIZE_X,LEVEL_DRAW_SIZE_Y);
  XSetForeground(display,gc_line,green_pixel[6]);
  XDrawRectangle(display,render_screen,gc_line,
    ox,oy,LEVEL_DRAW_SIZE_X,LEVEL_DRAW_SIZE_Y);
  XSetForeground(display,gc_dline,green_pixel[0]);
  sprintf(str,"Current: %s (%s)",
    droids[0]->stats.type,droids[0]->stats.name);
  XDrawString(display,render_screen,gc_dline,
    ox + 10,oy + 20,str,strlen(str));

  XSetForeground(display,gc_dline,green_pixel[3]);
  sprintf(str,"  Type: %s",droid_stats[console_ptr].type);
  XDrawString(display,render_screen,gc_dline,
    ox + 30,oy + 38,str,strlen(str));

  sprintf(str,"  Name: %s",droid_stats[console_ptr].name);
  XDrawString(display,render_screen,gc_dline,
    ox + 30,oy + 38 + (12 * 1),str,strlen(str));

  sprintf(str," Entry: %d",droid_stats[console_ptr].entry);
  XDrawString(display,render_screen,gc_dline,
    ox + 30,oy + 38 + (12 * 2),str,strlen(str));

  sprintf(str,"Height: %f kg",droid_stats[console_ptr].height);
  XDrawString(display,render_screen,gc_dline,
    ox + 30,oy + 38 + (12 * 3),str,strlen(str));

  sprintf(str,"Weight: %f kg",droid_stats[console_ptr].weight);
  XDrawString(display,render_screen,gc_dline,
    ox + 30,oy + 38 + (12 * 4),str,strlen(str));

  sprintf(str," Brain: %s",droid_stats[console_ptr].brain);
  XDrawString(display,render_screen,gc_dline,
    ox + 30,oy + 38 + (12 * 5),str,strlen(str));

  switch(droid_stats[console_ptr].armament)
  {
    case 0:
      sprintf(str,"   Arm: None");
      break;
    case 1:
      sprintf(str,"   Arm: Linarite Laser");
      break;
    case 2:
      sprintf(str,"   Arm: Crocoite-Benzol Laser");
      break;
    case 3:
      sprintf(str,"   Arm: Uvarovite Laser");
      break;
    case 4:
      sprintf(str,"   Arm: Quartz 'Tiger-Eye' Laser");
      break;
  }
  XDrawString(display,render_screen,gc_dline,
    ox + 30,oy + 38 + (12 * 6),str,strlen(str));

  sprintf(str,"Shield: %d (Max: %d)",
    droid_stats[console_ptr].shielding,
    droid_stats[console_ptr].max_shielding);
  XDrawString(display,render_screen,gc_dline,
    ox + 30,oy + 38 + (12 * 7),str,strlen(str));

  sprintf(str," Speed: %d",droid_stats[console_ptr].speed);
  XDrawString(display,render_screen,gc_dline,
    ox + 30,oy + 38 + (12 * 8),str,strlen(str));
  switch(droid_stats[console_ptr].speed)
  {
    case 1:
      sprintf(str,"(Very slow)");
      break;
    case 2:
      sprintf(str,"(slow)");
      break;
    case 3:
      sprintf(str,"(fair)");
      break;
    case 4:
      sprintf(str,"(average)");
      break;
    case 5:
      sprintf(str,"(fast)");
      break;
    case 6:
      sprintf(str,"(Extremely fast)");
      break;
  }
  XDrawString(display,render_screen,gc_dline,
    ox + 90,oy + 38 + (12 * 8),str,strlen(str));

  if(droid_stats[console_ptr].attack_rate == -1)
    sprintf(str,"Attack: N/A");
  else
    sprintf(str,"Attack: %d",droid_stats[console_ptr].attack_rate);
  XDrawString(display,render_screen,gc_dline,
    ox + 30,oy + 38 + (12 * 9),str,strlen(str));

  XCopyArea(display,render_screen,window,gc_bgblt,0,0,
    window_width,window_height,0,0);
}

void tship::level_check_event(void)
{
  while(XPending(display))
  {
    XEvent event;

    XNextEvent(display,&event);
    if(state == 1)
    {
      register int s;

      s = ((tparadroid *)droids[0])->action(&event);
      if(s == 1)
      {
        state = 0;
        transport_set();
        break;
      }
      if(s == 2)
      {
        for(console_ptr = 0;console_ptr < DROID_999;console_ptr++)
          if(!strcmp(droids[0]->stats.type,droid_stats[console_ptr].type))
            break;
        state = 4;
        break;
      }
    }
    if(event.xany.type == KeyPress)
    {
      register int ch;

      ch = XLookupKeysym((XKeyEvent *)&event,0);
      if(ch == 'q')
        if(!droids[0]->state)
          droids[0]->state = 1;
      switch(state)
      {
        case 0:
          switch(ch)
          {
            case XK_Down:
              transport_change();
              break;
            case ' ':
              state = 1;
              break;
          }
          break;
        case 1:
          if(ch == 'p')
            state = 3;
          break;
        case 3:
          if(ch == 'p')
            state = 1;
          break;
        case 4:
          switch(ch)   
          {
            case ' ':
              state = 1;
              break;
            case XK_Left:
              if(console_ptr > 0)
                console_ptr--;
              break;
            case XK_Right:
              if(console_ptr < DROID_999)
                console_ptr++;
              break;
          }
          break;
      }
    }
  }
}

void tship::level_bg_calc(void)
{
  switch(state)
  {
    case 1:
      {
        register int x,nd = 0;

        if(curr_location->floor != NULL)
          curr_location->floor->bg_calc();
        for(x = 0;x < MAX_DROIDS;x++)
          if(droids[x] != NULL)
          {
            droids[x]->bg_calc();
            if(x)
              if((*(droids + x))->state < 2)
                nd++;
          }
        if(!nd)
          ship_complete_f = 1;
        if(ship_complete_f)
        {
          if(!ship_end_cd)
            state = 2;
          else
            ship_end_cd--;
        }
        if(droids[0]->state == 2)
          if(!ship_end_cd)
            state = 2;
          else
            ship_end_cd--;
      }
      break;
  }
}

void tship::print_score(void)
{
  register int x,val;
  int d_table[] = {
   10000000,
   1000000,
   100000,
   10000,
   1000,
   100,
   10,
   1,
  };

  XSetForeground(display,gc_dline,white_pixel[7]);
  val = sscore;
  for(x = 0;x < 8;x++)
  {
    register int i;

    i = val / d_table[x];
    val -= i * d_table[x];
    drawxpm_ani(&digit2_bm,(x * 11) + 95,11,i,10);
  }
}

void tship::print_info(void)
{
  register int x;
  char str[STR_LABEL_LEN];

  drawxpm_ani(&ntitle_bm,0,0,0,1);
  XSetForeground(display,gc_dline,white_pixel[5]);
  sprintf(str," Ship: %s",name);
  XDrawString(display,render_screen,gc_dline,
    SCREEN_SIZE_X - 120,12,str,strlen(str));
  sprintf(str,"Floor: %s",curr_location->floor->name);
  XDrawString(display,render_screen,gc_dline,
    SCREEN_SIZE_X - 120,24,str,strlen(str));
  for(x = 0;x < MAX_MESSAGES;x++)
    if(*((((tparadroid *)droids[0])->message + x)) != NULL)
    {
      XDrawString(display,render_screen,gc_dline,
        SCREEN_SIZE_X - 75,(SCREEN_SIZE_Y - 77) + (x * 12),
        *((((tparadroid *)droids[0])->message + x)),
        strlen(*((((tparadroid *)droids[0])->message + x))));
    }
  print_score();
  if(ship_complete_f)
    drawxpm_ani(&ship_complete_bm,
      SCREEN_HSIZE_X - (ship_complete_bm.width >> 1),
      50,0,1);
  if(droids[0]->state == 2)
  {
    sscore = score;
    drawxpm_ani(&game_over1_bm,
      SCREEN_HSIZE_X - (game_over1_bm.width >> 1),
      50,0,1);
  }
  if(state == 3)
    drawxpm_ani(&paused_bm,
      SCREEN_HSIZE_X - (paused_bm.width >> 1),
      50,0,1);
#ifdef GOD_MODE
  {
    char *cheat_message1 = "You fucking cheat!";

    XDrawString(display,render_screen,gc_dline,
      110,70,cheat_message1,strlen(cheat_message1));
  }
#endif
}

void tship::play_draw(void)
{
  register int x;

  for(x = 0;x < MAX_DROIDS;x++)   
    if(droids[x] != NULL)
      droids[x]->snap_shot();
    else
      break;
  curr_location->floor->set_pos(
    droids[0]->spos_x,droids[0]->spos_y);
  curr_location->floor->draw();
  for(x = 1;x < MAX_DROIDS;x++)
    if(droids[x] != NULL)
      droids[x]->draw(
        curr_location->floor,
        droids[0]->spos_x,
        droids[0]->spos_y);
    else
      break;
  ((tparadroid *)droids[0])->draw();
  print_info();
  XCopyArea(display,render_screen,window,gc_bgblt,0,0,
    window_width,window_height,0,0);
}

void tship::level_run(void)
{
  state = 1;
  while(state != 2)
  {
    switch(state)
    {
      case 0:
        diagram_draw();
        break;
      case 1:
      case 3:
        play_draw();
        break;
      case 4:
        console_draw();
        break;
    }
    level_check_event();
    usleep(UPDATE_DELAY);
  }
}
