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
* Main
*
****************************************************************************
****************************************************************************
***************************************************************************/
#include "defs.h"
#include "protos.p"

/***************************************************************************
*
***************************************************************************/
PRIVATE void init(int argc, char *argv[], int mode)
{
  opengl_init(argc, argv, mode);

  srandom(12338);
  umask(~0666);

  if(!get_ctrl_file())
    return;

  load_flr_xpms();
  load_sprite_xpms();
  load_font();

  load_scores();

 /*
  * init colours
  */
  ramp_colour(
    0.4375, 0.4375, 0.4375, 1.0   ,    1.0,    1.0,
    (GLfloat *)white_pixel, 8);

  ramp_colour(
    0.0   , 0.0   , 0.6,    0.0   ,    0.0,    1.0,
    (GLfloat *)blue_pixel, 8);

  ramp_colour(
    0.4   , 1.0   , 0.4   , 0.0   ,    0.5,    0.0,
    (GLfloat *)green_pixel, MAX_SCORE_TABLE);

  ramp_colour(
    1.0   , 0.5   , 0.5   , 0.9375, 0.0625, 0.0625,
    (GLfloat *)red_pixel, MAX_SCORE_TABLE);

 /*
  * kick things off..
  */
  intro_start_init();
}

/***************************************************************************
*
***************************************************************************/
int main(int argc, char *argv[])
{
  int i, i2;

  printf(WELCOME);
  for(i = 1; i < argc; i++)
  {
    if(argv[i][0] == '-')
    {
      i2 = i + 1;

      switch(argv[i][1])
      {
        case 'h':
          printf(
            "Usage: nighthawk <options>\n"
            " -h         Display this help info\n"
            " -c <level> Start at a given ship (0 - 9)\n"
            " -u <speed> Update Speed (5 - 100 ms)\n"
            " -g         God Mode (No damage, no scores)\n"
            "\n");
          return 0;

        case 'c':
          if(i2 <= argc)
            ship_start = atoi(argv[i2]);
          if(ship_start < 0) ship_start = 0;
          if(ship_start > 9) ship_start = 9;
          printf("Starting at level %d (Nb/ High scores will note "
                 "that you have cheated)\n", ship_start);
          break;

        case 'u':
          if(i2 <= argc)
            update_speed = atoi(argv[i2]);
          if(update_speed < 5) update_speed = 5;
          if(update_speed > 100) update_speed = 100;
          break;

        case 'g':
          god_mode = 1;
          break;
      }
    }
  }

  printf(
    " Data Directory: " INSTALL_DIR "/data/\n"
    "Score Directory: " SCORES_FILE "\n"
    "   Update Speed: %u ms\n", update_speed);

  init(argc, argv, GLUT_DOUBLE | GLUT_RGBA);
  printf("Entering Nighthawk..\n");
  glutMainLoop();
  end_game(0);

  return 0;
}
