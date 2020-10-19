/*
 * Variables
 */
extern char	*home_override_path,
		*data_path,
		*scores_path,
		*preserve_path;

extern int	verbose_logging,
		god_mode;

extern char     fleet_start[SCORES_FLEET_LEN + 1];
extern fleet_st	*fleet_table,
		*fleet_ptr;
extern int	fleet_table_size;

extern int      ship_start_cmdline_f;
extern char	ship_start[SCORES_HSHIP_LEN + 1];
extern char	**ship_table;
extern int	ship_ptr;

extern void (*bg_calc_hook)(void);

/*
 * Functions
 */
extern void end_game(int status);
extern void printf_error(const char *fmt, ...);
extern void print_error(const char *func, const char *cmd);
extern void kill_proc(int sig);
extern void seg_fault(int sig);
extern void *alloc_mem(int mem_size);
extern char *alloc_mem_char(int mem_size);
extern void *realloc_mem(void *p, int mem_size);
extern void print_new_error(const char *func);
extern void strncpy2(char *dst, char *src, int len);
extern void strip_leading_whitespace(char *str);
extern char *concat_filepaths(char *fp1, char *fp2);
extern char *rel_to_abs_filepath(char *filepath);
extern char *rel_to_abs_filepath_fleet(char *filepath);
extern void init_kb_event(void);
extern void end_kb_event(void);
extern void put_kb_event(tkevent *k);
extern int get_kb_event(tkevent *k);
extern double get_clock(void);
extern void do_nothing(void);
extern void init_background_timer(void);
extern void sound_cmd_simple(char cmd);
extern void sound_cmd_primary(int fx_no, float vol, float freq);
extern void sound_cmd_snoise(int fx_no, float vol, float freq);
extern void sound_cmd(char cmd, int fx_no, float vol, float freq, int rx, int ry);
extern void create_filepaths(void);
extern void calc_norm_accel(float *d, float ref_v, float v, float div, float lim);
extern float atan3f(float y, float x);
extern void load_ship_table(void);
extern void free_ship_table(void);
extern int find_entry_in_ship_table(char *entry);
extern void load_fleet_table(void);
extern void find_fleet_entry(char *entry);
extern int test_mouse_pos(tkevent *k, int x, int y, int w, int h);
