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
                  console_ptr;
  char            name[STR_LABEL_LEN + 1],
                  type[STR_LABEL_LEN + 1];
  tfloor_list     *floor_list;
  ttransport_list *transport_list, *curr_location;
  bm_t            map_bm;
  tdroid          *droids[MAX_DROIDS];
  double          get_ready_count;
  float           ship_end_cd;

  tship(void);
  void create_floor(tfloor_list **ptr, char *fname);
  tfloor *find_floor(char *name);
  ttransport_list *create_transport(
    ttransport_list **ptr, char *fn, int px, int py, int fx, int fy, int tn);
  ttransport_list *find_transport(char *name, int x, int y);
  void free_all_transport(ttransport_list **ptr);
  void free_all_floor(tfloor_list **ptr);
//  void set_ship_noise(void);
  void load(char *shipname);
  void unload(void);
  void get_ready_draw(void);
  void diagram_draw(void);
  void transport_set(void);
  void transport_change(ttransport_list *selection);
  void transport_change_up(void);
  void transport_change_dn(void);
  void console_draw(void);
  void level_check_event(void);
  void level_bg_calc(void);
  void print_score(void);
  void print_info(void);
  void play_draw(void);
  void level_run(void);
};

#endif
