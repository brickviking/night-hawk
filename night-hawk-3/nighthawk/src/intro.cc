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
* game intro routines
*
****************************************************************************
****************************************************************************
***************************************************************************/
#include "defs.h"
#include "protos.p"

PRIVATE GLdouble intro_count;
PRIVATE int      intro_sequence_no, 
                 intro_back_bm_no;

PRIVATE char *scroll_story = 
"          -= Paradroid =-           "
"                                    "
" Chapter II - The Rescue from Vega! "
"                                    "
"                                    "
"The  Earth  forces   have   ambushed"
"an enemy fleet transporting captured"
"Earth vessels bound for  Vega. While"
"crews were boarding  these ships for"
"repairs,  surviving   Vegarian  crew"
"destroyed  each   ships  warp  core."
"This  has   made   the   ships  very"
"radioactive,  causing  many  of  the"
"droids on  board  these  ships to go"
"'rogue'.  Our last  contact with the"
"crew indicated heavy  loss  of life."
"All attempts  at  contact  or rescue"
"have proved futile. We can only fear"
"the worst..                         "
"                                    "
"..However, we have depatched a Class"
"002 Paradroid. Improved over the 001"
"class, the 002 will teleport to each"
"ship and annihilate all droids.     "
"                                    "
"                                    "
"        Hit Space to Start.         ";

/***************************************************************************
*
***************************************************************************/
PRIVATE void pause_1_calc(void)
{
  intro_count += get_time_lapse();
  if(intro_count > 100)
  {
    intro_count = 0;
    intro_sequence_no++;
    intro_back_bm_no = random() & 0x3;
  }
}

PRIVATE void pause_2_calc(void)
{
  intro_count += get_time_lapse();
  if(intro_count > 20)
  {
    intro_count = 0;
    intro_sequence_no++;
    intro_back_bm_no = random() & 0x3;
  }
}

PRIVATE void pause_3_calc(void)
{
  intro_count += get_time_lapse();
  if(intro_count > 150)
  {
    intro_count = 0;
    intro_sequence_no++;
    intro_back_bm_no = random() & 0x3;
  }
}

/***************************************************************************
*
***************************************************************************/
PRIVATE void credit_1_fg(void)
{
  static char *mess1 = "Based on Andrew Braybrooks",
              *mess2 = "Paradroid.";

  print_str_hazey(mess1, 56, 44, intro_haze[0], intro_haze[1]);
  print_str_hazey(mess2, 56, 60, intro_haze[0], intro_haze[1]);
}

PRIVATE void credit_2_fg(void)
{
  static char *mess1 = "By Jason Nunn",
              *mess2 = "Eric Gillespie and other",
              *mess3 = "'Night-Hawk' development members..";

  print_str_hazey(mess1, 108, 44, intro_haze[0], intro_haze[1]);
  print_str_hazey(mess2, 64, 60, intro_haze[0], intro_haze[1]);
  print_str_hazey(mess3, 24, 76, intro_haze[0], intro_haze[1]);
}

PRIVATE void main_logo_fg(void)
{
  static char *mess1 = "(C) 1996-2004",
              *mess2 = "Jason Nunn, Eric Gillespie et al";

  glColor4fv(bm_colour);
  blit_bm(&credit_6_bm, SCREEN_HSIZE_X - (credit_6_bm.size_x >> 1), 10);

  print_str_hazey(mess1, 108, 45, intro_haze[0], intro_haze[1]);
  print_str_hazey(mess2, 32, 61, intro_haze[0], intro_haze[1]);
}

PRIVATE void main_logo_fg2(void)
{
  static char *mess = "Space to play, 'q' to Quit,\n";

  main_logo_fg();

  glColor3fv(white_pixel[2]);
  display_message(mess, 160);
}

PRIVATE void credit_6_fg(void)
{
  main_logo_fg();
}

PRIVATE void credit_7_fg(void)
{
  main_logo_fg2();
}

