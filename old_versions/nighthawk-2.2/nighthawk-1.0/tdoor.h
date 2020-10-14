#ifndef _TDOOR_H
#define _TDOOR_H

#include "tpower_bay.h"

class tdoor : public tpower_bay
{
public:
  int state;
  int wait_t;

  tdoor(void);
  void init(int px,int py,int t);
  void open(void);
  void bg_calc(void);
};

#endif
