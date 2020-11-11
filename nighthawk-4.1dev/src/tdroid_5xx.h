#ifndef _TDROID_5XX_H
#define _TDROID_5XX_H

#include "tdroid_1xx.h"

class tdroid_513 : public tdroid_108
{
public:
  tdroid_513(void);
  virtual void attack(void);
  virtual void bg_calc(void);
};

class tdroid_520 : public tdroid_513
{
public:
  tdroid_520(void);
};

class tdroid_599 : public tdroid_513
{
public:
  tdroid_599(void);
};

#endif
