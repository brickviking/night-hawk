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
* floor object
*
****************************************************************************
****************************************************************************
***************************************************************************/
extern "C" {
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "defs.h"
#include "misc_externs.h"
#include "sprite_loader_externs.h"
#include "opengl_externs.h"
}
#include "tfloor.h"

/***************************************************************************
*
***************************************************************************/
tfloor::tfloor(void)
{
  int x;

  fmap_x_size = fmap_y_size = 0;
  fmap = NULL;
  power_bay = NULL;

  for(x = 0; x < MAX_DOORS; x++)
    door[x] = NULL;

  for(x = 0; x < MAX_TRANSPORTS; x++)
    transport[x].x = -1;

  console.x = -1;
  colour = 1.0;
  down = 0;

  ship_noise_no = -1;
  ship_noise_vol = 0;
  ship_noise_freq = 0;
}

/***************************************************************************
*
***************************************************************************/
void tfloor::create(int x_size, int y_size)
{
  fmap_x_size = x_size;
  fmap_y_size = y_size;
  fmap = (int *)alloc_mem(fmap_x_size * fmap_y_size * sizeof(int));
}

/***************************************************************************
* I had to split up the load() method to accomodate Eric's request to get
* ned working again. This method will be called by ned. JN 27SEP20
***************************************************************************/
void tfloor::load_floor_map(char *fn)
{
	FILE *fp;
	char str[STR_LEN + 1];
	int  sx, sy, x, y;

	snprintf(str, STR_LEN, "%s.f", fn);
	if (verbose_logging == TRUE)  // JN, 23AUG20
		printf("Loading floor map: %s\n", str);
	fp = fopen(str, "r");
	if (fp == NULL)
		print_error(__func__, "fopen(map.f)");

	if (fgets(str, STR_LEN, fp) == NULL)
		print_error(__func__, "fgets(). Expecting floor map dimensions");

	if (sscanf(str, "%d %d", &sx, &sy) != 2)
		print_error(__func__, "sscanf(1). Garbage found in floor map");

	create(sx, sy);
	for (y = 0; y < fmap_y_size; y++) {
		for(x = 0; x < fmap_x_size; x++) {
			if (fgets(str, STR_LEN, fp) == NULL)
				print_error(__func__, "fgets(). Error parsing floor map");
			if (sscanf(str, "%d", (fmap + (y * fmap_x_size) + x)) != 1)
				print_error(__func__, "sscanf(2). Garbage found in floor map");
    		}
	}

	fclose(fp);
}

/***************************************************************************
* Ditto. JN 27SEP20
***************************************************************************/
void tfloor::load_floor_misc(char *fn)
{
	FILE *fp;
	char str[STR_LEN + 1];
	int  sx, sy, door_ptr = 0, transport_ptr = 0;

	snprintf(str, STR_LEN, "%s.m", fn);
	if (verbose_logging == TRUE)  // JN, 23AUG20
		printf("Loading floor misc: %s\n", str);
	fp = fopen(str, "r");
	if (fp == NULL)
		print_error(__func__, "fopen(map.m)");

	while (!feof(fp)) {
		char amble[STR_LABEL_LEN];

		if (fgets(str, STR_LEN, fp) == NULL)
			continue;
		if (sscanf(str, "%s", amble) != 1)
			continue;

		if (!strcmp(amble, "door:")) {
			if (door_ptr < MAX_DOORS) {
				char t;

				if (sscanf(str, "%*s %d %d %c", &sx, &sy, &t) != 3)
					print_error(__func__, "sscanf(3). Garbage found parsing door");

				if (verbose_logging == TRUE)
					printf("new() door (%d)\n", door_ptr);
				door[door_ptr] = new(tdoor);
				if (door[door_ptr] == NULL)
					print_new_error(__func__);
				door[door_ptr]->init(sx, sy, t ==
						'h' ? DOOR_HORIZONTAL : DOOR_VERTICAL);
				door_ptr++;
			}
		} else if (!strcmp(amble, "power_bay:")) {
			if (power_bay == NULL) {
				if (sscanf(str, "%*s %d %d",&sx, &sy) != 2)
					print_error(__func__, "sscanf(4). Garbage found parsing power bay");

				if (verbose_logging == TRUE)
					printf("new() powerbay\n");
				power_bay = new(tpower_bay);
				if (power_bay == NULL)
					print_new_error(__func__);
				power_bay->init(sx, sy);
			}
    		} else if (!strcmp(amble, "transport:")) {
			if (transport_ptr < MAX_TRANSPORTS) {
				if (sscanf(str, "%*s %d %d",
						&transport[transport_ptr].x,
						&transport[transport_ptr].y) != 2)
					print_error(__func__, "sscanf(5). Garbage found parsing transport");
				transport_ptr++;
                        }
		} else if (!strcmp(amble, "console:")) {
			if (sscanf(str, "%*s %d %d", &console.x, &console.y) != 2)
				print_error(__func__, "sscanf(6). Garbage found parsing console");
		} else if (!strcmp(amble, "noise:")) {
			if (sscanf(str, "%*s %d %d %d", &ship_noise_no, &ship_noise_vol,
								&ship_noise_freq) != 3)
				print_error(__func__, "sscanf(7). Garbage found parsing noise");
		}
	}

	fclose(fp);
}