/***************************************************************************
*
***************************************************************************/
PRIVATE char *scroll_base_ptr;
PRIVATE int  scroll_limit;

PRIVATE void draw_scroll_calc(void)
{
  intro_count += get_time_lapse();
  if(intro_count > scroll_limit)
  {
    intro_count = 0;
    intro_sequence_no++;
    intro_back_bm_no = 0;
  }
}

PRIVATE void draw_scroll_init_1_fg(void)
{
  scroll_base_ptr = scroll_story;
  intro_count = -(210 << 2);
  intro_sequence_no++;
  scroll_limit = 1730;
}

PRIVATE void draw_scroll_fg(void)
{
  int i, a, b, c, d, p, xp;

  d = strlen(scroll_base_ptr);
  i = (int)(intro_count / 4);
  b = i / 16;
  c = i % 16;
  for(a = 0; a < 16; a++)
  {
    p = (a + b) * 36;
    if((p >= 0) && (p < d))
    {
      xp = (a << 4) - c;

      glColor3f(0.0, (100 - (GLfloat)abs(100 - xp)) / 100, 0.0);
      print_str_len(scroll_base_ptr + p, 36, 16, xp - 8);
    }
  }
}

/***************************************************************************
*
***************************************************************************/
PRIVATE void credit_8_fg(void)
{
  static char *mess1 = "= Contributors =\n",
              *mess2 = "Vincent Voois (Music)\n"
                       "A.Bridgett, E.Tamminen, W.Scherer\n"
                       "N.Minar, R.Laboissiere, T.Kulaots\n",
              *mess3 = "= Night-Hawk development members =\n",
              *mess5 = "viking667@users.sourceforge.net,\n"
                       "dionb@users.sourceforge.net &\n"
                       "zerodogg@users.sourceforge.net\n";

  glColor4fv(bm_colour);
  blit_bm(&intro_back_bm[intro_back_bm_no], 0, 0);

  glColor3fv(yellow_pixel[0]);
  display_message(mess1, 20);
  display_message(mess3, 110);

  glColor3fv(white_pixel[5]);
  display_message(mess2, 40);
  display_message(mess4, 130);
}

/***************************************************************************
*
***************************************************************************/
PRIVATE void contacts_1_fg(void)
{
  static char *mess1 = "=GPL=\n",
              *mess2 =
    "\n"
    "Nighthawk   comes   under  the   GNU\n"
    "general public license agreement,and\n"
    "comes  with  ABSOLUTELY NO WARRANTY.\n"
    "This is free software,  and  you are\n"
    "welcome  to  redistribute  it  under\n"
    "certain conditions.\n";

  glColor4fv(bm_colour);
  blit_bm(&intro_back_bm[intro_back_bm_no], 0, 0);

  glColor3fv(yellow_pixel[0]);
  display_message(mess1, 20);

  glColor3fv(white_pixel[5]);
  display_message(mess2, 40);
}

PRIVATE void contacts_2_fg(void)
{
  static char *mess1 = "=Contacts=\n",
              *mess2 = "Eric Gillespie (The Viking)\n"
                       "viking667@users.sourceforge.net\n",
              *mess3 = "-Website-\n",
              *mess4 = "http://night-hawk.sourceforge.net\n",
              *mess5 = "-Original Author-\n",
              *mess6 = "Jason Nunn (JsNO)\n"
                       "http://jsno.leal.com.au\n"
                       "C/- Jason Nunn\n"
                       "PO Box 15, Birdwood, SA, 5234\n"
                       "AUSTRALIA\n";

  glColor4fv(bm_colour);
  blit_bm(&intro_back_bm[intro_back_bm_no], 0, 0);

  glColor3fv(yellow_pixel[0]);
  display_message(mess1, 5);
  display_message(mess3, 60);
  display_message(mess5, 100);

  glColor3fv(white_pixel[5]);
  display_message(mess2, 20);
  display_message(mess4, 75);
  display_message(mess6, 115);
}

