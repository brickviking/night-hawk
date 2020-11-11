/***************************************************************************
****************************************************************************
****************************************************************************
*
* Nighthawk Copyright (C) 1997 Jason Nunn
* Nighthawk is under the GNU General Public License.
*
* Sourceforge admin: Eric Gillespie
* night-hawk.sourceforge.net
*
* See README file for a list of contributors.
*
* ----------------------------------------------------------------
* sprite_loader.c - handles loading the sprites.
*
****************************************************************************
****************************************************************************
***************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <GL/glut.h>
#include <GL/glx.h>
#include "defs.h"
#include "misc_externs.h"
#include "xpm_loader_externs.h"
#include "png_loader_externs.h"
#include "opengl_externs.h"

tflr_bm     *flr_bm;
int         flr_bm_size;

/*
 * Not the best to have hard coded arrays. It's bug free so we'll let
 * sleeping dogs lay. JN,28AUG20
 */
bm_t        droid_bm[10],
            paradroid_bm[10],
            laser_l_bm,
            laser_cb_bm,
            laser_uv_bm,
            laser_te_bm,
            explosion_bm[7],
            power_bay_bm[5],
            ntitle_bm,
            doorh_bm[9],
            doorv_bm[9],
            digit_bm[10],
            ship_complete_bm,
            credit_6_bm,
            neg_bm,
            trans_terminated_bm,
            get_ready1_bm,
            intro_back_bm[4];

bm_t        font_bm[FONT_SIZE];

/*
 * this is for the opengl alignment to actual bm size so that
 * ship.cc can properly align them on the screen
 */
int         ship_bm_offs_x[] =
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
void load_bm(char *fn, bm_t *bm, int aligned_f)
{
	GLubyte *buf;

	load_xpm(fn, (unsigned char **)&buf, &(bm->size_x), &(bm->size_y));
	convert_2_tex(buf, bm, aligned_f);
	free(buf);
}

/*
 * New for 4.0. xpm loader has bugs in it I can't be bothered fixing. Time
 * to png. Note: PNG loads are word aligned. JN, 07OCT20.
 */
void load_bm_png(char *fn, bm_t *bm)
{
	GLubyte *buf;

	load_png(fn, (void **)&buf, &(bm->size_x), &(bm->size_y));
	convert_2_tex(buf, bm, TRUE);
	free(buf);
}

