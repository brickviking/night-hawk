#ifndef _TLASER_H
#define _TLASER_H

#include "tentity.h"

class tlaser : public tentity
{
public:
  tentity *owner;
  int     damage_factor, buzzed,
          old_pos_x_sign, pos_x_sign, old_pos_y_sign, pos_y_sign;

  tlaser(void);
  int init_ssv(float d, int s1, int s2, int s3);
  virtual void init(
    tfloor *f, tentity **dp, float px, float py, int vx, int vy, tentity *o);
  int colision_droids(float cx, float cy);
  void colision(float cx, float cy);
  void calc_buzz(void);
  void buzz(void);
  void draw(tfloor *c_f_ptr, int ppx, int ppy);
  void bg_calc(void);
private:
  int laser_bm_s;
};

class tlaser0 : public tlaser
{
public:
  tlaser0(void);
  void init(
    tfloor *f, tentity **dp, float px, float py, int vx, int vy, tentity *o);
};

class tlaser1 : public tlaser
{
public:
  tlaser1(void);
  void init(
    tfloor *f, tentity **dp, float px, float py, int vx, int vy, tentity *o);
};

class tlaser2 : public tlaser
{
public:
  tlaser2(void);
  void init(
    tfloor *f, tentity **dp, float px, float py, int vx, int vy, tentity *o);
};

class tlaser3 : public tlaser
{
public:
  tlaser3(void);
  void init(
    tfloor *f, tentity **dp, float px, float py, int vx, int vy, tentity *o);
};

#endif
