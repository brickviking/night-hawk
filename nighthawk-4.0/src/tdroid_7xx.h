#ifndef _TDROID_7XX_H
#define _TDROID_7XX_H

#include "tdroid_6xx.h"

class tdroid_719 : public tdroid_606
{
public:
	int direction;
	int do_nothing_count;
	float avoid_angle;

	tdroid_719(void);
	void detect_avoid_condition(void);
	void move(void);
	void bg_calc(void);
};

class tdroid_720 : public tdroid_719
{
public:
	tdroid_720(void);
};

class tdroid_766 : public tdroid_719
{
public:
	tdroid_766(void);
};

class tdroid_799 : public tdroid_719
{
public:
	tdroid_799(void);
};

#endif
