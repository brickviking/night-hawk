#ifndef _TSHIP_H
#define _TSHIP_H

#define STATE_TRANSPORT 0
#define STATE_RUN       1
#define STATE_END       2
#define STATE_PAUSED    3
#define STATE_CONSOLE   4
#define STATE_GET_READY 5
#define STATE_COMPLETE  6

#include "tfloor.h"
#include "tparadroid.h"
#include "tdroid_1xx.h"
#include "tdroid_2xx.h"
#include "tdroid_3xx.h"
#include "tdroid_4xx.h"
#include "tdroid_5xx.h"
#include "tdroid_6xx.h"
#include "tdroid_7xx.h"
#include "tdroid_8xx.h"
#include "tdroid_9xx.h"

typedef struct _tfloor_list
{
  tfloor              *curr;
  struct _tfloor_list *next;
} tfloor_list;

typedef struct _ttransport_list
{
  tfloor                  *floor;
  int                     diagram_px;
  int                     diagram_py;
  int                     floor_pos_x;
  int                     floor_pos_y;
  int                     no;
  struct _ttransport_list *next;
} ttransport_list;

class tship
{
public:
  int             ship_complete_f,
                  state,
                  console_ptr,
                  ship_end_cd,  // Reverted 2.2. JN, 04SEP20
                  get_ready_count,
                  get_ready_siren_c,
                  get_ready_siren_w,
                  s_transport_spin, transport_spin;
  char            name[STR_LABEL_LEN + 1],
                  type[STR_LABEL_LEN + 1];
  tfloor_list     *floor_list;
  ttransport_list *transport_list, *curr_location;
  bm_t            map_bm;
  tdroid          *droids[MAX_DROIDS];
  float           pause_fade;
  float           s_screen_flash_fade, screen_flash_fade;
  float           screen_flash_color[3];

  tship(void);
  void snap_shot(void);
  void create_floor(char *fname);
  tfloor *find_floor(char *name);
  void free_all_floor(void);
  void create_transport(char *fn, int px, int py, int fx, int fy, int tn);
  ttransport_list *find_transport(char *name, int x, int y);
  void free_all_transport(void);
  void set_ship_noise(void);
  void load(const char *shipname);
  void unload(void);
  void draw_ntitle(void);   // New. JN, 31AUG20
  void get_ready_draw(void);
  void get_ready_calc(void); // New. JN, 18SEP20
  void diagram_draw(void);
  void diagram_calc(void);  // New. JN, 04SEP20
  void transport_set(void);
  void transport_change(ttransport_list *selection);
  void transport_change_up(void);
  void transport_change_dn(void);
  void console_draw(void);
  void setup_flash_screen(float r, float g, float b, float rate);
  void level_check_event(void);
  void level_bg_calc(void);
  void print_score(void);
  void paused(void);
  void print_info(void);
  void play_draw(void);
  void level_run(void);
};

#endif
