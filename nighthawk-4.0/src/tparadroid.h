#ifndef _TPARADROID_H
#define _TPARADROID_H

#include "tdroid.h"

class tparadroid : public tdroid
{
public:
  int     transferring, button_down, tug1,
          tug2, low_shields_trig, cascade;
  tdroid  *captured_droid;

  tparadroid(void);
  ~tparadroid(void);
  void init(tfloor *f, tentity **dp, float px, float py);
  void draw(void);
  int within_capt_dist(void);
  void bg_calc(void);
  void capture_droid(int cx, int cy);
  int action(tkevent *kp);
  void display_nd(void);
  void hit(tlaser *l);
};

#endif
