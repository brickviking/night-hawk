#ifndef _TDROID_1XX_H
#define _TDROID_1XX_H

#include "tdroid.h"

#define BLK_DETECT_BUF_SIZE 20

typedef struct { // Block detect buf
	float x;
	float y;
} blk_detect_buf_st;

typedef struct _tdroid_cmd_list
{
  char                    cmd;
  int                     d1;
  int                     d2;
  struct _tdroid_cmd_list *next;
} tdroid_cmd_list;

class tdroid_108 : public tdroid
{
public:
  int               count,
                    blk_detect_ptr,
                    blk_detect_count;
  tdroid_cmd_list   *droid_cmd_list, *droid_cmd_list_ptr;
  blk_detect_buf_st blk_detect_buf[BLK_DETECT_BUF_SIZE];
  float             blk_detect_old_pos_x, blk_detect_old_pos_y;

  tdroid_108(void);
  ~tdroid_108(void);
  void append_cmd(int cmd, int d1, int d2);
  void init_cmds(void); // Called after cmd list built. JN, 30AUG20
  void free_all_cmd(void);
  void set_dir_vect(float vx, float vy);
  void next_cmd(void);
  void queue_dequeue_block_buffer(void);
  int block_detect(void);
  void bg_calc(void);
};

class tdroid_176 : public tdroid_108
{
public:
  tdroid_176(void);
};

#endif
