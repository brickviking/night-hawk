#ifndef _TDROID_H
#define _TDROID_H

#include "tentity.h"
#include "tlaser.h"

class tdroid : public tentity
{
public:
  tlaser *laser_list[MAX_LASERS];
  int    expl_anim_speed,  // Reverted 2.2. JN 04SEP20
         ss_expl_anim_ptr,
         expl_anim_ptr,
         expl_anim_len,
         recharge_delay;
  float  shield_wave_inc;
  int    captured;
  int    boast_msg_f;  // New, JN, 08SEP20

  tdroid(void);
  void init(tfloor *f, tentity **dp, float px, float py);
  int create_laser(int vx, int vy);
  void free_all_laser(void);
  void snap_shot(void);
  void draw_laser(tfloor *c_f_ptr, int px, int py);
  void bg_calc_laser(void);
  void draw_droid_ls(int ix, int iy);
  void draw_droid(int ix, int iy);
  void draw(tfloor *c_f_ptr, int px, int py);
  void recharge(void);
  void bg_calc(void);
  void friction(void);
  virtual void display_nd(void);
  virtual void hit(tlaser *l);
};

#endif
