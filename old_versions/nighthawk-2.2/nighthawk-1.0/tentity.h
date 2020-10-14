#ifndef _TENTITY_H
#define _TENTITY_H

class tentity
{
public:
  tdroid_stats stats;
  int          state;
  tfloor       *floor_ptr;
  int          ss_anim_ptr,spos_x,spos_y;
  float        pos_x,pos_y,old_pos_x,old_pos_y,pos_dx,pos_dy;
  int          anim_ptr,anim_len,anim_speed;
  tbm          *bm;
  tentity      **droids;
  tentity      *attack_droid;

  tentity(void);
  virtual ~tentity(void);
  virtual void init(
    tfloor *f,tbm *bm_p,tentity **dp,int alen,float px,float py);
  virtual void snap_shot(void);
  int colision_other(float cx,float cy);
  int colision_droids(float cx,float cy);
  virtual void colision(float cx,float cy);
  virtual void bg_calc(void);
};

#endif

