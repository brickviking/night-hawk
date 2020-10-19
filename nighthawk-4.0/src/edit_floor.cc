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
* floor editor object. Restored for 4.x. JN, 27SEP20
*
****************************************************************************
****************************************************************************
***************************************************************************/
extern "C" {
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "defs.h"
#include "misc_externs.h"
#include "sprite_loader_externs.h"
#include "opengl_externs.h"
}
#include "tedit_floor.h"
#include "ned_externs.h"

/***************************************************************************
*
***************************************************************************/
tedit_floor::tedit_floor(void)
{
	spos_x = 0.0;
	spos_y = 0.0;
	blank = 0;
	sflashing_alpha = flashing_alpha = 0.0;
}

void tedit_floor::snap_shot(void)
{
	sflashing_alpha = flashing_alpha;
}

void tedit_floor::create(int x_size, int y_size)
{
	int x, y;

	tfloor::create(x_size, y_size);

	for (y = 0; y < fmap_y_size; y++)
		for (x = 0; x < fmap_x_size; x++)
			*(fmap + (y * fmap_x_size) + x) = default_map_value;
	sprite_sel_ptr = 0;
}

void tedit_floor::save(void)
{
	FILE *fp;
	int x, y;
	char str[STR_LEN + 1];

	snprintf(str, STR_LEN, "%s.f", floor_relpath);
	printf("Saving floor: %s\n", str);
	fp = fopen(rel_to_abs_filepath(str), "w");
	if(fp == NULL)
		print_error(__func__, "fopen(map.f)");

	if (!fprintf(fp, "%d %d\n", fmap_x_size, fmap_y_size))
		print_error(__func__, "fprintf(1)");

	for (y = 0; y < fmap_y_size; y++)
		for (x = 0; x < fmap_x_size; x++)
			if (!fprintf(fp,"%d\n", *(fmap + (y * fmap_x_size) + x)))
				print_error(__func__, "fprintf(2)");
	fclose(fp);
}

void tedit_floor::process_user_inputs(void)
{
	int ipx,ipy;

	for (;;) {
		tkevent k;

		if (!get_kb_event(&k))
			break;

		switch(k.type) {
			case KEYBOARD_EVENT:
			switch (tolower(k.val)) {
				case KEY_SELECT:
				case 13: /*enter key*/
				ipx = spos_x / SPRITE_SIZE_X;
				ipy = spos_y / SPRITE_SIZE_Y;
				if((ipx >= 0) && (ipx < fmap_x_size) &&
						(ipy >= 0) && (ipy < fmap_y_size))
					*(fmap + (ipy * fmap_x_size) + ipx) = sprite_sel_ptr;
				break;

				case 's':
				save();
				break;

				case 'b':
				blank ^= 1;
				break;

				case 'q':
				end_game(0);
			}
			break;

			case SPECIAL_EVENT:
			switch(k.val) {
				case GLUT_KEY_UP:
				spos_y -= SPRITE_SIZE_Y;
				break;

				case GLUT_KEY_DOWN:
				spos_y += SPRITE_SIZE_Y;
				break;

				case GLUT_KEY_LEFT:
				spos_x -= SPRITE_SIZE_X;
				break;

				case GLUT_KEY_RIGHT:
				spos_x += SPRITE_SIZE_X;
				break;

				case GLUT_KEY_PAGE_UP:
				if( sprite_sel_ptr > 0)
					sprite_sel_ptr--;
				break;

				case GLUT_KEY_PAGE_DOWN:
				if (sprite_sel_ptr < (flr_bm_size - 1))
					sprite_sel_ptr++;
				break;
			}
			break;
		}
	}
}

void tedit_floor::draw(void)
{
	int ipx, ipy, ssp_block;
	float alpha_pulse;

	snap_shot();
	tfloor::draw();
	process_user_inputs();
	if (blank)
		return;

	alpha_pulse = 0.5 * fabsf(sinf(sflashing_alpha));
	alpha_pulse += 0.5;

	glColor3f(1.0, 1.0, 1.0);
	print_str(ship_start, 2, 2);
	print_str(floor_name, 2, 20);

	glLineWidth(3.0);

	ipx = spos_x / SPRITE_SIZE_X;
	ipy = spos_y / SPRITE_SIZE_Y;
	if ((ipx >= 0) && (ipx < fmap_x_size) && (ipy >= 0) && (ipy < fmap_y_size)) {
		char str[STR_LEN + 1];

		glColor4f(1.0, 1.0, 1.0, alpha_pulse);
		blit_rect_hollow(
			SCREEN_HSIZE_X - (spos_x % SPRITE_SIZE_X),
			SCREEN_HSIZE_Y - (spos_y % SPRITE_SIZE_Y),
			SPRITE_SIZE_X,
			SPRITE_SIZE_Y);

		glColor3f(1.0, 1.0, 1.0);
		snprintf(str, STR_LEN, "%d,%d",
			spos_x + SPRITE_HSIZE_X,
			spos_y + SPRITE_HSIZE_Y);
		print_str(str, 2, 38);
	}

	glColor3fv(black_pixel[0]);
	blit_rect(
		SCREEN_SIZE_X - SPRITE_SIZE_X - 6,
		10 - 4,
		SPRITE_SIZE_X + 6,
		(4 * (SPRITE_SIZE_Y + 10)));

	ssp_block = sprite_sel_ptr & ~0x3;
	for (ipy = 0; ipy < 4; ipy++) {
		if ((ipy + ssp_block) >= flr_bm_size)
			continue;

		glColor4f(1.0, 1.0, 1.0, 1.0);
		blit_bm(&flr_bm[ipy + ssp_block].bm,
			SCREEN_SIZE_X - 34,
			(ipy * (SPRITE_SIZE_Y + 10)) + 10);

		if ((sprite_sel_ptr & 0x3) == ipy)
			glColor4f(1.0, 1.0, 1.0, alpha_pulse);
		else
			glColor3fv(black_pixel[0]);

		blit_rect_hollow(
			SCREEN_SIZE_X - SPRITE_SIZE_X - 4,
			(ipy * (SPRITE_SIZE_Y + 10)) + 10 - 2,
			SPRITE_SIZE_X + 4,
			SPRITE_SIZE_Y + 4);
	}
}

void tedit_floor::bg_calc(void)
{
	flashing_alpha += DEG2RAD(9.0);
	flashing_alpha = fmodf(flashing_alpha, 2.0 * M_PI);
}
