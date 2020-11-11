/*
 * Variables
 */

extern int	halt_game_f;
extern int	display_delay;
extern int	window_size_x,
		window_size_y;

extern void	(*draw_hook)(void);

extern GLfloat	black_pixel[1][3],
		white_pixel[8][3],
		blue_pixel[8][3],
		yellow_pixel[1][3],
		green_pixel[MAX_SCORE_TABLE][3],
		red_pixel[MAX_SCORE_TABLE][3],
		intro_haze[2][3],
		purple_pixel[1][3];

extern GLfloat	bm_colour[4],
		bm_colour2[4];

extern char	*console_message[MAX_MESSAGES];
extern float	console_message_alpha_fade;

extern unsigned int explosion_count;

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
extern void blit_bm(bm_t *bm, int x, int y);
extern void blit_bm_scale(bm_t *bm, int x, int y, int scale); //JN, 31AUG20
extern void blit_bm_font(bm_t *bm, int x, int y); // JN, 08OCT20
extern void print_str_len(char *str, int l, int x, int y);
extern void print_str(char *str, int x, int y);
extern void print_str_hazey(char *str, int x, int y, GLfloat *c1, GLfloat *c2);
extern void display_message(char *s, int y);
extern void append_tex(GLuint id);
extern void delete_tex(GLuint id);
extern void free_textures(void);
extern void convert_2_tex(GLubyte *buf, bm_t *bm, int aligned_f);
extern void console_message_add(char *str);
extern void console_message_display(void);
extern void console_message_free(void);
extern int get_special_key_state(void);
extern void shake_reset(void);
extern void shake_calc(void);
extern void set_background_hook(void (*func)(int));
extern void timer_h(int value);
extern void opengl_init(int argc, char *argv[]);
