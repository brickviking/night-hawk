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
* root game routines
*
****************************************************************************
****************************************************************************
***************************************************************************/
#include "defs.h"
#include "protos.p"

#include "tship.h"

/*
 * nighthawk is assumed to have a max of 10 ships . no more unless you
 * make space
 */
PUBLIC tship *ship;
PUBLIC char   *ship_table[MAX_SHIPS + 1];
PUBLIC int    ship_ptr, ship_start, god_mode, reduced_f;

PUBLIC tdroid_stats droid_stats[] = {
  {
    "002",
    "Paradroid",
    1,
    1.0,
    27.0,
    "Neurologic",
    LASER_LINARITE,
    10,
    10,
    4.0,
    0.0
  },
  {
    "108",
    "Minor Cleaning Droid",
    1,
    0.6,
    38.3,
    "Embedded Logic",
    LASER_NONE,
    1,
    1,
    1.0,
   -1.0
  },
  {
    "176",
    "Standard Cleaning Droid",
    1,
    1.2,
    80.6,
    "Embedded Logic",
    LASER_NONE,
    5,
    5,
    2.0,
   -1.0
  },
  {
    "261",
    "Cargo Paletting Droid",
    2,
    3,
    800.75,
    "Embedded Logic",
    LASER_NONE,
    500,
    500,
    0.5,
   -1
  },
  {
    "275",
    "Servant Droid",
    2,
    0.4,
    15.3,
    "Embedded Logic",
    LASER_NONE,
    15,
    15,
    3.0,
   -1.0
  },
  {
    "355",
    "Messenger (Class C) Droid",
    3,
    0.5,
    10.0,
    "Embedded Logic",
    LASER_NONE,
    10,
    10,
    3.5,
   -1.0
  },
  {
    "368",
    "Messenger (Class D) Droid",
    3,
    0.76,
    11.0,
    "Embedded Logic",
    LASER_NONE,
    20,
    20,
    3.6,
   -1.0
  },
  {
    "423",
    "Standard Maintenance Droid",
    4,
    2,
    60.0,
    "Embedded Logic",
    LASER_NONE,
    20,
    20,
    1.8,
   -1.0
  },
  {
    "467",
    "Heavy Duty Maintenance Droid",
    4,
    1.5,
    100.0,
    "Embedded Logic",
    LASER_CROC_BENZ,
    25,
    25,
    1.3,
   -1.0
  },
  {
    "489",
    "Aerial Maintenance Droid",
    4,
    0.2,
    5.0,
    "Embedded Logic",
    LASER_LINARITE,
    25,
    25,
    4.6,
   -1.0
  },
  {
    "513",
    "Trauma Technician",
    5,
    1,
    40.0,
    "Neurologic",
    LASER_CROC_BENZ,
    30,
    30,
    2.3,
    0.0
  },
  {
    "520",
    "Nurse Droid",
    5,
    0.6,
    50.0,
    "Neurologic",
    LASER_CROC_BENZ,
    35,
    35,
    3.0,
    0.0
  },
  {
    "599",
    "Surgeon Droid",
    5,
    2,
    56.80,
    "Neurologic",
    LASER_UVAROVITE,
    45,
    45,
    4.0,
    0.0
  },
  {
    "606",
    "Guard Monitor",
    6,
    1.1,  
    20.0,
    "Neurologic",
    LASER_CROC_BENZ,
    15,
    15,
    2.6,
    50.0
  },
  {
    "691",
    "Sentinel Droid",
    6,
    1.9,
    45.0,
    "Neurologic",
    LASER_CROC_BENZ,
    40,
    40,
    3.9,
    30.0
  },
  {
    "693",
    "Centurion Droid",
    6,
    2.0,  
    50.0,
    "Neurologic",
    LASER_UVAROVITE,
    50,
    50,
    1.9,
    26.0
  },
  {
    "719",
    "Battle Droid",
    7,
    1.5,
    40.92,
    "Neurologic",
    LASER_CROC_BENZ,
    40,
    40,
    1.4,
    38.0
  },
  {
    "720",
    "Flank Droid",
    7,
    1.0,
    30.10,
    "Neurologic",
    LASER_CROC_BENZ,
    30,
    30,
    4.8,
    16.0
  },
  {
    "766",
    "Attack Droid",
    7,
    1.9,
    67.45,
    "Neurologic",
    LASER_UVAROVITE,
    70,
    70,
    2.8,
    30.0
  },
  {
    "799",
    "Heavy Battle Droid",
    7,
    2.0,
    90.98,
    "Neurologic",
    LASER_UVAROVITE,
    100,
    100,
    2.6,
    16.0
  },
  {
    "805",
    "Communications Droid",
    8,
    0.8,
    50.10,
    "Neurologic",
    LASER_UVAROVITE,
    70,
    70,
    2.0,
    22.0
  },
  {
    "810",
    "Engineer Droid",
    8,
    1.74,
    60.00,
    "Neurologic",
    LASER_UVAROVITE,
    80,
    80,
    2.1,
    30.0
  },
  {
    "820",
    "Science Droid",
    8,
    1.0,
    40.00,
    "Neurologic",
    LASER_UVAROVITE,
    90,
    90,
    2.7,
    32.0
  },
  {
    "899",
    "Pilot Droid",
    8,
    1.74,
    60.00,
    "Neurologic",
    LASER_UVAROVITE,
    90,
    90,
    2.0,
    18.0
  },
  {
    "933",
    "Minor Cyborg",
    9,
    3.1,
    89.0,
    "Neurologic",
    LASER_TIGER_EYE,
    20,
    20,
    3.0,
    34.0
  },
  {
    "949",
    "Command Cyborg",
    9,
    2.6,
    34.0,
    "Neurologic",
    LASER_TIGER_EYE,
    70,
    70,
    4.2,
    30.0
  },
  {
    "999",
    "Fleet Cyborg",
    9,
    3.1,
    40.95,
    "Neurologic",
    LASER_TIGER_EYE,
    200,
    200,
    5.0,
    15.0
  }
};

