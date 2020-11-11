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
* ship object
*
****************************************************************************
****************************************************************************
***************************************************************************/
extern "C" {
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "math.h"
#include "defs.h"
#include "linked_data_externs.h"
#include "misc_externs.h"
#include "sprite_loader_externs.h"
#include "opengl_externs.h"
#include "scores_externs.h"
}
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
  ship_end_cd = 100;  // Reverted 2.2. JN, 04SEP20
  state = STATE_GET_READY;
  get_ready_count = 0;
  get_ready_siren_c = 0;  // JN, 04SEP20
  get_ready_siren_w = 0;
  s_transport_spin = transport_spin = 0;  // JN, 04SEP20
  s_screen_flash_fade = screen_flash_fade = 0; // New. JN, 09SEP20
}

/***************************************************************************
* New. Light weight inter process communication mechanism.
* Woops. This object is meant to have one. JN, 18SEP20
*
* Nb/ any variables with in objects with a s_ or ss_ amble means it's a
* snapshot variable
***************************************************************************/
void tship::snap_shot(void)
{
	s_transport_spin = transport_spin;
	s_screen_flash_fade = screen_flash_fade;
}

/***************************************************************************
* Floor methods.
*
* Converted methods from a recursive linked list to an iterative linked list
* design to improve code safety. Linked list is FIFO. JN, 30AUG20
***************************************************************************/
void tship::create_floor(char *fname)
{
	tfloor_list **p;
	char str[STR_LEN + 1];

	// Find end of list.
	p = &floor_list;
	while (*p != NULL)
		p = &((*p)->next);

	// Append entry to end of list.
	*p = (tfloor_list *)alloc_mem(sizeof(tfloor_list));
	(*p)->next = NULL;

	if (verbose_logging == TRUE)
		printf("new() floor (%s)\n", fname);
	(*p)->curr = new tfloor();
	if((*p)->curr == NULL)
		print_new_error(__func__);

	snprintf(str, STR_LEN, "/%s/%s", name, fname); // JN, 06SEP20
	(*p)->curr->load(rel_to_abs_filepath_fleet(str), fname);
}

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

void tship::free_all_floor(void)
{
	tfloor_list **p;

	p = &floor_list;
	while (*p != NULL) {
		tfloor_list *tmp;

		tmp = (*p)->next;
		(*p)->curr->unload();

		if (verbose_logging == TRUE)
			printf("delete() floor (%s)\n", (*p)->curr->name);
		delete((*p)->curr);
		free(*p);
		*p = tmp;
	}
}

/***************************************************************************
* Transport methods.
*
* Converted methods from a recursive linked list to an iterative linked list
* design to improve code safety. Linked list is FIFO. JN, 30AUG20
***************************************************************************/
void tship::create_transport(char *fn, int px, int py, int fx, int fy, int tn)
{
	ttransport_list **p;

	// Find end of list.
	p = &transport_list;
	while (*p != NULL)
		p = &((*p)->next);

	// Append entry and save record on end of linked list.
	*p = (ttransport_list *)alloc_mem(sizeof(ttransport_list));
	(*p)->floor = find_floor(fn);
	(*p)->diagram_px = px;
	(*p)->diagram_py = py;
	(*p)->floor_pos_x = fx;
	(*p)->floor_pos_y = fy;
	(*p)->no = tn;
	(*p)->next = NULL;
}

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

void tship::free_all_transport(void)
{
	ttransport_list **p;

	p = &transport_list;	// This C programming is coming back to me
	while (*p != NULL) {	// Now :). JN, 30AUG20
		ttransport_list *tmp;

		tmp = (*p)->next;
		free(*p);
		*p = tmp;
	}
}

