#ifndef _TLASER_H
#define _TLASER_H

#include "tentity.h"

class tlaser0 : public tentity
{
public:
  tentity *owner;
  int     damage_factor;

  tlaser0(void);
  int init_ssv(float d,int s1,int s2,int s3);
  virtual void init(
    tfloor *f,tentity **dp,float px,float py,int vx,int vy,tentity *o);
  int colision_droids(float cx,float cy);
  void colision(float cx,float cy);
  void draw(tfloor *c_f_ptr,int ppx,int ppy);
  void bg_calc(void);
private:
  int laser_bm_s;
};

class tlaser1 : public tlaser0
{
public:
  tlaser1(void);
  void init(
    tfloor *f,tentity **dp,float px,float py,int vx,int vy,tentity *o);
};

class tlaser2 : public tlaser0
{
public:
  tlaser2(void);
  void init(
    tfloor *f,tentity **dp,float px,float py,int vx,int vy,tentity *o);
};

class tlaser3 : public tlaser0
{
public:
  tlaser3(void);
  void init(
    tfloor *f,tentity **dp,float px,float py,int vx,int vy,tentity *o);
};

#endif
