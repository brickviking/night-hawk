#ifndef _TSHIP_DEMO_H
#define _TSHIP_DEMO_H

#include "tship.h"

typedef struct 
{
  char *mess;
  int  dc;
} tmess_tab;

class tship_demo : public tship
{
public:
  int  mess_ptr,mess_dis_cd;

  tship_demo(void);
  void print_info(void);
  void level_bg_calc(void);
  void level_run(void);
};

#endif