/***************************************************************************
* Updated method for OpenAL. JN, 17SEP20
*
* The *.m files in the ship data have Funktracker volume and frequency
* settings (Funktracker was a sound system in v2.x). The hardcoded
* Funktracker vol/freq in this game have been converted to OpenAL scalings.
* But I won't touch the game data. I'll just convert them. It's a bit hacky,
* but oh well. JN, 17SEP20
***************************************************************************/
void tship::set_ship_noise(void)
{
	float v, f;

	if(curr_location->floor->ship_noise_no == -1)
		return;

	v = ((float)curr_location->floor->ship_noise_vol) / 1024.0;
	f = ((float)(curr_location->floor->ship_noise_freq << 8)) / 11025.0;

	sound_cmd_snoise(
		curr_location->floor->ship_noise_no + FX_SHIP_NOISE_START,
		v, f);
}

/***************************************************************************
* This method reworked to make it memory safe. JN, 28AUG20
***************************************************************************/
void tship::load(const char *shipname)
{
	FILE *fp;
	char	str[STR_LEN + 1],
		amble[STR_LABEL_LEN + 1],
		floor_n[STR_LABEL_LEN + 1];
	int	droid_ptr = 1;

	if (verbose_logging == TRUE)  // JN, 23AUG20
		printf("Loading ship: %s\n", shipname);

	strncpy2(name, (char *)shipname, STR_LABEL_LEN);
	snprintf(str, STR_LEN, "/%s/chars", name); // JN, 06SEP20
	fp = fopen(rel_to_abs_filepath_fleet(str), "r");
	if (fp == NULL)
		print_error(__func__, "fopen(chars)");

	while(!feof(fp)) {
		if (fgets(str, STR_LEN, fp) == NULL)
			break;

		if (sscanf(str, "%s", amble) != 1)
			continue;

		if (!strcmp(amble, "type:")) {
			strncpy2(type, str + 6, STR_LABEL_LEN); // JN, 28AUG20
			strip_leading_whitespace(type); // JN, 28AUG20

		} else if (!strcmp(amble, "floor:")) {
			if (sscanf(str, "%*s %s", floor_n) != 1)
				print_error(__func__, "sscanf(1). Garbage found parsing type");
			create_floor(floor_n);
		} else if (!strcmp(amble, "transport:")) {
			int px, py, fx, fy, tn;

			if (sscanf(str, "%*s %s %d %d %d %d %d",
					floor_n, &px, &py, &fx, &fy, &tn) != 6)
				print_error(__func__, "sscanf(2). Garbage found parsing transport");
			create_transport(floor_n, px, py, fx, fy, tn);
		} else if (!strcmp(amble, "init:")) {
			int x, y;

			if (sscanf(str, "%*s %s %d %d", floor_n, &x, &y) != 3)
				print_error(__func__, "sscanf(3). Garbage found parsing init");

			curr_location = find_transport(floor_n, x, y);
			if (curr_location != NULL) {
				if (verbose_logging == TRUE)
					printf("new() paradroid (002)\n");
				droids[0] = new(tparadroid);
				if (droids[0] == NULL)
					print_new_error(__func__);

				// Load the little guy, 002 :), JN, 23AUG20
				//
				((tparadroid *)droids[0])->init(
					curr_location->floor,(tentity **)&droids, x, y);
			}
		} else if (!strcmp(amble, "droid:")) {
			int x, y;
			tfloor *f;

			if (droid_ptr == MAX_DROIDS) {
				if (verbose_logging == TRUE)
					printf("Warning: droid_ptr == MAX_DROIDS. Ignored droid.\n");
				continue; // JN, 28AUG20
			}

			if (sscanf(str, "%*s %s %s %d %d", amble, floor_n, &x, &y) != 4)
				print_error(__func__, "sscanf(4). Garbage found parsing droid");

			f = find_floor(floor_n);
			if (f == NULL)
				print_error(__func__, "find_floor(). Unable to find floor");

			if (verbose_logging == TRUE)
				printf("new() droid (%s)\n", amble);
			if (!strcmp(amble, "108"))
				droids[droid_ptr] = new(tdroid_108);
			else if (!strcmp(amble, "176"))
				droids[droid_ptr] = new(tdroid_176);
			else if (!strcmp(amble, "261"))
				droids[droid_ptr] = new(tdroid_261);
			else if (!strcmp(amble, "275"))
				droids[droid_ptr] = new(tdroid_275);
			else if (!strcmp(amble, "355"))
				droids[droid_ptr] = new(tdroid_355);
			else if (!strcmp(amble, "368"))
				droids[droid_ptr] = new(tdroid_368);
			else if (!strcmp(amble, "423"))
				droids[droid_ptr] = new(tdroid_423);
			else if (!strcmp(amble, "467"))
				droids[droid_ptr] = new(tdroid_467);
			else if (!strcmp(amble, "489"))
				droids[droid_ptr] = new(tdroid_489);
			else if (!strcmp(amble, "513"))
				droids[droid_ptr] = new(tdroid_513);
			else if (!strcmp(amble, "520"))
				droids[droid_ptr] = new(tdroid_520);
			else if (!strcmp(amble, "599"))
				droids[droid_ptr] = new(tdroid_599);
			else if (!strcmp(amble, "606"))
				droids[droid_ptr] = new(tdroid_606);
			else if (!strcmp(amble, "691"))
				droids[droid_ptr] = new(tdroid_691);
			else if (!strcmp(amble, "693"))
				droids[droid_ptr] = new(tdroid_693);
			else if (!strcmp(amble, "719"))
				droids[droid_ptr] = new(tdroid_719);
			else if (!strcmp(amble, "720"))
				droids[droid_ptr] = new(tdroid_720);
			else if (!strcmp(amble, "766"))
				droids[droid_ptr] = new(tdroid_766);
			else if (!strcmp(amble, "799"))
				droids[droid_ptr] = new(tdroid_799);
			else if (!strcmp(amble, "805"))
				droids[droid_ptr] = new(tdroid_805);
			else if (!strcmp(amble, "810"))
				droids[droid_ptr] = new(tdroid_810);
			else if (!strcmp(amble, "820"))
				droids[droid_ptr] = new(tdroid_820);
			else if (!strcmp(amble, "899"))
				droids[droid_ptr] = new(tdroid_899);
			else if (!strcmp(amble, "933"))
				droids[droid_ptr] = new(tdroid_933);
			else if (!strcmp(amble, "949"))
				droids[droid_ptr] = new(tdroid_949);
			else if (!strcmp(amble, "999"))
				droids[droid_ptr] = new(tdroid_999);
			else
				printf_error("Unknown droid: %s. Ignoring.", amble);

			if (droids[droid_ptr] == NULL)
				print_new_error(__func__);
			droids[droid_ptr]->init(f, (tentity **)&droids, x, y);

			/*
			 * Parse droid commands (movements) for droids that are not random.
			 */
			while (!feof(fp)) {
				long lpos;

				lpos = ftell(fp);
				if (fgets(str, STR_LEN, fp) == NULL)
					continue;

				if (str[0] == '*') {
					char c;

					if (sscanf(str, "%*c %s %d %d", &c, &x, &y) != 3)
						print_error(__func__, "sscanf(4). Garbage found parsing droid command");

					/*
					 * Saves data as a linked list.
					 */
					((tdroid_108 *)droids[droid_ptr])->append_cmd(c, x, y);
				} else { // Detect amble, rewind
					fseek(fp, lpos, SEEK_SET);
					break;
				}
			}

			((tdroid_108 *)droids[droid_ptr])->init_cmds(); // JN, 30AUG20
			droid_ptr++;
		}
	}

	fclose(fp);

	/*
	 * Load map
	 */
	snprintf(str, STR_LEN, "%s/%s/map.xpm", fleet_ptr->name, name); // JN, 28AUG20
	load_bm(str, &map_bm, 1);

	snprintf(str, STR_LEN, "SS %s", shipname);
	console_message_add(str);
	droids[0]->display_nd();
}