/***************************************************************************
* Quite a bit of work to this method. Reformatted, Made memory handling
* safer. JN, 28AUG20
***************************************************************************/
void tfloor::load(char *fn, char *fname)
{
	map_filename = fn;
	strncpy2(name, fname, STR_LABEL_LEN); // Safer string copy. JN, 23AUG20
	load_floor_map(fn);
	load_floor_misc(fn);
}

/***************************************************************************
*
***************************************************************************/
void tfloor::set_pos(int x, int y)
{
  spos_x = x;
  spos_y = y;
}

/***************************************************************************
*
***************************************************************************/
void tfloor::reinit(void)
{
  int x;

  if(colour != 1.0)
    colour = 0.5;

  for(x = 0; x < MAX_DOORS; x++)
  {
    if(door[x] != NULL)
    {
      door[x]->state = 0;
      door[x]->anim_ptr = 0;
    }
    else
      break;
  }
}

/***************************************************************************
*
***************************************************************************/
/*
 * This method needed a bit of love. Optimised a bit better as it's a
 * fairly intensive method. JN, 31AUG20
 */
void tfloor::draw(void)
{
	int	cnr_x, cnr_y,
		ipx_reset, ipy,
		adj_x, adj_y,
		yp, mapy_len,
		*fmap_start;

	cnr_x = spos_x - SCREEN_HSIZE_X;
	cnr_y = spos_y - SCREEN_HSIZE_Y;
	ipx_reset = (cnr_x / SPRITE_SIZE_X) - 1;
	ipy = (cnr_y / SPRITE_SIZE_Y) - 1;
	adj_x = (cnr_x % SPRITE_SIZE_X) + SPRITE_SIZE_X;
	adj_y = (cnr_y % SPRITE_SIZE_Y) + SPRITE_SIZE_Y;

	glColor3f(colour, colour, colour);

	fmap_start = fmap + (ipy * fmap_x_size);
	mapy_len = ipy + ((SCREEN_SIZE_Y / SPRITE_SIZE_Y) + 3);
	for (yp = -adj_y; ipy < mapy_len; yp += SPRITE_SIZE_Y, ipy++, fmap_start += fmap_x_size) {
		if (ipy >= 0 && ipy < fmap_y_size) {
			int mapx_len, xp, ipx;

			mapx_len = ipx_reset + ((SCREEN_SIZE_X / SPRITE_SIZE_X) + 3);
			for (xp = -adj_x, ipx = ipx_reset; ipx < mapx_len; xp += SPRITE_SIZE_X, ipx++) {
				if (ipx >= 0 && ipx < fmap_x_size)
					blit_bm(&flr_bm[*(fmap_start + ipx)].bm, xp, yp);
			}
		}
	}

	if (power_bay != NULL)
		power_bay->draw(spos_x, spos_y);

	for (int i = 0; i < MAX_DOORS; i++) {
		if (door[i] == NULL)
			break;
		door[i]->draw(spos_x, spos_y);
	}
}

/***************************************************************************
*
***************************************************************************/
void tfloor::bg_calc(void)
{
  int x;

  if(down)
  {
    colour -= 0.01;  // JN, 03SEP20
    if(colour < 0.5)
    {
      colour = 0.5;
      down++;  // Slight tweak for tship::level_bg_calc(). JN, 18SEP20
    }
  }

  if(power_bay != NULL)
    power_bay->bg_calc();

  for(x = 0; x < MAX_DOORS; x++)
    if(door[x] != NULL)
      door[x]->bg_calc();
    else
      break;
}

/***************************************************************************
*
***************************************************************************/
void tfloor::unload(void)
{
  int x;

  if(fmap != NULL)
    free(fmap);

  if(verbose_logging == TRUE)
    printf("delete() powerbay\n");
  if(power_bay != NULL)
    delete(power_bay);

  for(x = 0; x < MAX_DOORS; x++)
    if(door[x] != NULL) {
      if(verbose_logging == TRUE)
        printf("delete() door (%d)\n", x);
      delete(door[x]);
    }
}
