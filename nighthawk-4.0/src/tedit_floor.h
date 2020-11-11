#ifndef _TEDIT_FLOOR_H
#define _TEDIT_FLOOR_H

#include "tfloor.h"

class tedit_floor : public tfloor  // Restored for 4.x. JN, 27SEP20
{
public:
	int	sprite_sel_ptr,
		default_map_value,
		blank;
	float	sflashing_alpha, flashing_alpha;

	tedit_floor(void);
	void snap_shot(void);
	void create(int x_size, int y_size);
	void save(void);
	void process_user_inputs(void);
	void draw(void);
	void bg_calc(void);
};

#endif