void tship::unload(void)
{
  int x;

  free_all_floor();
  free_all_transport();

 /*
  * normally, all tex's are loaded at start... accept for these, which
  * are continuely loaded for each level... therefore, to save filling
  * up text memory, we much dispose them.
  */
  delete_tex(map_bm.id);

  for(x = 0; x < MAX_DROIDS; x++)
    if(droids[x] != NULL) {
      if(verbose_logging == TRUE)
        printf("delete() droid (%s)\n", droids[x]->stats.type);
      delete(droids[x]);
    }
}

/***************************************************************************
* Put this in it's own method. JN, 31AUG20
***************************************************************************/
void tship::draw_ntitle(void)
{
	glColor4f(1.0, 1.0, 1.0, 0.5);
	blit_bm(&ntitle_bm, 0, 0);
}

/***************************************************************************
*
***************************************************************************/
void tship::get_ready_draw(void)
{
  char str[STR_LEN + 1];

  glColor4fv(bm_colour);
  blit_bm(&get_ready1_bm, SCREEN_HSIZE_X - (get_ready1_bm.size_x >> 1), 12);
  blit_bm(&(map_bm), SCREEN_HSIZE_X - ship_bm_offs_x[ship_ptr], 70);

  snprintf(str, STR_LEN, "Starship %s", name);
  glColor3fv(purple_pixel[0]);
  print_str(str, SCREEN_HSIZE_X - ((strlen(str) >> 1) * 7), 50);
}

