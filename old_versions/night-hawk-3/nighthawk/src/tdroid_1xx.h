#ifndef _TDROID_1XX_H
#define _TDROID_1XX_H

#include "tdroid.h"

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
  int             count;
  tdroid_cmd_list *droid_cmd_list, *droid_cmd_list_ptr;

  tdroid_108(void);
  ~tdroid_108(void);
  int append_cmd(tdroid_cmd_list **ptr, int cmd, int d1, int d2);
  void free_all_cmd(tdroid_cmd_list **ptr);
  void set_dir_vect(float vx, float vy);
  void next_cmd(void);
  void bg_calc(void);
};

class tdroid_176 : public tdroid_108
{
public:
  tdroid_176(void);
};

#endif