PRIVATE GLdouble game_count;
PRIVATE void     game_ship_start(void);

/***************************************************************************
*
***************************************************************************/
PUBLIC int get_ctrl_file(void)
{
  FILE *fp;
  int  x = 0;
  char str[STR_LABEL_LEN + 1];

  printf("Loading Ship List..\n");
  fp = fopen(INSTALL_DIR "/data/ctrl.d", "r");
  if(fp == NULL)
  {
    perror("get_ctrl_file()");
    fprintf(stderr, "Error: get_ctrl_file()::fopen()\n");
    end_game(-1);
  }

  for(;;)
  {
    if(fgets(str, STR_LABEL_LEN, fp) == NULL)
      break;
    str[strlen(str) - 1] = 0;
    ship_table[x] = (char *)alloc_mem(strlen(str) + 1);
    strcpy(ship_table[x], str);
    x++;
  }

  fclose(fp);

  return 1;
}

/***************************************************************************
*
***************************************************************************/
void display_final_score(void)
{
  char str[STR_LEN + 1];

  sprintf(str, "Your Final Score: %d", score);
  glColor3fv(green_pixel[2]);
  print_str(str, SCREEN_HSIZE_X - ((strlen(str) << 3) >> 1), 180);
}

/***************************************************************************
* Tanel Kulaots's level continuence patch melded into these routines
***************************************************************************/
PRIVATE int game_term_dis_cont;

PRIVATE void game_term_calc(void)
{
  game_count += get_time_lapse();
  if(game_count > 100)
    game_term_dis_cont = 1;
}