/*
 * Made a dedicated background method for this state of the ship.
 * JN, 18SEP20
 */
void tship::get_ready_calc(void)
{
	get_ready_count++; // Reverted 2.2. 04SEP20
	if (get_ready_count > 80) {  // 4s wait. JN, 02SEP20
		state = STATE_RUN;
		sound_cmd(SND_CMD_FX_MIXED, FX_POWER_UP, 1.0, 1.2, 0.0, 0.0);
		set_ship_noise();
	}

	if(get_ready_siren_c < 3) {
		if(!get_ready_siren_w) {
			get_ready_siren_w = 20;
			get_ready_siren_c++;
			sound_cmd_primary(FX_ALERT_SIREN, 1.0, 1.0);
		} else {
			get_ready_siren_w--;
		}
	}
}

/***************************************************************************
*
***************************************************************************/
#define LEVEL_DRAW_SIZE_X 300
#define LEVEL_DRAW_SIZE_Y 167 // Changed. JN, 05SEP20
#define LEVEL_DRAW_Y_OFFS 13

void tship::diagram_draw(void)
{
  const char *mess1 = "Use arrow keys to change level.\n";
  int  ox, oy, sprtx, sprty;
  char str[STR_LEN + 1];
  float plse;

  draw_ntitle();  // JN, 31AUG20
  print_score();

  // Changed. JN, 05SEP20
  ox = SCREEN_HSIZE_X - (LEVEL_DRAW_SIZE_X >> 1);
  oy = SCREEN_HSIZE_Y - (LEVEL_DRAW_SIZE_Y >> 1) + LEVEL_DRAW_Y_OFFS;

  glLineWidth(1.0); // JN, 30AUG20

  glColor3fv(blue_pixel[0]);
  blit_rect(ox, oy, LEVEL_DRAW_SIZE_X, LEVEL_DRAW_SIZE_Y);
  glColor3fv(blue_pixel[4]);
  blit_rect_hollow(ox, oy, LEVEL_DRAW_SIZE_X, LEVEL_DRAW_SIZE_Y);

  glColor3fv(blue_pixel[7]);
  snprintf(str, STR_LEN, "StarShip %s (%s)\n", name, type); // JN, 28AUG20
  display_message(str, oy + 8);
  display_message((char *)mess1, 174); // New. JN, 05SEP20

  sprtx = ox + (LEVEL_DRAW_SIZE_X >> 1) - ship_bm_offs_x[ship_ptr];
  sprty = oy + (LEVEL_DRAW_SIZE_Y >> 1) - (map_bm.size_y >> 1) + 10;

  glColor4fv(bm_colour);
  blit_bm(&map_bm, sprtx, sprty);

  /*
   * Render spinny thing that indicates transport location.
   * JN, 04SEP20
   */
  sprtx += curr_location->diagram_px;
  sprty += curr_location->diagram_py;
  glPushMatrix();  // Animated transport indicator. JN, 04SEP20
  glTranslatef(sprtx, sprty, 0.0);
  glRotatef(s_transport_spin, 0.0, 0.0, 1.0);
  plse = 0.5 * fabsf(sinf(DEG2RAD(s_transport_spin)));
  plse += 0.5;
  glColor4f(1.0, 0.4, 0.4, plse);
  glLineWidth(4.0);
  plse *= 2.0;
  glScalef(plse, plse, 0.0);
  blit_rect_hollow(-2, -2, 4, 4);
  glPopMatrix();
}

