#ifndef _TEDIT_FLOOR_H
#define _TEDIT_FLOOR_H

#include "tfloor.h"

class tedit_floor : public tfloor
{
public:
  int  sprite_sel_ptr;
  int  default_map_value;

  tedit_floor(void);
  void init_colours(void);
  int create(char *fn,int x_size,int y_size);
  int save(void);
  void draw(void);
  void action(XEvent *event);
private:
  unsigned long black_pixel,white_pixel;
};

#endif
