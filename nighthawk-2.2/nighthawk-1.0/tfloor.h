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
  char       name[STR_LABEL_LEN];
  int        spos_x,spos_y;
  char       *map_filename;
  int        fmap_x_size;
  int        fmap_y_size;
  int        *fmap;
  tpower_bay *power_bay;
  tdoor      *door[MAX_DOORS];
  tpoint     transport[MAX_TRANSPORTS];
  tpoint     console;

  tfloor(void);
  virtual int create(int x_size,int y_size);
  int load(char *fn,char *fname);
  void set_pos(int x,int y);
  virtual void draw(void);
  void bg_calc(void);
  void unload(void);
};

#endif