/*
 * Animated transport indicator. Realtime calcs. JN, 04SEP20
 */
void tship::diagram_calc(void)
{
	transport_spin += 9;
	transport_spin %= 360;
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
* Worked this method a bit for larger font. Converted numerious sprintf()
* to snprintf() for safer memory. JN, 28AUG20
***************************************************************************/
#define CONSOLE_LN_SPACE 16

void tship::console_draw(void)
{
  int         ox, oy;
  char        str[STR_LEN + 1];
  const char  *header_mess = "Ship Computer: Droid Records\n";

//  draw_ntitle();  // JN, 31AUG20

  // Changed. JN, 05SEP20
  ox = SCREEN_HSIZE_X - (LEVEL_DRAW_SIZE_X >> 1);
  oy = SCREEN_HSIZE_Y - (LEVEL_DRAW_SIZE_Y >> 1) + LEVEL_DRAW_Y_OFFS;

  glLineWidth(1.0); // JN, 30AUG20
  glColor3fv(green_pixel[7]);
  blit_rect(ox, oy, LEVEL_DRAW_SIZE_X, LEVEL_DRAW_SIZE_Y);
  glColor3fv(green_pixel[2]);
  blit_rect_hollow(ox, oy, LEVEL_DRAW_SIZE_X, LEVEL_DRAW_SIZE_Y);

  glColor3fv(white_pixel[7]);
  display_message((char *)header_mess, 5);

  glColor3fv(green_pixel[0]);
  snprintf(str, STR_LEN,
	"Current: %s (%s)\n", droids[0]->stats.type, droids[0]->stats.name);
  display_message(str, oy + 3);

  ox += 5;
  oy += CONSOLE_LN_SPACE + 5;

  glColor3fv(green_pixel[3]);
  snprintf(str, STR_LEN, "  Type: %s", droid_stats[console_ptr].type);
  print_str(str, ox, oy);
  oy += CONSOLE_LN_SPACE;

  snprintf(str, STR_LEN, "  Name: %s", droid_stats[console_ptr].name);
  print_str(str, ox, oy);
  oy += CONSOLE_LN_SPACE;

  snprintf(str, STR_LEN, " Entry: %d", droid_stats[console_ptr].entry);
  print_str(str, ox, oy);
  oy += CONSOLE_LN_SPACE;

  snprintf(str, STR_LEN, "Height: %.2f Metres", droid_stats[console_ptr].height);
  print_str(str, ox, oy);
  oy += CONSOLE_LN_SPACE;

  snprintf(str, STR_LEN, "Weight: %.2f kg", droid_stats[console_ptr].weight);
  print_str(str, ox, oy);
  oy += CONSOLE_LN_SPACE;

  snprintf(str, STR_LEN, " Brain: %s",droid_stats[console_ptr].brain);
  print_str(str, ox, oy);
  oy += CONSOLE_LN_SPACE;

  switch(droid_stats[console_ptr].armament)
  {
    case 0:
      snprintf(str, STR_LEN, "Arm: None");
      break;
    case 1:
      snprintf(str, STR_LEN, "Arm: Linarite Laser");
      break;
    case 2:
      snprintf(str, STR_LEN, "Arm: Crocoite-Benzol Laser");
      break;
    case 3:
      snprintf(str, STR_LEN, "Arm: Uvarovite Laser");
      break;
    case 4:
      snprintf(str, STR_LEN, "Arm: Quartz 'Tiger-Eye' Laser");
      break;
  }
  print_str(str, ox + (8 * 3), oy);
  oy += CONSOLE_LN_SPACE;

  snprintf(str, STR_LEN, "Shield: %d", droid_stats[console_ptr].max_shielding);
  print_str(str, ox, oy);

  snprintf(str, STR_LEN, " Speed: %.0f", droid_stats[console_ptr].speed);
  print_str(str, ox + 125, oy);

  if(droid_stats[console_ptr].speed >= 0.0 &&
     droid_stats[console_ptr].speed <= 1.0)
  {
    snprintf(str, STR_LEN, "(Very Slow)");
  }
  else if(droid_stats[console_ptr].speed >  1.0 &&
          droid_stats[console_ptr].speed <= 2.0)
  {
    snprintf(str, STR_LEN, "(Slow)");
  }
  else if(droid_stats[console_ptr].speed >  2.0 &&
          droid_stats[console_ptr].speed <= 3.0)
  {
    snprintf(str, STR_LEN, "(Fair)");
  }
  else if(droid_stats[console_ptr].speed >  3.0 &&
          droid_stats[console_ptr].speed <= 4.0)
  {
    snprintf(str, STR_LEN, "(Average)");
  }
  else if(droid_stats[console_ptr].speed >  4.0 &&
          droid_stats[console_ptr].speed <= 5.0)
  {
    snprintf(str, STR_LEN, "(Fast)");
  }
  else if(droid_stats[console_ptr].speed >  5.0 &&
          droid_stats[console_ptr].speed <= 6.0)
  {
    snprintf(str, STR_LEN, "(Extreme)");
  }
  print_str(str, ox + (10 * 8) + 125, oy);
  oy += CONSOLE_LN_SPACE;

  // New. JN, 05SEP20
  if(droid_stats[console_ptr].attack_rate == -1)
    snprintf(str, STR_LEN, "Attack: N/A");
  else if(droid_stats[console_ptr].attack_rate == 0)
    snprintf(str, STR_LEN, "Attack: Deadly Firerate");
  else if(droid_stats[console_ptr].attack_rate >= 2 &&
          droid_stats[console_ptr].attack_rate <= 15)
    snprintf(str, STR_LEN, "Attack: Extreme Firerate");
  else if(droid_stats[console_ptr].attack_rate >= 17 &&
          droid_stats[console_ptr].attack_rate <= 15)
    snprintf(str, STR_LEN, "Attack: Fast Firerate");
  else
    snprintf(str, STR_LEN, "Attack: Slow Firerate");
  print_str(str, ox, oy);
}

/***************************************************************************
* New. JN, 09SEP20
***************************************************************************/
void tship::setup_flash_screen(float r, float g, float b, float rate)
{
	screen_flash_fade = rate;
	screen_flash_color[R] = r;
	screen_flash_color[G] = g;
	screen_flash_color[B] = b;
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
        sound_cmd_simple(SND_CMD_FX_SNOISE_STOP); //turn off ship noise
        transport_set();
        break;
      }

      if(s == 2) // paradroid hit space bar next to console
      {
        for(console_ptr = 0; console_ptr < DROID_999; console_ptr++)
          if(!strcmp(droids[0]->stats.type, droid_stats[console_ptr].type))
            break;

        state = STATE_CONSOLE;
        sound_cmd_simple(SND_CMD_FX_SNOISE_STOP); //New. JN, 17SEP20
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
              set_ship_noise();     //switch on ship noise
            }
            break;

          case SPECIAL_EVENT:
            switch(k.val)
            {
              case GLUT_KEY_UP:
                transport_change_up();
                sound_cmd_primary(FX_SELECT, 1.0, 1.0);
                break;

              case GLUT_KEY_DOWN:
                transport_change_dn();
                sound_cmd_primary(FX_SELECT, 1.0, 1.0);
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
            case KEY_HELP:
              state = STATE_PAUSED;
              pause_fade = 0.0;
              sound_cmd_simple(SND_CMD_FX_SNOISE_STOP); //New. JN, 17SEP20
              sound_cmd_primary(FX_SELECT, 1.0, 1.0);
              break;

            case KEY_QUIT:
              if(!droids[0]->state)
              {
                droids[0]->state = 1;
                sound_cmd(SND_CMD_FX_MIXED, FX_EXPLOSION_1, 1.0, 1.0, 0.0, 0.0);
              }
          }
        }
        break;

      case STATE_PAUSED:
        if(k.type == KEYBOARD_EVENT)
        {
          switch(k.val) // Expanded for both HELP and PAUSE key. JN, 09SEP20
          {
            case KEY_PAUSE:
            case KEY_HELP:
              state = STATE_RUN;
              set_ship_noise(); // New. JN, 17SEP20
              sound_cmd_primary(FX_SELECT, 1.0, 1.0);
            break;
          }
        }
        break;

      case STATE_CONSOLE:
        switch(k.type)
        {
          case KEYBOARD_EVENT:
            if(k.val == KEY_SELECT)
            {
              state = STATE_RUN;
              set_ship_noise(); // New. JN, 17SEP20
            }
            break;

          case SPECIAL_EVENT:
            switch(k.val)
            {
              case GLUT_KEY_LEFT:
                if(console_ptr > 0)
                  console_ptr--;
                sound_cmd_primary(FX_SELECT, 1.0, 1.0);
                break;

              case GLUT_KEY_RIGHT:
                if(console_ptr < DROID_999)
                  console_ptr++;
                sound_cmd_primary(FX_SELECT, 1.0, 1.0);
                break;
            }
            break; // Oops, this accidently got deleted from v2.2 to 3.0v. JN, 02SEP20
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
    case STATE_PAUSED: // New, pause fade. JN, 09SEP20
      pause_fade += 0.05;
      if(pause_fade > 0.7)
        pause_fade = 0.7;
      break;

    case STATE_RUN:
      {
        int i, nds = 0, ndf = 0;

        if(curr_location->floor != NULL)
          curr_location->floor->bg_calc();

       /*
        * Did a bit of work on this code to solve a detection bug when
        * detecting when a floor was clear of droids. The code was in
        * droid, but moved here. Tided up and integrated. JN, 18SEP20
        */
        for(i = 0; i < MAX_DROIDS; i++)
        {
          if(droids[i] == NULL)
            continue;

          droids[i]->bg_calc();  // Execute droids' background calcs

          if (i == 0) // Ignore the little guy in the ship/floor tallies below.
            continue;

          if((*(droids + i))->state < 2)
          {
            nds++; // Count the no of alive droids on the ship

            if(curr_location->floor == droids[i]->floor_ptr)
              ndf++; // Count the no of alive droids on the floor
          }
        }

	// Changed around this code a bit when looking for a bug.
	// JN, 18SEP20
        if(ship_complete_f)
        {
          if(!ship_end_cd)   // Reverted 2.2. 04SEP20
            state = STATE_COMPLETE;
          else
            ship_end_cd--; // Reverted 2.2. 04SEP20
        }
        else
        {
          if(!nds)
          {
            sound_cmd(SND_CMD_FX_MIXED, FX_SHIP_COMPLETE, 1.0, 1.5, 0.0, 0.0);
            ship_complete_f = 1;
          }

          if(curr_location->floor->down == 0)
          {
            if(!ndf)
            {
              sound_cmd(SND_CMD_FX_MIXED, FX_FLOOR_COMPLETE, 1.0, 0.5, 0.0, 0.0);
              curr_location->floor->down = 1;
              console_message_add((char *)"Floor clear");
            }
          }
        }

        if(droids[0]->state == 2) // Paradroid is dead.
        {
          if(ship_end_cd < 0)  // Reverted 2.2. 04SEP20
          {
            state = STATE_END;
            sound_cmd_simple(SND_CMD_FX_STOP_ALL); // JN, 17SEP20
          }
          else
            ship_end_cd -= 3; // Reverted 2.2. 04SEP20
        }

        console_message_alpha_fade -= 0.005;
        if(console_message_alpha_fade < 0.0)
          console_message_alpha_fade = 0.0;

        screen_flash_fade -= 0.2;
	if(screen_flash_fade < 0.0)
          screen_flash_fade = 0.0;
      }
      break;

    case STATE_GET_READY:
      get_ready_calc();
      break;

    case STATE_TRANSPORT:  // New. JN, 04SEP20
      diagram_calc();
      break;
  }
}

