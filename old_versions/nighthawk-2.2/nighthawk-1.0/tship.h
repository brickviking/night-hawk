#ifndef _TSHIP_H
#define _TSHIP_H

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

typedef struct _ttransport_list_link
{
  char                         floor[STR_LABEL_LEN];
  int                          floor_pos_x;
  int                          floor_pos_y;
  struct _ttransport_list_link *next;
} ttransport_list_link;

typedef struct _ttransport_list
{
  tfloor                  *floor;
  int                     diagram_px;
  int                     diagram_py;
  int                     floor_pos_x;
  int                     floor_pos_y;
  ttransport_list_link    *transport_list_link;
  struct _ttransport_list *next;
} ttransport_list;

class tship
{
public:
  int             ship_complete_f;
  int             ship_end_cd;
  int             state;
  char            name[STR_LABEL_LEN + 1];
  char            type[STR_LABEL_LEN + 1];
  tfloor_list     *floor_list;
  ttransport_list *transport_list,*curr_location;
  tbm             map_bm;
  tdroid          *droids[MAX_DROIDS];
  int             console_ptr;

  tship(void);
  int create_floor(tfloor_list **ptr,char *fname);
  tfloor *find_floor(char *name);
  ttransport_list *create_transport(
    ttransport_list **ptr,char *fn,int px,int py,int fx,int fy);
  ttransport_list *find_transport(char *name,int x,int y);
  int create_transport_link(
    ttransport_list_link **ptr,char *fn,int x,int y);
  void free_all_transport_links(ttransport_list_link **ptr);
  void free_all_transport(ttransport_list **ptr);
  void free_all_floor(tfloor_list **ptr);
  int load(char *shipname);
  void unload(void);
  void diagram_draw(void);
  void transport_set(void);
  void transport_change(void);
  void console_draw(void);
  void level_check_event();
  void level_bg_calc(void);
  void print_score(void);
  void print_info(void);
  void play_draw(void);
  void level_run(void);
};

#endif
