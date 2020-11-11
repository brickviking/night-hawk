#ifndef _TFLOOR_H
#define _TFLOOR_H

#include "tpower_bay.h"
#include "tdoor.h"

typedef struct
{
  int x;
  int y;
} tpoint;

class tfloor
{
public:
  char       name[STR_LABEL_LEN + 1],
             *map_filename;
  int        spos_x, spos_y,
             fmap_x_size,
             fmap_y_size,
             *fmap,
             down;
  int        ship_noise_no;
  int        ship_noise_vol;
  int        ship_noise_freq;
  tpower_bay *power_bay;
  tdoor      *door[MAX_DOORS];
  tpoint     transport[MAX_TRANSPORTS];
  tpoint     console;
  float      colour;

  tfloor(void);
  void create(int x_size, int y_size);  //removed virtual. Nothing inherits it. JN, 28AUG20
  void load_floor_map(char *fn);
  void load_floor_misc(char *fn);
  void load(char *fn, char *fname);
  void set_pos(int x, int y);
  void reinit(void);
  void draw(void); //removed virtual. Nothing inherits it. JN, 28AUG20
  void bg_calc(void);
  void unload(void);
};

#endif