/***************************************************************************
* Updated this function for unsigned int score. Changed to base font.
* JN, 08SEP20
***************************************************************************/
void tship::print_score(void)
{
	unsigned int i, remdr;
	int x;

	glColor4fv(bm_colour2);
	remdr = sscore;
	for (i = 0, x = 127, remdr = sscore; i < 8; i++) {
		int d;

		d = remdr / division_table[i];
		remdr %= division_table[i]; // JN, 08SEP20
		blit_bm_font(&(font_bm[(int)(d + '0')]), x, 0);
		x += 9;
	}
}

/***************************************************************************
* New method. JN, 09SEP20
***************************************************************************/
void tship::paused(void)
{
	const char
		*mess1 = "= KEY MAP =\n\n",
		*mess2 = "'q' Quit, 'f' fps\n"
			"<Enter> Transfer mode\n"
			"'t' Transfer mode\n"
			"'c' Cascade, <Space> Select\n"
			"'s' Status, 'g' Mouse Grab\n"
			"'p' or 'h' This screen\n"
			"Arrows/keypad Move\n";

	glColor4f(0.0, 0.0, 0.0, pause_fade); // New, pause fade. JN, 09SEP20
	blit_rect(0, 0, window_size_x, window_size_x);

	glColor4f(1.0, 1.0, 0.0, 1.0);
	display_message((char *)mess1, 20);
	glColor4f(1.0, 1.0, 1.0, 1.0);
	display_message((char *)mess2, 55);
}

/***************************************************************************
*
***************************************************************************/
void tship::print_info(void)
{
  draw_ntitle();  // JN, 31AUG20

  glColor4f(0.0, 1.0, 0.0, 0.5);
  print_str(name, SCREEN_SIZE_X - (strlen(name) << 3), 0);
  print_str(
    curr_location->floor->name,
    SCREEN_SIZE_X - (strlen(curr_location->floor->name) << 3), 16);

  console_message_display();

  print_score();

  if(ship_complete_f)
  {
    glColor4fv(bm_colour);
    blit_bm(
      &ship_complete_bm, SCREEN_HSIZE_X - (168 >> 1), 50);
  }

  if(state == STATE_PAUSED)
    paused();   // JN, 09SEP20
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

  if(s_screen_flash_fade != 0.0) {
    glColor4f(
      screen_flash_color[R],
      screen_flash_color[G],
      screen_flash_color[B],
      s_screen_flash_fade);
    blit_rect(0, 0, window_size_x, window_size_x);
  }
}

/***************************************************************************
*
***************************************************************************/
void tship::level_run(void)
{
  snap_shot(); // New. 18SEP20

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
}
