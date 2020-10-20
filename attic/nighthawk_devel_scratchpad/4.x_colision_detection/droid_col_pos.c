#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define RAD2DEG(x) ((x * 180.0) / M_PI)
#define DEG2RAD(x) ((x * M_PI) / 180.0)

#define TAB_SIZE 64
#define STR_LEN  40

char tab[TAB_SIZE][STR_LEN + 1];

void plot(void)
{
	int i;
	float a = 0.0;

	for (i = 0; i < TAB_SIZE; i++) {
		float x, y;

		x = 11.0 * cosf(a);
		y = 11.0 * sinf(a);

		snprintf(tab[i], STR_LEN, "\t{%.0f, %.0f}, %c*%.0f deg*/",
			nearbyint(x), nearbyint(y), '/', nearbyint(RAD2DEG(a)));

		a += (2.0 * M_PI) / TAB_SIZE;
		a = fmodf(a, 2.0 * M_PI);
	}
}

void print_rand(void)
{
	for(int c = 0; c < TAB_SIZE;) {
		int i;

		i = random() % TAB_SIZE;
		if (tab[i][0] == 0)
			continue;
		puts(tab[i]);
		tab[i][0] = 0;
		c++;
	}
}

int main(void)
{
	srandom(2346);
	plot();
	print_rand();

	return 0;
}

