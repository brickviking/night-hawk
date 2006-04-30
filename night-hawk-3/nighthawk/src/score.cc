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
* score routines
*
****************************************************************************
****************************************************************************
***************************************************************************/
#include "defs.h"
#include "protos.p"

PUBLIC int          score, sscore, last_score;
PUBLIC tscore_table score_table[MAX_SCORE_TABLE];

/***************************************************************************
*
***************************************************************************/
PUBLIC void init_scores(void)
{
  strcpy(score_table[0].name, "Wolfgang");
  strcpy(score_table[0].highest_ship, "Tobruk");
  score_table[0].score = 100000;

  strcpy(score_table[1].name, "Rafael");
  strcpy(score_table[1].highest_ship, "Zaxon");
  score_table[1].score = 50000;

  strcpy(score_table[2].name, "Eero");
  strcpy(score_table[2].highest_ship, "Discovery");
  score_table[2].score = 25000;

  strcpy(score_table[3].name, "dionb");
  strcpy(score_table[3].highest_ship, "Friendship");
  score_table[3].score = 15000;

  strcpy(score_table[4].name, "zerodogg");
  strcpy(score_table[4].highest_ship, "Mearkat");
  score_table[4].score = 10000;

  strcpy(score_table[5].name, "Vincent");
  strcpy(score_table[5].highest_ship, "Ophukus");
  score_table[5].score = 8000;

  strcpy(score_table[6].name, "Adrian");
  strcpy(score_table[6].highest_ship, "Esperence");
  score_table[6].score = 5000;

  strcpy(score_table[7].name, "Tanel");
  strcpy(score_table[7].highest_ship, "Anoyle");
  score_table[7].score = 3000;

  strcpy(score_table[8].name, "Nelson");
  strcpy(score_table[8].highest_ship, "Seafarer");
  score_table[8].score = 1000;
}

/***************************************************************************
*
***************************************************************************/
PUBLIC void load_scores(void)
{
  FILE *fp;

  fp = fopen(SCORES_FILE, "r");
  if(fp == NULL)
  {
    init_scores();
    return;
  }
  fread(score_table, sizeof(tscore_table), MAX_SCORE_TABLE, fp);
  fclose(fp);
}

/***************************************************************************
*
***************************************************************************/
PUBLIC void update_score(int complete)
{
  FILE          *fp;
  int           y, a, x;
  struct passwd *ps;

  fp = fopen(SCORES_FILE, "w");
  if(fp == NULL)
    return;

  for(x = 0; x < MAX_SCORE_TABLE; x++)
  {
    if(score > score_table[x].score)
    {
      for(y = MAX_SCORE_TABLE - 1; y > x; y--)
        memcpy(
          (void *)&(score_table[y]), (void *)&(score_table[y - 1]), sizeof(tscore_table));

      ps = getpwuid(getuid());
      if(ps != NULL)
        strncpy(score_table[x].name, ps->pw_name, 8);
      else
        strcpy(score_table[x].name, "unknown");

      if(ship_start)
      {
        strncpy(score_table[x].highest_ship, ship_table[ship_ptr], 15);
        a = strlen(score_table[x].highest_ship);
        score_table[x].highest_ship[a] = '*';
        score_table[x].highest_ship[a + 1] = '0' + (char)ship_start;
      }
      else
      {
        if(complete)
          strcpy(score_table[x].highest_ship, "Complete!");
        else
          strncpy(score_table[x].highest_ship, ship_table[ship_ptr], 15);
      }

      score_table[x].highest_ship[15] = 0;
      score_table[x].score = score;
      break;
    }
  }

  rewind(fp);
  fwrite(score_table, sizeof(tscore_table), MAX_SCORE_TABLE, fp);
  fclose(fp);
}
