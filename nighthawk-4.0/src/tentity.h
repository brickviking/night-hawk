#ifndef _TENTITY_H
#define _TENTITY_H

#include "tfloor.h"

class tentity
{
public:
  tdroid_stats stats;
  int          state;
  tfloor       *floor_ptr;
  int          spos_x, spos_y;
  int          ss_anim_ptr, anim_ptr, anim_len, anim_speed; // Reverted to 2.2. JN, 04SEP20
  float        pos_x, pos_y, old_pos_x, old_pos_y, pos_dx, pos_dy;
  bm_t         *bm;
  tentity      **droids;
  tentity      *attack_droid;

  tentity(void);
  virtual ~tentity(void);
  virtual void init(
    tfloor *f, bm_t *bm_p, tentity **dp, int alen, float px, float py);
  virtual void snap_shot(void);
  void do_sound(int fx_no, float vol, float freq);
  int droid_circum_box_detect(int x1, int y1, int x2, int y2);
  void open_door(tdoor *d);
  int colision_floor(void);
  int colision_doors(void);
  int colision_droids(void);
  virtual void colision_detect(void);
  virtual void bg_calc(void);
};

#endif
