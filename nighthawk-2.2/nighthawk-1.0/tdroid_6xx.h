#ifndef _TDROID_6XX_H
#define _TDROID_6XX_H

#include "tdroid_5xx.h"

class tdroid_606 : public tdroid_513
{
public:
  int attack_wait;

  tdroid_606(void);
  int target_not_hidden(tentity *d);
  virtual void attack(void);
};

class tdroid_691 : public tdroid_606
{
public:
  tdroid_691(void);
};

class tdroid_693 : public tdroid_606
{
public:
  tdroid_693(void);
};

#endif
