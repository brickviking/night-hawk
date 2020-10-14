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
* ship object
*
****************************************************************************
****************************************************************************
***************************************************************************/
#include "defs.h"
#include "protos.p"

#include "tship.h"

/***************************************************************************
*
***************************************************************************/
tship::tship(void)
{
  int x;

  floor_list = NULL;
  transport_list = NULL;
  curr_location = NULL;

  for(x = 0; x < MAX_DROIDS; x++)
    droids[x] = NULL;

  ship_complete_f = 0;
  ship_end_cd = 200.0;
  state = STATE_GET_READY;
  get_ready_count = 0.0;
}

/***************************************************************************
*
***************************************************************************/
void tship::create_floor(tfloor_list **ptr, char *fname)
{
  char str[STR_LEN + 1];

  if(*ptr == NULL)
  {
    *ptr = (tfloor_list *)alloc_mem(sizeof(tfloor_list));
    (*ptr)->next = NULL;

    (*ptr)->curr = new tfloor();
    if((*ptr)->curr == NULL)
      malloc_error_h();

    strcpy(str, INSTALL_DIR "/data/");
    strcat(str, name);
    strcat(str, "/");
    strcat(str, fname);
    (*ptr)->curr->load(str, fname);
  }
  else
    create_floor(&((*ptr)->next), fname);
}

