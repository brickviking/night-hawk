/****************************************************************************
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
* png_loader.c - PNG loader (libpng 1.5.x)
*
* Code taken Sailplay R11. (C) 2009 Jason Nunn.
* https://sourceforge.net/projects/sailplay
*
* Code based on example from:
* http://www.libpng.org/pub/png/book/chapter13.html and
* https://fossies.org/linux/libpng/example.c
*
* Nb/ This code scales width of bitmap to so that it is a divisible by
* 4 (a long word). Therefore, align_f = 1 can be set when calling
* convert_2_tex(). JN, 08OCT20
*
***************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <png.h>
#include "defs.h"
#include "misc_externs.h"

/****************************************************************************
*
****************************************************************************/
static int is_png_file(FILE *fp)
{
	unsigned char header[8];

	if(fread(header, 8, 1, fp) != 1)
		return 0;
	rewind(fp);
	return !png_sig_cmp(header, (png_size_t)0, 8);
}

/****************************************************************************
*
****************************************************************************/
void load_png(char *fn, void **buf, unsigned int *sx, unsigned int *sy)
{
	FILE		*fp;
	char		str[STR_LEN + 1];
	png_structp	png_ptr;
	png_infop	info_ptr;
	png_uint_32	width, height;
	int		bit_depth, color_type, interlace_type;
	png_bytep	*row_pointers;
	int		x, y;
	float		rx, rx_inc;
	png_byte	*p;
	int		bytes_per_pixel;

	/*
	 * Load PNG
	 * This library is a bit of a headache to use. JN, 07OCT20
	 */
	if(verbose_logging == TRUE)
		printf("Loading png: %s\n", fn);

	snprintf(str, STR_LEN, "/%s", fn);
	fp = fopen(rel_to_abs_filepath(str), "r");
	if(!fp)
		print_error(__func__, "fopen()");

	if(!is_png_file(fp))
		printf_error("%s:is_png_file() - Not a PNG file", __func__);

	png_ptr = png_create_read_struct(
				PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!png_ptr)
		printf_error("%s:png_create_read_struct()", __func__);

	info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr)
		printf_error("%s:png_create_info_struct()", __func__);

	if (setjmp(png_jmpbuf(png_ptr)))
		printf_error("%s:setjmp()", __func__);

	png_init_io(png_ptr, fp);
	png_read_info(png_ptr, info_ptr);
	png_get_IHDR(png_ptr, info_ptr,
		&width,
		&height,
		&bit_depth,
		&color_type,
		&interlace_type,
		NULL, NULL);

	if(interlace_type != PNG_INTERLACE_NONE)
		printf_error("%s- PNG file is interlaced. Can't handle !", __func__);

	/*
	 * Old MesaGL implementation requires rows to be word aligned.
	 */
	*sx = (width >> 2) << 2;
	if (*sx < width)
		*sx += 4;
	*sy = height;

#ifdef PNG_READ_SCALE_16_TO_8_SUPPORTED
	png_set_scale_16(png_ptr);
#else
	png_set_strip_16(png_ptr);
#endif
	png_set_packing(png_ptr);
	png_set_packswap(png_ptr);
	if (color_type == PNG_COLOR_TYPE_PALETTE)
		png_set_palette_to_rgb(png_ptr);
	if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
		png_set_expand_gray_1_2_4_to_8(png_ptr);
	if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS) != 0)
		png_set_tRNS_to_alpha(png_ptr);

	png_read_update_info(png_ptr, info_ptr);

	row_pointers = (png_bytep*)alloc_mem(sizeof(png_bytep) * *sy);
	for(y = 0; y < *sy; y++)
		row_pointers[y] = (png_byte *)alloc_mem(
				png_get_rowbytes(png_ptr, info_ptr));
	png_read_image(png_ptr, row_pointers);

	/*
	 * Convert (scale row to long align width - MesaGL requirement)
	 */
	bytes_per_pixel = png_get_rowbytes(png_ptr, info_ptr) / width;
	if (!(bytes_per_pixel >= 2 && bytes_per_pixel <= 4))
		printf_error("PNG file has unknown pixel format");
	*buf = alloc_mem(*sx * *sy * 4);
	p = (png_byte *)*buf;
	rx_inc = (float)width / (float)*sx;
	if (verbose_logging == TRUE)
		if (rx_inc != 1.0)
			printf("PNG file not long aligned. Scaling.\n");
	for(y = 0; y < *sy; y++) {
		png_byte *row = row_pointers[y];
		rx = 0.0f;
		for(x = 0; x < *sx; x++) {
			switch (bytes_per_pixel) {
				case 2: /*Grey alpha to RGBA copy*/
				p[R] = p[G] = p[B] = row[2 * (int)rintf(rx)];
				p[A] = 0xff;
				break;

				case 3: /*RBA to RGBA copy*/
				memcpy(p, &row[3 * (int)rintf(rx)], 3);
				p[A] = 0xff;
				break;

				case 4: /*RGBA copy*/
				memcpy(p, &row[4 * (int)rintf(rx)], 4);
				break;
			}

			p += 4;
			rx += rx_inc;
		}
	}

	/*
	 * cleanup
	 */
	png_read_end(png_ptr, info_ptr);
        fclose(fp);

	for (y = 0; y < *sy; y++)
		free(row_pointers[y]);
	free(row_pointers);

	png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
}
