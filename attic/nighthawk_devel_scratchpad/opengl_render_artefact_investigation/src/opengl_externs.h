/*
 * Variables
 */

extern double	display_delay;
extern int	window_size_x,
		window_size_y;

extern GLfloat white_pixel[8][3];

extern void	(*draw_hook)(void);

/*
 * Functions
 */
extern void ramp_colour(
  GLfloat r1, float g1, float b1,
  GLfloat r2, float g2, float b2,
  GLfloat *p, int run);
extern void blit_line(int x1, int y1, int x2, int y2);
extern void blit_rect(int x, int y, int size_x, int size_y);
extern void blit_rect_hollow(int x, int y, int size_x, int size_y);
extern void do_nothing(int sig);
extern void set_background_hook(void (*func)(int));
extern void timer_h(int value);
extern void opengl_init(int argc, char *argv[]);
