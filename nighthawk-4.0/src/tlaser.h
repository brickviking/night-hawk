#ifndef _TLASER_H
#define _TLASER_H

#include "tentity.h"

class tlaser : public tentity
{
public:
  tentity *owner;
  int     damage_factor, buzzed,
          old_pos_x_sign, pos_x_sign, old_pos_y_sign, pos_y_sign;
  float   laser_angle; // New 4.0. JN, 02OCT20

  tlaser(void);
  virtual void init(
    tfloor *f, tentity **dp, float px, float py, int vx, int vy, tentity *o);
  int colision_doors(void);
  int colision_droids(void);
  void colision_detect(void);
  void calc_buzz(void);
  void buzz(void);
  void draw(tfloor *c_f_ptr, int ppx, int ppy);
  void bg_calc(void);
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
