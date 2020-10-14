#ifndef _TPOWER_BAY_H
#define _TPOWER_BAY_H

class tpower_bay
{
public:
  int    pos_x, pos_y;
  double anim_ptr, anim_len, anim_speed;
  bm_t   *bm;

  tpower_bay(void);
  void init(int px, int py);
  void draw(int px, int py);
  void bg_calc(void);
};

#endif
