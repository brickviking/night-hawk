#ifndef _TDROID_8XX_H
#define _TDROID_8XX_H

#include "tdroid_6xx.h"

class tdroid_805 : public tdroid_606
{
public:
  int evase,voice_c;

  tdroid_805(void);
  virtual void attack(void);
};

class tdroid_810 : public tdroid_805
{
public:
  tdroid_810(void);
};

class tdroid_820 : public tdroid_805
{
public:
  tdroid_820(void);
};

class tdroid_899 : public tdroid_805
{
public:
  tdroid_899(void);
};

#endif