PRIVATE void contacts_4_fg(void)
{
  static char *mess =
    "Thanks to the  many  people who have\n"
    "emailed  feedback  about  Nighthawk.\n"
    "Support Free Software!\n"
    "\n"
    "Thanks also to Sourceforge.net\n"
    "for hosting this fantastic project\n"
    "and many many others, including\n"
    "paradroid clones well worth a play\n"
    "when you have moments of nostalgia\n"
    "http://reedroid.sourceforge.net/\n"
    "http://paradroid.sourceforge.net/\n"
    "\n"
    "Hit Space to Start.\n";

  glColor4fv(bm_colour);
  blit_bm(&intro_back_bm[intro_back_bm_no], 0, 0);

  glColor3fv(white_pixel[5]);
  display_message(mess, 10);
}

/***************************************************************************
*
***************************************************************************/
PRIVATE void display_scores_calc(void)
{
  intro_count += get_time_lapse();
  if(intro_count > 80)
  {
    intro_count = 0;
    intro_sequence_no++;
    intro_back_bm_no = random() & 0x3;
  }
}

PRIVATE void display_scores_fg(void)
{
  char str[STR_LEN + 1];
  int  x, yp;

  glColor4fv(bm_colour);
  blit_bm(&intro_back_bm[intro_back_bm_no], 0, 0);
  blit_bm(&ntitle_bm, SCREEN_HSIZE_X - (ntitle_bm.size_x >> 1),5);

  sprintf(str, "%-8s %-15s    Score", "Name", "Ship");
  glColor3fv(red_pixel[0]);
  print_str(str, 20, 52);

  for(x = 0, yp = 70; x < MAX_SCORE_TABLE; x++, yp += 13)
  {
    sprintf(str, "%-8s %-15s %8d",
      score_table[x].name,
      score_table[x].highest_ship,
      score_table[x].score);

    glColor3fv(green_pixel[x]);
    print_str(str, 20, yp);
  }
}

/***************************************************************************
*
***************************************************************************/
PRIVATE sequence_tab_t intro_tab[] = {
  {pause_2_calc,        NULL},
  {pause_1_calc,        credit_1_fg},
  {pause_2_calc,        NULL},
  {pause_1_calc,        credit_2_fg},
  {pause_2_calc,        NULL},
  {pause_1_calc,        credit_6_fg},
  {pause_1_calc,        credit_7_fg},
  {pause_1_calc,        credit_7_fg},
  {draw_scroll_calc,    draw_scroll_init_1_fg},  /*restart*/
  {draw_scroll_calc,    draw_scroll_fg},
  {pause_2_calc,        NULL},
  {pause_3_calc,        credit_8_fg},
  {pause_3_calc,        contacts_1_fg},
  {pause_3_calc,        contacts_2_fg},
  {pause_3_calc,        contacts_4_fg},
  {display_scores_calc, display_scores_fg},
  {display_scores_calc, display_scores_fg},
  {display_scores_calc, display_scores_fg},
  {NULL,                NULL}
};

PRIVATE void intro_calc(void)
{
  if(intro_tab[intro_sequence_no].bg != NULL)
    intro_tab[intro_sequence_no].bg();
  else
    intro_sequence_no = 8;
}

PRIVATE void intro_draw_h(void)
{
  tkevent k;

  if(intro_tab[intro_sequence_no].fg != NULL)
    intro_tab[intro_sequence_no].fg();

  if(!get_kb_event(&k))
    return;

  if(k.type == KEYBOARD_EVENT && k.val == KEY_QUIT)
    exit(0);

  game_ship_start_init();
}

/***************************************************************************
*
***************************************************************************/
PUBLIC void intro_start_init(void)
{
  ship_ptr = ship_start;
  sscore = score = last_score = 0;

  intro_count = 0.0;
  intro_sequence_no = 0;
  intro_back_bm_no = 0;

  draw_h_hook = intro_draw_h;
  sync_time_lapse();
  calc_hook = intro_calc;

//  sound_engine_cmd(SND_CMD_LOAD_SONG,0,0,0);
}
