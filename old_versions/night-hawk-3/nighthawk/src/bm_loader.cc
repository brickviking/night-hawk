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
* ================================================================
*
****************************************************************************
****************************************************************************
***************************************************************************/
#include "defs.h"
#include "protos.p"

PUBLIC tflr_bm     *flr_bm;
PUBLIC int         flr_bm_size;

PUBLIC bm_t        droid_bm[10],
                   paradroid_bm[10],
                   laser_l_bm[4],
                   laser_cb_bm[4],
                   laser_uv_bm[4],
                   laser_te_bm[4],
                   explosion_bm[7],
                   power_bay_bm[5],
                   ntitle_bm,
                   doorh_bm[9],
                   doorv_bm[9],
                   digit_bm[10],
                   digit2_bm[10],
                   ship_complete_bm,
                   credit_6_bm,
                   neg_bm,
                   paused_bm,
                   trans_terminated_bm,
                   get_ready1_bm,
                   intro_back_bm[4];

PUBLIC bm_t        font_bm[FONT_SIZE];

/*
 * this is for the opengl alignment to actual bm size so that
 * ship.cc can properly align them on the screen
 */
PUBLIC int         ship_bm_offs_x[] =
{
  191 >> 1,
  226 >> 1,
  226 >> 1,
  242 >> 1,
  250 >> 1,
  130 >> 1,
  202 >> 1,
  186 >> 1,
  250 >> 1,
  202 >> 1
};

/***************************************************************************
*
***************************************************************************/
PUBLIC void load_bm(char *fn, bm_t *bm, int aligned_f)
{
  GLubyte *buf;

  load_xpm(fn, (unsigned char **)&buf, &(bm->size_x), &(bm->size_y));
  convert_2_tex(buf, bm, aligned_f);
  free(buf);
}

/***************************************************************************
*
***************************************************************************/
PUBLIC void load_bm_ani(char *fn, bm_t *bm, unsigned int len, int aligned_f)
{
  GLubyte      *buf, *tmp_buf;
  unsigned int i, size_x, size_y, i_size_y, tmp_buf_size;

  load_xpm(fn, (unsigned char **)&buf, &size_x, &size_y);

  i_size_y = size_y / len;
  tmp_buf_size = size_x * i_size_y * 4;
  tmp_buf = (GLubyte *)alloc_mem(tmp_buf_size);

  for(i = 0; i < len; i++)
  {
    bm[i].size_x = size_x;
    bm[i].size_y = i_size_y;
    memcpy(tmp_buf, (void *)&buf[i * tmp_buf_size], tmp_buf_size);
    convert_2_tex(tmp_buf, &bm[i], aligned_f);
  }

  free(tmp_buf);
  free(buf);
}

/***************************************************************************
*
***************************************************************************/
PUBLIC void load_font(void)
{
  GLubyte      *src;
  unsigned int size_x, size_y, fx, fy, y, soffs, dyo, fc = 0;
  GLubyte      *dst[FONT_SIZE];

  printf("Loading font..\n");

  load_xpm(
    "xpm/font/font.xpm", (unsigned char **)&src, &size_x, &size_y);

  for(y = 0; y < FONT_SIZE; y++)
  {
    font_bm[y].size_x = 8;
    font_bm[y].size_y = 16;

    dst[y] = (GLubyte *)alloc_mem(8 * 16 * 4);
  }

  for(fy = 2; fy < 8; fy++)
  {
    soffs = fy * (16 * 128 * 4);

    for(fx = 0; fx < 16; fx++)
    {
      for(y = 0, dyo = 0; y < 16; y++)
      {
        memcpy(dst[fc] + dyo, src + soffs, 8 * 4);

        dyo += 8 * 4;
        soffs += 128 * 4;
      }

      soffs += (8 * 4) - (16 * 128 * 4);
      fc++;
    }
  }

  for(y = 0; y < FONT_SIZE; y++)
  {
    convert_2_tex(dst[y], &font_bm[y], 1);
    free(dst[y]);
  }

  free(src);
}

/***************************************************************************
*
***************************************************************************/
PUBLIC void load_flr_xpms(void)
{
  int  i;
  FILE *fp;
  char str[STR_LEN + 1], fn[STR_LEN + 1];

  printf("Loading floor sprites..\n");

  fp = fopen(INSTALL_DIR "/data/xpm/xpm.i", "r");
  if(fp == NULL)
  {
    perror("load_flr_xpms() ");
    end_game(-1);
  }

  while(!feof(fp))
  {
    if(fgets(str, STR_LEN, fp) == NULL)
      continue;
    if(str[0])
      flr_bm_size++;
  }
  rewind(fp);

  flr_bm = (tflr_bm *)alloc_mem(flr_bm_size * sizeof(tflr_bm));

  for(i = 0; i < flr_bm_size; i++)
  {
    if(fgets(str, STR_LEN, fp) == NULL)
      continue;

    if(sscanf(str, "%s %c", fn, &(flr_bm[i].bound)) != 2)
      continue;

    if(!fn[0])
      continue;

    load_bm(fn, &(flr_bm[i].bm), 1);
  }

  fclose(fp);
}

/***************************************************************************
*
***************************************************************************/
PUBLIC void load_sprite_xpms(void)
{
  printf("Loading sprites..\n");

  load_bm_ani("xpm/standard/droid_ani.xpm", &droid_bm[0], 10, 0);
  load_bm_ani("xpm/standard/paradroid_ani.xpm", &paradroid_bm[0], 10, 0);
  load_bm_ani("xpm/standard/laser_l.xpm", &laser_l_bm[0], 4, 0);
  load_bm_ani("xpm/standard/laser_cb.xpm", &laser_cb_bm[0], 4, 0);
  load_bm_ani("xpm/standard/laser_uv.xpm", &laser_uv_bm[0], 4, 0);
  load_bm_ani("xpm/standard/laser_te.xpm", &laser_te_bm[0], 4, 0);
  load_bm_ani("xpm/standard/explosion.xpm", &explosion_bm[0], 7, 0);
  load_bm_ani("xpm/standard/power_bay.xpm", &power_bay_bm[0], 5, 0);
  load_bm("xpm/standard/ntitle.xpm", &ntitle_bm, 1);
  load_bm_ani("xpm/standard/flr_door_h_cyan_ani.xpm", &doorh_bm[0], 9, 1);
  load_bm_ani("xpm/standard/flr_door_v_cyan_ani.xpm", &doorv_bm[0], 9, 1);
  load_bm_ani("xpm/standard/digits.xpm", &digit_bm[0], 10, 1);
  load_bm_ani("xpm/standard/digits2.xpm", &digit2_bm[0], 10, 1);
  load_bm("xpm/standard/ship_complete.xpm", &ship_complete_bm, 1);
  load_bm("xpm/standard/credit_6.xpm", &credit_6_bm, 0);
  load_bm("xpm/standard/neg.xpm", &neg_bm, 1);
  load_bm("xpm/standard/paused.xpm", &paused_bm, 1);
  load_bm("xpm/standard/trans_terminated.xpm", &trans_terminated_bm, 0);
  load_bm("xpm/standard/docking_to_ship.xpm", &get_ready1_bm, 0);
  load_bm("xpm/standard/intro_back_1.xpm", &intro_back_bm[0], 0);
  load_bm("xpm/standard/intro_back_2.xpm", &intro_back_bm[1], 0);
  load_bm("xpm/standard/intro_back_3.xpm", &intro_back_bm[2], 0);
  load_bm("xpm/standard/intro_back_4.xpm", &intro_back_bm[3], 0);
}
