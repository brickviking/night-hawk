/*
 * Reverted to 2.2. JN, 04SEP20
 */
#ifndef _TDOOR_H
#define _TDOOR_H

#include "tpower_bay.h"

class tdoor : public tpower_bay
{
public:
  int state;
  int wait_t;
  int type;  //Vertical door = 0, Horizontal door = 1. JN, 26SEP20

  tdoor(void);
  void init(int px, int py, int t);
  int  open(void);
  void bg_calc(void);
};

#endif