/***************************************************************************
*
***************************************************************************/
tfloor *tship::find_floor(char *name)
{
  tfloor_list *p = floor_list;   
  tfloor      *s = NULL;
 
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

/***************************************************************************
*
***************************************************************************/
ttransport_list *tship::create_transport(
  ttransport_list **ptr, char *fn, int px, int py, int fx, int fy, int tn)
{
  ttransport_list *r = NULL;

  if(*ptr == NULL)
  {
    *ptr = (ttransport_list *)alloc_mem(sizeof(ttransport_list));
    (*ptr)->floor = find_floor(fn);
    (*ptr)->diagram_px = px;
    (*ptr)->diagram_py = py;
    (*ptr)->floor_pos_x = fx;
    (*ptr)->floor_pos_y = fy;
    (*ptr)->no = tn;
    (*ptr)->next = NULL;
    r = *ptr;
  }
  else
    r = create_transport(&((*ptr)->next), fn, px, py, fx, fy, tn);

  return r;
}

/***************************************************************************
*
***************************************************************************/
ttransport_list *tship::find_transport(char *name, int x, int y)
{
  ttransport_list *p = transport_list;   
  ttransport_list *s = NULL;
 
  while(p != NULL)
  {
    if(!strcmp(name,p->floor->name) &&
       (x == p->floor_pos_x) &&
       (y == p->floor_pos_y))
    {
      s = p;
      break;
    }
    p = p->next;
  }

  return s;
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

void tship::free_all_transport(ttransport_list **ptr)
{
  if(*ptr != NULL)
  {
    free_all_transport(&((*ptr)->next));
    free(*ptr);
    *ptr = NULL;
  }
}

/***************************************************************************
*
***************************************************************************/
/*
void tship::set_ship_noise(void)
{
  if(curr_location->floor->ship_noise_no != -1)
    sound_engine_cmd(
      SND_CMD_FX_SHIP_NOISE,
      curr_location->floor->ship_noise_no + FX_SHIP_NOISE_START,
      curr_location->floor->ship_noise_vol,
      curr_location->floor->ship_noise_freq);
}
*/

/***************************************************************************
*
***************************************************************************/
void tship::load(char *shipname)
{
  FILE *fp;
  char str[STR_LEN + 1],
       amble[STR_LABEL_LEN + 1],
       floor_n[STR_LABEL_LEN + 1];
  int  droid_ptr = 1;

  strcpy(name, shipname);
  strcpy(str, INSTALL_DIR "/data/");
  strcat(str, name);
  strcat(str, "/chars");
  fp = fopen(str, "r");
  if(fp == NULL)
  {
    fprintf(stderr, "Error: tship::load::fopen()\n");
    end_game(-1);
  }

  while(!feof(fp))
  {
    if(fgets(str, STR_LEN, fp) == NULL)
      break;

    if(!str[0])
      continue;

    sscanf(str, "%s", amble);
    if(!strcmp(amble, "type:"))
    {
      strncpy(type, str + 6, STR_LABEL_LEN);
      type[strlen(type) - 1] = 0;
    }
    else if(!strcmp(amble, "floor:"))
    {
      sscanf(str,"%*s %s", floor_n);
      create_floor(&floor_list, floor_n);
    }
    else if(!strcmp(amble, "transport:"))
    {
      int             px, py, fx, fy, tn;
      ttransport_list *p;

      sscanf(str, "%*s %s %d %d %d %d %d", floor_n, &px, &py, &fx, &fy, &tn);
      p = create_transport(&transport_list, floor_n, px, py, fx, fy, tn);
    }
    else if(!strcmp(amble, "init:"))
    {
      int x, y;

      sscanf(str, "%*s %s %d %d", floor_n, &x, &y);
      curr_location = find_transport(floor_n, x, y);
      if(curr_location != NULL)
      {
        droids[0] = new(tparadroid);
        if(droids[0] == NULL)
          malloc_error_h();

        ((tparadroid *)droids[0])->init(
          curr_location->floor,(tentity **)&droids, x, y);
      }
    }
    else if(!strcmp(amble, "droid:"))
    {
      if(droid_ptr < MAX_DROIDS)
      {
        int    x, y;
        tfloor *f;

        sscanf(str, "%*s %s %s %d %d", amble, floor_n, &x, &y);
        f = find_floor(floor_n);
        if(f != NULL)
        {
          if(!strcmp(amble, "108"))
            droids[droid_ptr] = new(tdroid_108);
          else if(!strcmp(amble, "176"))
            droids[droid_ptr] = new(tdroid_176);
          else if(!strcmp(amble, "261"))
            droids[droid_ptr] = new(tdroid_261);
          else if(!strcmp(amble, "275"))
            droids[droid_ptr] = new(tdroid_275);
          else if(!strcmp(amble, "355"))
            droids[droid_ptr] = new(tdroid_355);
          else if(!strcmp(amble, "368"))
            droids[droid_ptr] = new(tdroid_368);
          else if(!strcmp(amble, "423"))
            droids[droid_ptr] = new(tdroid_423);
          else if(!strcmp(amble, "467"))
            droids[droid_ptr] = new(tdroid_467);
          else if(!strcmp(amble, "489"))
            droids[droid_ptr] = new(tdroid_489);
          else if(!strcmp(amble, "513"))
            droids[droid_ptr] = new(tdroid_513);
          else if(!strcmp(amble, "520"))
            droids[droid_ptr] = new(tdroid_520);
          else if(!strcmp(amble, "599"))
            droids[droid_ptr] = new(tdroid_599);
          else if(!strcmp(amble, "606"))
            droids[droid_ptr] = new(tdroid_606);
          else if(!strcmp(amble, "691"))
            droids[droid_ptr] = new(tdroid_691);
          else if(!strcmp(amble, "693"))
            droids[droid_ptr] = new(tdroid_693);
          else if(!strcmp(amble, "719"))
            droids[droid_ptr] = new(tdroid_719);
          else if(!strcmp(amble, "720"))
            droids[droid_ptr] = new(tdroid_720);
          else if(!strcmp(amble, "766"))
            droids[droid_ptr] = new(tdroid_766);
          else if(!strcmp(amble, "799"))
            droids[droid_ptr] = new(tdroid_799);
          else if(!strcmp(amble, "805"))
            droids[droid_ptr] = new(tdroid_805);
          else if(!strcmp(amble, "810"))
            droids[droid_ptr] = new(tdroid_810);
          else if(!strcmp(amble, "820"))
            droids[droid_ptr] = new(tdroid_820);
          else if(!strcmp(amble, "899"))
            droids[droid_ptr] = new(tdroid_899);
          else if(!strcmp(amble, "933"))
            droids[droid_ptr] = new(tdroid_933);
          else if(!strcmp(amble, "949"))
            droids[droid_ptr] = new(tdroid_949);
          else if(!strcmp(amble, "999"))
            droids[droid_ptr] = new(tdroid_999);
          else
          {
            fprintf(stderr, "Error: Unknown droid: %s\n", amble);
            end_game(-1);
          }

          if(droids[droid_ptr] == NULL)
            malloc_error_h();

          droids[droid_ptr]->init(f, (tentity **)&droids, x, y);

          while(!feof(fp))
          {
            long lpos;

            lpos = ftell(fp);
            if(fgets(str, STR_LEN, fp) == NULL)
              continue;

            if(str[0] == '*')
            {
              char c;

              sscanf(
                str, "%*c %s %d %d", &c, &x, &y);

              ((tdroid_108 *)droids[droid_ptr])->append_cmd(
                &(((tdroid_108 *)droids[droid_ptr])->droid_cmd_list),
                c, x, y);
            }
            else
            {
              fseek(fp, lpos, SEEK_SET);
              break;
            }
          }
          droid_ptr++;
        }
      }
    }
  }

  fclose(fp);

  strcpy(str, name);
  strcat(str, "/map.xpm");
  load_bm(str, &map_bm, 1);

  sprintf(str, "SS %s", shipname);
  console_message_add(str);
  droids[0]->display_nd();
}

void tship::unload(void)
{
  int x;

  free_all_floor(&floor_list);
  free_all_transport(&transport_list);

 /*
  * normally, all tex's are loaded at start... accept for these, which 
  * are continuely loaded for each level... therefore, to save filling 
  * up text memory, we much dispose them.
  */
  delete_tex(map_bm.id);

  for(x = 0; x < MAX_DROIDS; x++)
    if(droids[x] != NULL)
      delete(droids[x]);
}

/***************************************************************************
*
***************************************************************************/
void tship::get_ready_draw(void)
{
  char str[STR_LABEL_LEN + 1];

//  int    siren_r = 0, siren_c = 0;
//  if(siren_c < 3)
//    if(!siren_r)
//    {
//      siren_r = 6;
//      siren_c++;
//      sound_engine_cmd(SND_CMD_FX,FX_ALERT_SIREN,0xff,0x80);
//      sound_engine_cmd(SND_CMD_FX,FX_ALERT_SIREN,0xff,0x80);
//    }
//    else
//      siren_r--;

  glColor4fv(bm_colour);
  blit_bm(&get_ready1_bm, SCREEN_HSIZE_X - (get_ready1_bm.size_x >> 1), 12);
  blit_bm(&(ship->map_bm), SCREEN_HSIZE_X - ship_bm_offs_x[ship_ptr], 70);

  sprintf(str, "Starship %s", ship->name);
  glColor3fv(purple_pixel[0]);
  print_str(str, SCREEN_HSIZE_X - ((strlen(str) >> 1) * 7), 50);
}

/***************************************************************************
*
***************************************************************************/
#define LEVEL_DRAW_SIZE_X 300
#define LEVEL_DRAW_SIZE_Y 160

void tship::diagram_draw(void)
{
  int  ox, oy, sprtx, sprty;
  char str[STR_LEN + 1];

  if(!reduced_f)
  {
    glColor4fv(bm_colour);
    blit_bm(&ntitle_bm,0,0);
  }

  print_score();

  ox = SCREEN_HSIZE_X - (LEVEL_DRAW_SIZE_X >> 1);
  oy = SCREEN_HSIZE_Y - (LEVEL_DRAW_SIZE_Y >> 1) + 10;

  glColor3fv(blue_pixel[0]);
  blit_rect(ox, oy, LEVEL_DRAW_SIZE_X, LEVEL_DRAW_SIZE_Y);
  glColor3fv(blue_pixel[4]);
  blit_rect_hollow(ox, oy, LEVEL_DRAW_SIZE_X, LEVEL_DRAW_SIZE_Y);

  glColor3fv(blue_pixel[7]);
  sprintf(str, "StarShip %s (%s)\n", name, type);
  display_message(str, oy + 8);

  sprtx = ox + (LEVEL_DRAW_SIZE_X >> 1) - ship_bm_offs_x[ship_ptr];
  sprty = oy + (LEVEL_DRAW_SIZE_Y >> 1) - (map_bm.size_y >> 1) + 10;

  glColor4fv(bm_colour);
  blit_bm(&map_bm, sprtx, sprty);

  sprtx += curr_location->diagram_px;
  sprty += curr_location->diagram_py;
  glColor3fv(yellow_pixel[0]);
  blit_rect_hollow(sprtx - 1, sprty - 2, 3, 3);
}

/***************************************************************************
*
***************************************************************************/
void tship::transport_set(void)
{
  ttransport_list *p = transport_list;

  while(p != NULL)
  {
    if(!strcmp(curr_location->floor->name, p->floor->name))
    {
      int ix, iy;

      ix = p->floor_pos_x - droids[0]->spos_x + SPRITE_HSIZE_X;
      iy = p->floor_pos_y - droids[0]->spos_y + SPRITE_HSIZE_Y;
      if((ix >= 0) && (ix < SPRITE_SIZE_X) &&
         (iy >= 0) && (iy < SPRITE_SIZE_Y))
      {
        curr_location->floor->reinit();
        curr_location = p;
        break;
      }
    }
    p = p->next;
  }
}

void tship::transport_change(ttransport_list *selection)
{
  if(selection != NULL)
  {
    curr_location = selection;
    ((tparadroid *)droids[0])->init(
      curr_location->floor,(tentity **)&droids,
      curr_location->floor_pos_x,curr_location->floor_pos_y);
  }
}

void tship::transport_change_up(void)
{
  ttransport_list *p = transport_list,
                  *s = NULL;
  int             selection = -1;

  while(p != NULL)
  {
    if(p->no == curr_location->no)
    {
      if(p->diagram_py < curr_location->diagram_py)
        if(p->diagram_py > selection)
        {
          selection = p->diagram_py;
          s = p;
        }
    }
    p = p->next;
  }
  transport_change(s);
}

void tship::transport_change_dn(void)
{
  ttransport_list *p = transport_list,*s = NULL;
  int             selection = 0xffff;

  while(p != NULL)
  {
    if(p->no == curr_location->no)
    {
      if(p->diagram_py > curr_location->diagram_py)
        if(p->diagram_py < selection)
        {
          selection = p->diagram_py;
          s = p;
        }
    }
    p = p->next;
  }
  transport_change(s);
}

/***************************************************************************
*
***************************************************************************/
void tship::console_draw(void)
{
  int         ox, oy;
  char        str[STR_LEN + 1];
  static char *header_mess = "Ship Computer- Droid Records\n";

  if(!reduced_f)
  {
    glColor4fv(bm_colour);
    blit_bm(&ntitle_bm, 0, 0);
  }

  ox = SCREEN_HSIZE_X - (LEVEL_DRAW_SIZE_X >> 1);
  oy = SCREEN_HSIZE_Y - (LEVEL_DRAW_SIZE_Y >> 1) + 10;

  glColor3fv(green_pixel[7]);
  blit_rect(ox, oy, LEVEL_DRAW_SIZE_X, LEVEL_DRAW_SIZE_Y);
  glColor3fv(green_pixel[2]);
  blit_rect_hollow(ox, oy, LEVEL_DRAW_SIZE_X, LEVEL_DRAW_SIZE_Y);

  glColor3fv(white_pixel[7]);
  display_message(header_mess, oy + 2);

  glColor3fv(green_pixel[0]);
  sprintf(
    str, "Current: %s (%s)\n", droids[0]->stats.type, droids[0]->stats.name);
  display_message(str, oy + 20);

  ox += 5;
  oy += 38;

  glColor3fv(green_pixel[3]);
  sprintf(str, "  Type: %s", droid_stats[console_ptr].type);
  print_str(str, ox, oy);

  sprintf(str, "  Name: %s", droid_stats[console_ptr].name);
  print_str(str, ox, oy + (12 * 1));

  sprintf(str, " Entry: %d", droid_stats[console_ptr].entry);
  print_str(str, ox, oy + (12 * 2));

  sprintf(str, "Height: %.2f Metres", droid_stats[console_ptr].height);
  print_str(str, ox, oy + (12 * 3));

  sprintf(str, "Weight: %.2f kg", droid_stats[console_ptr].weight);
  print_str(str, ox, oy + (12 * 4));

  sprintf(str, " Brain: %s",droid_stats[console_ptr].brain);
  print_str(str, ox, oy + (12 * 5));

  switch(droid_stats[console_ptr].armament)
  {
    case 0:
      sprintf(str, "Arm: None");
      break;
    case 1:
      sprintf(str, "Arm: Linarite Laser");
      break;
    case 2:
      sprintf(str, "Arm: Crocoite-Benzol Laser");
      break;
    case 3:
      sprintf(str, "Arm: Uvarovite Laser");
      break;
    case 4:
      sprintf(str, "Arm: Quartz 'Tiger-Eye' Laser");
      break;
  }
  print_str(str, ox + (8 * 3), oy + (12 * 6));

  sprintf(str, "Shield: %d", droid_stats[console_ptr].max_shielding);
  print_str(str, ox, oy + (12 * 7));

  sprintf(str, " Speed: %.2f", droid_stats[console_ptr].speed);
  print_str(str, ox, oy + (12 * 8));

  if(droid_stats[console_ptr].speed >= 0.0 &&
     droid_stats[console_ptr].speed <= 1.0)
  {
    sprintf(str, "(Very slow)");
  }
  else if(droid_stats[console_ptr].speed >  1.0 && 
          droid_stats[console_ptr].speed <= 2.0)
  {
    sprintf(str, "(slow)");
  }
  else if(droid_stats[console_ptr].speed >  2.0 && 
          droid_stats[console_ptr].speed <= 3.0)
  {
    sprintf(str, "(fair)");
  }
  else if(droid_stats[console_ptr].speed >  3.0 && 
          droid_stats[console_ptr].speed <= 4.0)
  {
    sprintf(str, "(average)");
  }
  else if(droid_stats[console_ptr].speed >  4.0 && 
          droid_stats[console_ptr].speed <= 5.0)
  {
    sprintf(str, "(fast)");
  }
  else if(droid_stats[console_ptr].speed >  5.0 && 
          droid_stats[console_ptr].speed <= 6.0)
  {
    sprintf(str, "(Extremely fast)");
  }

  print_str(str, ox + (8 * 14), oy + (12 * 8));

  if(droid_stats[console_ptr].attack_rate == -1)
    sprintf(str, "Attack: N/A");
  else
    sprintf(str, "Attack: %.2f", droid_stats[console_ptr].attack_rate);
  print_str(str, ox, oy + (12 * 9));
}

/***************************************************************************
*
***************************************************************************/
void tship::level_check_event(void)
{
  tkevent k;

  for(;;)
  {
    if(!get_kb_event(&k))
      break;

    if(state == STATE_RUN)
    {
      int s;

      s = ((tparadroid *)droids[0])->action(&k);

      if(s == 1)  // paradroid hit space bar over transport bay
      {
        state = STATE_TRANSPORT;
//        sound_engine_cmd(SND_CMD_SILENCE_SNOISE,0,0,0); //turn off ship noise
        transport_set();
        break;
      }

      if(s == 2) // paradroid hit space bar next to console
      {
        for(console_ptr = 0; console_ptr < DROID_999; console_ptr++)
          if(!strcmp(droids[0]->stats.type, droid_stats[console_ptr].type))
            break;

        state = STATE_CONSOLE;
        break;
      }
    }

    switch(state)
    {
      case STATE_TRANSPORT:
        switch(k.type)
        {
          case KEYBOARD_EVENT:
            if(k.val == KEY_SELECT) //paradroid exits transport bay
            {
              state = STATE_RUN;
//            set_ship_noise();     //switch on ship noise
            }
            break;

          case SPECIAL_EVENT:
            switch(k.val)
            {
              case GLUT_KEY_UP:
                transport_change_up();
//                sound_engine_cmd(SND_CMD_FX,FX_SELECT,0xff,0x80);
                break;

              case GLUT_KEY_DOWN:
                transport_change_dn();
//                sound_engine_cmd(SND_CMD_FX,FX_SELECT,0xff,0x80);
                break;
            }
            break;
        }
        break;

      case STATE_RUN:
        if(k.type == KEYBOARD_EVENT)
        {
          switch(k.val)
          {
            case KEY_PAUSE:
              state = STATE_PAUSED;
//              sound_engine_cmd(SND_CMD_FX,FX_SELECT,0xff,0x80);
              break;

            case KEY_QUIT:
              if(!droids[0]->state)
              {
                droids[0]->state = 1;
//                sound_engine_cmd(SND_CMD_FX,FX_EXPLOSION_1,0xff,0x80);
              }
          }
        }
        break;

      case STATE_PAUSED:
        if(k.type == KEYBOARD_EVENT && k.val == KEY_PAUSE)
        {
          state = STATE_RUN;
//          sound_engine_cmd(SND_CMD_FX,FX_SELECT,0xff,0x80);
        }
        break;

      case STATE_CONSOLE:
        switch(k.type)
        {
          case KEYBOARD_EVENT:
            if(k.val == KEY_SELECT)
              state = STATE_RUN;
            break;

          case SPECIAL_EVENT:
            switch(k.val)
            {
              case KEY_SELECT:
                state = STATE_RUN;
                break;

              case GLUT_KEY_LEFT:
                if(console_ptr > 0)
                  console_ptr--;
//                sound_engine_cmd(SND_CMD_FX,FX_SELECT,0xff,0x80);
                break;

              case GLUT_KEY_RIGHT:
                if(console_ptr < DROID_999)
                  console_ptr++;
//                sound_engine_cmd(SND_CMD_FX,FX_SELECT,0xff,0x80);
                break;
            }  
        }
    }
  }
}

/***************************************************************************
*
***************************************************************************/
void tship::level_bg_calc(void)
{
  switch(state)
  {
    case STATE_RUN:
      {
        int x, nd = 0;

        if(curr_location->floor != NULL)
          curr_location->floor->bg_calc();

        for(x = 0; x < MAX_DROIDS; x++)
        {
          if(droids[x] != NULL)
          {
            droids[x]->bg_calc();
            if(x)
              if((*(droids + x))->state < 2)
                nd++;
          }
        }

        if(!nd)
        {
//          if(ship_complete_f == 0)
//            sound_engine_cmd(SND_CMD_FX,FX_SHIP_COMPLETE,0xff,0x80);
          ship_complete_f = 1;
        }
        if(ship_complete_f)
        {
          if(ship_end_cd < 0.0)
            state = STATE_COMPLETE;
          else
            ship_end_cd -= get_time_lapse();
        }

        if(droids[0]->state == 2)
        {
          if(ship_end_cd < 0.0)
            state = STATE_END;
          else
            ship_end_cd -= 3.0 * get_time_lapse();
        }
      }

      console_message_alpha_fade -= 0.002 * get_time_lapse();
      if(console_message_alpha_fade < 0.0)
        console_message_alpha_fade = 0.0;
      break;

    case STATE_GET_READY:
      get_ready_count += get_time_lapse();
      if(get_ready_count > 100.0)
        state = STATE_RUN;
      break;
  }
}

/***************************************************************************
*
***************************************************************************/
void tship::print_score(void)
{
  int x, val,
      d_table[] =
  {
    10000000,
    1000000,
    100000,
    10000,
    1000,
    100,
    10,
    1,
  };

  glColor4fv(bm_colour2);
  val = sscore;
  for(x = 0; x < 8; x++)
  {
    int i;

    i = val / d_table[x];
    val -= i * d_table[x];
    blit_bm(&digit2_bm[i], (x * 11) + 95, 11);
  }
}

void tship::print_info(void)
{
  if(!reduced_f)
  {
    glColor4fv(bm_colour);
    blit_bm(&ntitle_bm, 0, 0);

    glColor4f(0.0, 1.0, 0.0, 0.3);
    print_str(name, SCREEN_SIZE_X - (strlen(name) << 3), 0);
    print_str(
      curr_location->floor->name,
      SCREEN_SIZE_X - (strlen(curr_location->floor->name) << 3), 16);

    console_message_display();
  }

  print_score();

  if(ship_complete_f)
  {
    glColor4fv(bm_colour);
    blit_bm(
      &ship_complete_bm, SCREEN_HSIZE_X - (168 >> 1), 50);
  }

  if(state == STATE_PAUSED)
  {
    glColor4fv(bm_colour);
    blit_bm(&paused_bm, SCREEN_HSIZE_X - (72 >> 1), 50);
  }
}

/***************************************************************************
*
***************************************************************************/
void tship::play_draw(void)
{
  int x;

  for(x = 0; x < MAX_DROIDS; x++)
    if(droids[x] != NULL)
      droids[x]->snap_shot();
    else
      break;

  curr_location->floor->set_pos(
    droids[0]->spos_x,droids[0]->spos_y);

  curr_location->floor->draw();

  for(x = 1; x < MAX_DROIDS; x++)
    if(droids[x] != NULL)
      droids[x]->draw(
        curr_location->floor, droids[0]->spos_x, droids[0]->spos_y);
    else
      break;

  ((tparadroid *)droids[0])->draw();
  print_info();
}

/***************************************************************************
*
***************************************************************************/
void tship::level_run(void)
{
//  sound_engine_cmd(SND_CMD_FX,FX_POWER_UP,0xff,0x80);
//  set_ship_noise();

  switch(state)
  {
    case STATE_TRANSPORT:
      diagram_draw();
      break;

    case STATE_RUN:
    case STATE_PAUSED:
      play_draw();
      break;

    case STATE_CONSOLE:
      console_draw();
      break;

    case STATE_GET_READY:
      get_ready_draw();
      break;
  }

  level_check_event();

//  sound_engine_cmd(SND_CMD_SILENCE_SNOISE,0,0,0);
}