/***************************************************************************
*
***************************************************************************/
void load_bm_ani(char *fn, bm_t *bm, unsigned int len, int aligned_f)
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
* Doubled font resolution from V3.0 to V4.0.
* This function is a bit of a dog's dinner of a encoder/mapper, but it gets
* the job done. No need to be elegant. JN, 30AUG20
*
* From gprof analysis. Optimised tship::print_score() and print_str_len()
* JN, 09OCT20
***************************************************************************/
void load_font(void)
{
  GLubyte      *src;
  unsigned int size_x, size_y, fx, fy, y, soffs, dyo, fc = 0;
  GLubyte      *dst[FONT_SIZE];

  if(verbose_logging == TRUE)
    printf("Loading font..\n");

  load_xpm(
    "xpm/font/font.xpm", (unsigned char **)&src, &size_x, &size_y);

  for(y = 0; y < FONT_SIZE; y++)
  {
    font_bm[y].size_x = FONT_SIZE_X;
    font_bm[y].size_y = FONT_SIZE_Y;

    dst[y] = (GLubyte *)alloc_mem(FONT_SIZE_X * FONT_SIZE_Y * 4);
  }

  for(fy = 0; fy < FONT_TABLE_Y; fy++)
  {
    soffs = fy * (FONT_TABLE_X * FONT_SIZE_X * FONT_SIZE_Y * 4);

    for(fx = 0; fx < FONT_TABLE_X; fx++)
    {
      for(y = 0, dyo = 0; y < FONT_SIZE_Y; y++)
      {
        memcpy(dst[fc] + dyo, src + soffs, FONT_SIZE_X * 4);

        dyo += FONT_SIZE_X * 4;
        soffs += FONT_SIZE_X * FONT_TABLE_X * 4;
      }

      soffs += (FONT_SIZE_X * 4) - (FONT_TABLE_X * FONT_SIZE_X * FONT_SIZE_Y * 4);
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
* Reworked function to get rid of rewind(). What an arse of a hack this
* function was. JN, 28AUG20
***************************************************************************/
void load_flr_xpms(void)
{
	FILE *fp;

	if (verbose_logging == TRUE)
		printf("Loading floor sprites..\n");

	fp = fopen(rel_to_abs_filepath("/xpm/xpm.i"), "r"); /*JN, 06SEP20*/
	if (fp == NULL)
		print_error(__func__, "fopen()");

	/*
	 * Parse xpm.i file and load sprites into a table called
	 * *flr_bm. Before we read ahead in the xpm.i and counted up the
	 * sprite entries and did a malloc(). This code dynamically
	 * increases the size of the table using realloc() for every new
	 *  entry is parses (the right way to do it). JN, 28AUG20
	 */
	while (!feof(fp)) {
		char str[STR_LEN + 1], fn[STR_LEN + 1];
		int i;
		char bound;

		if (fgets(str, STR_LEN, fp) == NULL)
			break;	/*EOF.. break out of loop*/

		if (sscanf(str, "%s %c", fn, &bound) != 2) {
			if (verbose_logging == TRUE) /*JN, 27OCT20*/
				printf("Warning: Garbage found 'in xpm.i file. Ignoring.\n");
			continue;
		}

		i = flr_bm_size;
		flr_bm_size++;
		flr_bm = (tflr_bm *)realloc_mem(
				flr_bm,
				flr_bm_size * sizeof(tflr_bm));
		flr_bm[i].bound = bound;
		load_bm(fn, &(flr_bm[i].bm), 1);
	}

  fclose(fp);
}

/***************************************************************************
*
***************************************************************************/
void load_sprite_xpms(void)
{
  if(verbose_logging == TRUE)
    printf("Loading sprites..\n");

  /*
   * Standard pics.
   * Laser are single sprites now, but will leave the original xpm files alone.
   * JN, 02OCT20
   *
   * Some pics converted to PNG. JN, 07OCT20
   */
  load_bm_ani("xpm/standard/droid_ani.xpm", &droid_bm[0], 10, 0);
  load_bm_ani("xpm/standard/paradroid_ani.xpm", &paradroid_bm[0], 10, 0);
  load_bm("xpm/standard/laser_l.xpm", &laser_l_bm, 0);
  load_bm("xpm/standard/laser_cb.xpm", &laser_cb_bm, 0);
  load_bm("xpm/standard/laser_uv.xpm", &laser_uv_bm, 0);
  load_bm("xpm/standard/laser_te.xpm", &laser_te_bm, 0);
  load_bm_ani("xpm/standard/explosion.xpm", &explosion_bm[0], 7, 0);
  load_bm_ani("xpm/standard/power_bay.xpm", &power_bay_bm[0], 5, 0);
  load_bm_png("xpm/standard/ntitle.png", &ntitle_bm);
  load_bm_ani("xpm/standard/flr_door_h_cyan_ani.xpm", &doorh_bm[0], 9, 1);
  load_bm_ani("xpm/standard/flr_door_v_cyan_ani.xpm", &doorv_bm[0], 9, 1);
  load_bm_ani("xpm/standard/digits.xpm", &digit_bm[0], 10, 1);
  load_bm_png("xpm/standard/ship_complete.png", &ship_complete_bm);
  load_bm_png("xpm/standard/credit_6.png", &credit_6_bm);
  load_bm("xpm/standard/neg.xpm", &neg_bm, 1);
  load_bm_png("xpm/standard/trans_terminated.png", &trans_terminated_bm);
  load_bm_png("xpm/standard/docking_to_ship.png", &get_ready1_bm);
  load_bm_png("xpm/standard/intro_back_1.png", &intro_back_bm[0]);
  load_bm_png("xpm/standard/intro_back_2.png", &intro_back_bm[1]);
  load_bm_png("xpm/standard/intro_back_3.png", &intro_back_bm[2]);
  load_bm_png("xpm/standard/intro_back_4.png", &intro_back_bm[3]);
}

/***************************************************************************
* New for 4.0. Load fleet logos (this has to be run post OpenGL init.
* JN, 02OCT20.
***************************************************************************/
void load_fleet_logos(void)
{
	if (verbose_logging == TRUE)
		printf("Loading fleet logos..\n");

	for (int i = 0; i < fleet_table_size; i++) {
		char str[STR_LEN + 1];

		snprintf(str, STR_LEN, "/%s/logo.png", fleet_table[i].name);
		load_bm_png(str, &fleet_table[i].logo);
	}
}
