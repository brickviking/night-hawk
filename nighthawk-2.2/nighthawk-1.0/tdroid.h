#ifndef _TDROID_H
#define _TDROID_H

#include "tentity.h"
#include "tlaser.h"

class tdroid : public tentity
{
public:
  tlaser0 *laser_list[MAX_LASERS];
  int expl_anim_speed,ss_expl_anim_ptr,expl_anim_ptr,expl_anim_len;
  float shield_wave_inc;
  int captured;

  tdroid(void);
  void init(tfloor *f,tentity **dp,float px,float py);
  int create_laser(int vx,int vy);
  void free_all_laser(void);
  void snap_shot(void);
  void draw_laser(tfloor *c_f_ptr,int px,int py);
  void bg_calc_laser(void);
  void shields_low(int ix,int iy);
  void draw(tfloor *c_f_ptr,int px,int py);
  void bg_calc(void);
  void friction(void);
  virtual void display_nd(void);
  void hit(tlaser0 *l);
};

#endif