PRIVATE void game_term_draw(void)
{
  char    str[STR_LEN + 1];
  tkevent k;

  glColor4fv(bm_colour);
  blit_bm(
    &trans_terminated_bm,
    SCREEN_HSIZE_X - (trans_terminated_bm.size_x >> 1), 50);

  display_final_score();

  if(game_term_dis_cont)
  {
    sprintf(str, "Continue: y/n ?");
    print_str(str, SCREEN_HSIZE_X - ((strlen(str) << 3) >> 1), 150);
  }

//  sound_engine_cmd(SND_CMD_SILENCE,0,0,0);
//  sound_engine_cmd(SND_CMD_FX,FX_TRANS_TERM,0xff,0x80);

  if(!get_kb_event(&k))
    return;

  if(k.type == KEYBOARD_EVENT && tolower(k.val) == 'y')
  {
    game_ship_start_init();
    return;
  }

  update_score(0);
  intro_start_init();
}

PRIVATE void game_term_init(void)
{
  game_count = 0.0;
  game_term_dis_cont = 0;
  draw_h_hook = game_term_draw;
  sync_time_lapse();
  calc_hook = game_term_calc;
}

/***************************************************************************
*
***************************************************************************/
PRIVATE void game_noise_calc(void)
{
  game_count += get_time_lapse();
  if(game_count > 50)
    game_term_init();
}

PRIVATE void game_noise_draw(void)
{
  int x, y;

//  sound_engine_cmd(SND_CMD_RANDOM,0,0,0);

  for(y = 0; y < 200; y += 10)
  {
    for(x = 0; x < 320; x += 10)
    {
      glColor3fv(white_pixel[random() & 0x7]);
      blit_rect(x, y, 10, 10);
    }
  }
}

PRIVATE void game_noise_init(void)
{
  game_count = 0;
  draw_h_hook = game_noise_draw;
  sync_time_lapse();
  calc_hook = game_noise_calc;
}

/***************************************************************************
*
***************************************************************************/
PRIVATE void game_finale_draw(void)
{
  tkevent     k;
  static char *mess =
    "You have completed all levels.\n"
    "WELL DONE !\n"
    "(Send us a screen dump)\n"
    "\n"
    "Thank you for playing Nighthawk.\n";

  update_score(1);

  glColor3fv(white_pixel[2]);
  display_message(mess, 40);
  display_final_score();

  if(!get_kb_event(&k))
    return;

  intro_start_init();
}

PRIVATE void game_finale_init(void)
{
  draw_h_hook = game_finale_draw;
  sync_time_lapse();
  calc_hook = NULL;
}

/***************************************************************************
*
***************************************************************************/
PRIVATE void delete_ship(void)
{
  ship->unload();
  delete(ship);
}

PRIVATE void game_run_calc(void)
{
  static double ud;

  if(ship != NULL)
    ship->level_bg_calc();

  ud += get_time_lapse();
  if(ud >= 2.0)
  {
    ud = 0.0;
    if(score != sscore)
      sscore += ((score - sscore) >> 1) | 1;
  }
}

PRIVATE void game_run_draw(void)
{
  ship->level_run();

  if(ship->state == STATE_END)
  {
    delete_ship();

    game_noise_init();
    return;
  }

  if(ship->state == STATE_COMPLETE)
  {
    last_score = score;
    delete_ship();

    ship_ptr++;
    if(ship_table[ship_ptr] != NULL)
    {
      game_ship_start();
      return;
    }

    ship_ptr--;
    game_finale_init();
  }
}

PRIVATE void game_run_init(void)
{
  draw_h_hook = game_run_draw;
  calc_hook = game_run_calc;
}

/***************************************************************************
*
***************************************************************************/
PRIVATE void game_ship_start(void)
{
  ship = new(tship);
  if(ship == NULL)
    malloc_error_h();

  ship->load(ship_table[ship_ptr]);

//  sound_engine_cmd(SND_CMD_FX_INIT,0,0,0);

  game_run_init();
}

PUBLIC void game_ship_start_init(void)
{
  score = last_score;

  draw_h_hook = game_ship_start;
  sync_time_lapse();
  calc_hook = NULL;
}
