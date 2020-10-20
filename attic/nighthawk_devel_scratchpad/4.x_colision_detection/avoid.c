#include <stdio.h>
#include <math.h>

#define RAD2DEG(x) ((x * 180.0) / M_PI)
#define DEG2RAD(x) ((x * M_PI) / 180.0)

float	pos_x, pos_y,
	dpos_x, dpos_y,
	pos_dx, pos_dy,
	dpos_dx, dpos_dy;

float atan3f(float y, float x)
{
	float a;

	a = atan2f(y, x);
	if (a < 0.0) a += 2.0 * M_PI;
	return a;
}

void avoid(void)
{
	float dx, dy, dist, a, da, dang, df;

	/*
	 * Calc distance between droid[this] and other droid[other].
	 */
	dx = dpos_x - pos_x;
	dy = dpos_y - pos_y;
	dist = sqrtf(dx * dx + dy * dy);
	printf("dist=%.1f\n", dist);

	/*
	 * If out of range of avoidance. Ignore. Next droid[other]..
	 */
	if(dist >= 50.0)
		return;

	a = atan3f(dy, dx);
	printf("a=%.1f\n", RAD2DEG(a));

	/*
	 * Calc angle of droid[other] angle and flip 180deg
	 */
	da = atan3f(dpos_dy, dpos_dx);
	da = fmodf(da + M_PI, 2.0 * M_PI);
	printf("da=%.1f\n", RAD2DEG(da));

	/*
	 * Calc angular difference between direction angle droid[this] and
	 * droid[other]'s velocity vector. The sign will tell us where to
	 * droid[this].
	 */
	dang = fmodf(da - a, 2.0 * M_PI);
	printf("dang=%.1f\n", RAD2DEG(dang));

	/*
	 * Calc avoidance factor of droid[this]. The smaller the distance,
	 * the higher this factor. Max value is 2.0 to stop droid[this]
	 * shooting out of the map and core dumping.
	 */
	df = 5.0 / dist;
	if (df > 2.0)
		df = 2.0;
	printf("df=%.1f\n", df);

	/*
	 * Adjust direction angle + or - 90 deg.
	 */
	if (dang < 0.0)
		a -= M_PI_2;
	else
		a += M_PI_2;
	printf("adj a=%.1f\n", RAD2DEG(a));

	/*
	 * Adjust droid[this] velocity vector.
	 */
	pos_dx = df * cosf(a);
	pos_dy = df * sinf(a);
	printf("pos_dx=%.3f, pos_dy=%.3f\n", pos_dx, pos_dy);
}

int main(void)
{
	pos_x = 0.0;
	pos_y = 0.0;
	pos_dx = 0.0;
	pos_dy = 1.0;

	dpos_x = 0.0;
	dpos_y = 10.0;
	dpos_dx = -1.0;
	dpos_dy = 0.0;

	avoid();

	return 0;
}
