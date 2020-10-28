/***************************************************************************
****************************************************************************
****************************************************************************
*
* Nighthawk Copyright (C) 1997 Jason Nunn
* Nighthawk is under the GNU General Public License.
*
* Sourceforge admin: Eric Gillespie
* night-hawk.sourceforge.net
*
* See README file for a list of contributors.
*
* ----------------------------------------------------------------
* misc.c - Miscellaneous code.
*
* 28OCT20: Added atexit() handling to fix close window bug. JN
*
****************************************************************************
****************************************************************************
***************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include <ctype.h>
#include <time.h>
#include <signal.h>
#include <semaphore.h>
#include <sys/time.h>
#include <math.h>
#include <AL/al.h>
#include <AL/alc.h>
#include "defs.h"
#include "sprite_loader_externs.h"
#include "opengl_externs.h"
#include "openal_externs.h"

char		*home_override_path, /*paths setup by create_filepaths*/
		*data_path,
		*scores_path,
		*preserve_path;

int		verbose_logging,
		god_mode;	/*Move god mode to here. JN,28AUG20*/

/*
 * New for 4.0. Multiple fleets. JN, 02OCT20.
 */
char		fleet_start[SCORES_FLEET_LEN + 1];
fleet_st	*fleet_table,
		*fleet_ptr;
int		fleet_table_size;

/*
 * Nighthawk can now have more than 10 ships. JN, 06SEP20
 */
int		ship_start_cmdline_f;
char		ship_start[SCORES_HSHIP_LEN + 1];
char		**ship_table;
int		ship_ptr;

const char	*scores_path_compiletime = SCORES_PATH,
		*scores_fn_compiletime = SCORES_FILENAME,
		*preserve_fn_compiletime = PRESERVE_FILENAME;

void (*bg_calc_hook)(void);
void end_kb_event(void);

/***************************************************************************
* end_game() must be called whenever the game needs to exit. This is the exit
* point of the game for portability purposes (ie: other operating systems will
* have other methods of program termination etc).
*
* Most program exemptions/signaling is handled by the sigaction() code in
* main:init(), however, an alpha tester (wotnot from austech), found that when
* closing the game window, the game wasn't able to handle this situation via
* it's signal handling. GLUT has a fix for this (glutSetOption()), but this fix
* is a GLUT extension (and not a standard GLUT function, and I don't want to
* use non-standard functions). So, the work around is to modify end_game() and
* create an at_exit_h() function. So, now the game exits nicely when you close
* the window on it. JN, 28OCT20
***************************************************************************/
void atexit_h(void)
{
	if (verbose_logging == TRUE)
		printf("Shutting down resources.\n");
	free_textures();
	end_kb_event();
	shutdown_sound();
}

void end_game(int status)
{
	if (!status)
		printf("See ya\n");

	exit(status); /*This then calls atexit_h(). JN, 28OCT20*/
}

/***************************************************************************
* Improved error handling functions. JN 24AUG20
***************************************************************************/
void printf_error(const char *fmt, ...)
{
	va_list		ap;
	char		msg[STR_LEN_HUGE + 1];

	va_start(ap, fmt);
	vsnprintf(msg, sizeof(msg), fmt, ap);
	va_end(ap);

	printf("Error: %s.\n", msg);
	end_game(-1);
}

void print_error(const char *func, const char *cmd)
{
	printf("Error: %s:%s- %s\n", func, cmd, strerror(errno));
	end_game(-1);
}

void kill_proc(int sig)
{
	printf_error("Termination signal received, terminating game");
}

void seg_fault(int sig)
{
	printf_error("Segmentation fault. Aborting (Please contact developers)");
}

/***************************************************************************
* Improved memory handing functions. JN, 24AUG20
***************************************************************************/
void *alloc_mem(int mem_size)
{
	void *r;

	r = malloc(mem_size);
	if (r == NULL)
		print_error(__func__, "malloc()");

	return r;
}

/*
 * Because C++ has turned into such as arse of an extension, I need this,
 * or rather, I can't be bothered learning the millionth revision of the C++.
 * I don't need C++ to write good object orientated code. If I could convert
 * this game to C, then I would. Rant over ! :(. JN, 06SEP20
 */
char *alloc_mem_char(int mem_size)
{
	return alloc_mem(mem_size);
}

void *realloc_mem(void *p, int mem_size)
{
	void *r;

	r = realloc(p, mem_size);
	if (r == NULL)
		print_error(__func__, "realloc()");

	return r;
}

void print_new_error(const char *func)
{
	print_error(__func__, "new <class>");
	end_game(-1);
}

/****************************************************************************
* String functions added, JN, 26AUG20
****************************************************************************/
void strncpy2(char *dst, char *src, int len)
{
	strncpy(dst, src, len);
	dst[len] = 0;
}

void strip_leading_whitespace(char *str)
{
	char *s, *se;

	s = se = str + strlen(str) - 1;
	if (s == str)
		return;
	for (; s != str; s--) {
		if(!isspace(*s)) {
			if (s != se)
				s[1] = 0;
			return;
		}
	}
}

/****************************************************************************
* New for 4.0. This function was created by request for Eric who wants to
* set a different data location for level creation purposes. This also
* serves the scores and preservation files JN, 06SEP20
****************************************************************************/
static char *concat_filepaths(char *fp1, char *fp2)
{
	int l;
	char *new_str;

	l = strlen(fp1) + strlen(fp2) + 2;
	new_str = alloc_mem(l);
	strcpy(new_str, fp1);
	l = strlen(new_str);
	if (new_str[l] == '/')
		new_str[l] = 0;
	strcat(new_str, fp2);
	return new_str;
}

char *rel_to_abs_filepath(char *filepath)
{
	return concat_filepaths(data_path, filepath);
}

char *rel_to_abs_filepath_fleet(char *filepath)
{
	char str[STR_LEN + 1];

	snprintf(str, STR_LEN, "/%s%s", fleet_ptr->name, filepath);
	return concat_filepaths(data_path, str);
}

/****************************************************************************
* Added semaphores for OpenGL input hooks and foreground code. Suspect
* execution is not mutually exclusive. It is to try to solve a rare core
* dump bug. JN, 12SEP20
****************************************************************************/
static sem_t	kb_event_sem;
static tkevent	kevent[KEVENT_SIZE];
static int	kevent_r_ptr, kevent_w_ptr;

void init_kb_event(void)
{
	if (sem_init(&kb_event_sem, 0, 1) == -1)
		print_error(__func__, "sem_init()");
}

void end_kb_event(void)
{
	sem_destroy(&kb_event_sem);
}

void put_kb_event(tkevent *k)
{
	if (sem_wait(&kb_event_sem) == -1)
		print_error(__func__, "sem_wait()");

	memcpy(&kevent[kevent_w_ptr], k, sizeof(tkevent));
	kevent_w_ptr++;
	kevent_w_ptr &= (KEVENT_SIZE - 1);

	if (sem_post(&kb_event_sem) == -1)
		print_error(__func__, "sem_post()");
}

int get_kb_event(tkevent *k)
{
	if(kevent_r_ptr == kevent_w_ptr)
		return 0;

	if (sem_wait(&kb_event_sem) == -1)
		print_error(__func__, "sem_wait()");

	memcpy(k, &kevent[kevent_r_ptr], sizeof(tkevent));
	kevent_r_ptr++;
	kevent_r_ptr &= (KEVENT_SIZE - 1);

	if (sem_post(&kb_event_sem) == -1)
		print_error(__func__, "sem_post()");

	return 1;
}

/****************************************************************************
* Timing
****************************************************************************/
double get_clock(void)
{
	struct timeval t;

	gettimeofday(&t, NULL);
	return ((double)t.tv_sec + (0.000001 * (double)t.tv_usec));
}

/****************************************************************************
* Background calculation functions. Reintroduced from v2.2 into v4.0.
****************************************************************************/
void do_nothing(void) // Dummy call back function for timer_h(). JN, 03SEP20
{
}

static void background_timer_h(int sig)
{
	if (halt_game_f == TRUE)
		return;

	bg_calc_hook();
}

void init_background_timer(void)
{
	struct sigaction timer_action;
	struct itimerval timer;

	timer_action.sa_handler = background_timer_h;
	sigemptyset(&timer_action.sa_mask);
	timer_action.sa_flags = 0;

	bg_calc_hook = do_nothing;
	sigaction(SIGALRM, &timer_action, NULL);

	timer.it_value.tv_sec = 0;
	timer.it_interval.tv_sec = 0;
	timer.it_value.tv_usec = CALC_DELAY;
	timer.it_interval.tv_usec = CALC_DELAY;
	setitimer(ITIMER_REAL, &timer, NULL);
}

/***************************************************************************
* The sound FX queuing requests the background context makes during the
* game. JN, 17SEP20
***************************************************************************/
void sound_cmd_simple(char cmd)
{
	snd_event_st se;

	se.cmd = cmd;
	put_snd_event(&se);
}

void sound_cmd_primary(int fx_no, float vol, float freq)
{
	snd_event_st se;

	se.cmd = SND_CMD_FX_PRI;
	se.fx_no = fx_no;
	se.vol = vol;
	se.freq = freq;
	put_snd_event(&se);
}

void sound_cmd_snoise(int fx_no, float vol, float freq)
{
	snd_event_st se;

	se.cmd = SND_CMD_FX_SNOISE;
	se.fx_no = fx_no;
	se.vol = vol;
	se.freq = freq;
	put_snd_event(&se);
}

void sound_cmd(char cmd, int fx_no, float vol, float freq, int rx, int ry)
{
	snd_event_st se;

	if (fx_no == FX_EXPLOSION_1 || fx_no == FX_EXPLOSION_2)
		explosion_count++;

	se.cmd = cmd;
	se.fx_no = fx_no;
	se.vol = vol;
	se.freq = freq;
	se.rx = rx;  /*relative sound position to paradroid. JN, 18SEP20*/
	se.ry = ry;
	put_snd_event(&se);
}

/***************************************************************************
* New v4.0 function
***************************************************************************/
void create_filepaths(void)
{
	char	*home;

	if (data_path == NULL) {
		data_path = alloc_mem_char(strlen(DATA_PATH) + 1);
		strcpy(data_path, DATA_PATH);
	}

	if (home_override_path != NULL)
		home = home_override_path;
	else {
		home = getenv("HOME");
		if (home == NULL) {
			printf(
"Warning: $HOME environmental variable not found. "
"Unable to setup file handling. You are a Windose user ??. "
"Suggest specifying $HOME with the -s option.\n");
			return;
		}
	}

	preserve_path = concat_filepaths(home, (char *)preserve_fn_compiletime);

	/*
	 * Score directory is an absolute filepath.
	 */
	if (scores_path_compiletime[0] == '/') {
		scores_path = concat_filepaths(
			(char *)scores_path_compiletime,
			(char *)scores_fn_compiletime);
		return;
	}

	/*
	 * Score directory is relative. Ignore it, and save score file in
	 * user's account as a hidden file.
	 */
	scores_path = concat_filepaths(home, (char *)scores_fn_compiletime);
}

/***************************************************************************
* New v4.0 function
***************************************************************************/
void calc_norm_accel(float *d, float ref_v, float v, float div, float lim)
{
	*d += (v - ref_v) / div;
	if (*d > lim)
		*d = lim;
	if (*d < -lim)
		*d = -lim;
}

/***************************************************************************
* New v4.0 function. 0 - 2xpi atan2() function
***************************************************************************/
float atan3f(float y, float x)
{
	float a;

	a = atan2f(y, x);
	if (a < 0.0) a += 2.0 * M_PI;
	return a;
}

/***************************************************************************
* This function is for loading fleet.d and ctrl.d files. Returns the
* number of entries loaded. This function will be called everytime a game
* starts to play.
***************************************************************************/
void load_ship_table(void)
{
	char str[STR_LEN + 1];
	FILE *fp;
	int i;

	if (verbose_logging == TRUE)
		printf("Loading ship list.\n");

	fp = fopen(rel_to_abs_filepath_fleet((char *)"/ctrl.d"), "r");
	if (fp == NULL)
		print_error(__func__, "fopen()");

	for (i = 0;;) {
		if (fgets(str, SCORES_HSHIP_LEN, fp) == NULL)
			break;

		ship_table = (char **)realloc_mem(ship_table,
					((i + 1) * sizeof(char **)));
		strip_leading_whitespace(str);
		ship_table[i] = (char *)alloc_mem(strlen(str) + 1);
		strcpy(ship_table[i], str);
		i++;
	}

	if (i == 0)
		printf_error("Ship list is empty");

	/*
	 * finish off with the last ship table entry being a NULL.
	 */
	ship_table = (char **)realloc_mem(ship_table, i * sizeof(char **));
	ship_table[i] = NULL;
	fclose(fp);

	if (verbose_logging == TRUE)
		for (char **p = ship_table, i = 1; *p != NULL; p++, i++)
			printf("%d. %s\n", i, *p);
}

/***************************************************************************
* New for 4.0. JN, 02OCT20.
***************************************************************************/
void free_ship_table(void)
{
	if (ship_table == NULL)
		return;

	if (verbose_logging == TRUE)
		printf("Freeing ship list.\n");
	for (char **p = ship_table; *p != NULL; p++)
		free(*p);
	free(ship_table);
	ship_table = NULL;
}

/***************************************************************************
* New for 4.0. Find entry in ship table. Return index no. JN, 02OCT20.
***************************************************************************/
int find_entry_in_ship_table(char *entry)
{
	char **p = ship_table;
	int i;

	if (verbose_logging == TRUE)
		printf("Finding ship %s.\n", entry);

	for (i = 0; *p != NULL; p++, i++) {
		if (!strcmp(*p, entry)) {
			if (verbose_logging == TRUE)
				printf("\tFound ! (%d)\n", i);
			return i;
		}
	}

	printf_error("Could not find '%s' in ship table", entry);
	return 0;
}

/***************************************************************************
* New for 4.0. Load fleet table. This is only called once at start of
* program. JN, 02OCT20.
***************************************************************************/
static void load_fleet_comment(fleet_st *f)
{
	FILE *fp;
	int s;
	char str[STR_LEN + 1];

	snprintf(str, STR_LEN, "/%s/comment.d", f->name);
	fp = fopen(rel_to_abs_filepath(str), "r");
	if (fp == NULL)
		print_error(__func__, "fopen()");
	if (fgets(str, STR_LABEL_LEN, fp) == NULL)
		printf_error("Empty comment.d file in %s", f->name);
	s = strlen(str);
	f->comment = (char *)alloc_mem(s + 1);
	strncpy2(f->comment, str, s);
	fclose(fp);
}

static void load_fleet_scroll_story(fleet_st *f)
{
	FILE *fp;
	char str[STR_LEN + 1];
	int i, ptr, ns;

	snprintf(str, STR_LEN, "/%s/scroll_story.d", f->name);
	fp = fopen(rel_to_abs_filepath(str), "r");
	if (fp == NULL)
		print_error(__func__, "fopen()");

	/*
	 * Skip first 2 lines.
	 */
	for (i = 0; i < 2; i++)
		if (fgets(str, STR_LEN, fp) == NULL)
			printf_error(
				"Error: Error reading scroll story for fleet %s.\n",
				f->name);

	/*
	 * Load the rest. The first and last characters of each line are ignored.
	 */
	ptr = 0;
	for (;;) {
		int s;

		if (fgets(str, STR_LEN, fp) == NULL)
			break;

		strip_leading_whitespace(str);
		s = strlen(str) - 2;
		ns = ptr + s;
		f->scroll_story = (char *)realloc_mem(f->scroll_story, ns);
		memcpy(f->scroll_story + ptr, str + 1, s);
		ptr = ns;
	}

	ns = ptr + 1;
	f->scroll_story = (char *)realloc_mem(f->scroll_story, ns);
	f->scroll_story[ptr] = 0;

	fclose(fp);
}

void load_fleet_table(void)
{
	char str[STR_LEN + 1];
	FILE *fp;

	if (verbose_logging == TRUE)
		printf("Loading fleet List.\n");

	fp = fopen(rel_to_abs_filepath((char *)"/fleet.d"), "r");
	if (fp == NULL)
		print_error(__func__, "fopen()");

	for (fleet_table_size = 0;;) {
		if (fgets(str, SCORES_FLEET_LEN, fp) == NULL)
			break;

		fleet_table = (fleet_st *)realloc_mem(fleet_table,
				((fleet_table_size + 1) * sizeof(fleet_st)));
		strip_leading_whitespace(str);
		if (verbose_logging == TRUE)
			printf("\tParsing fleet: %s\n", str);
		strncpy2(fleet_table[fleet_table_size].name, str,
							SCORES_FLEET_LEN);
		load_fleet_comment(&fleet_table[fleet_table_size]);
		load_fleet_scroll_story(&fleet_table[fleet_table_size]);
		fleet_table_size++;
	}

	if (fleet_table_size == 0)
		printf_error("Fleet list is empty");

	fclose(fp);

	if (verbose_logging == TRUE)
		for (int i = 0; i < fleet_table_size; i++)
			printf("%d. %s\n",
				i + 1,
				fleet_table[i].name);
}

/***************************************************************************
* New for 4.0. Find entry fleet table. set fleet_ptr. JN, 02OCT20.
***************************************************************************/
void find_fleet_entry(char *entry)
{
	if (verbose_logging == TRUE)
		printf("Finding fleet %s.\n", entry);

	for (int i = 0; i < fleet_table_size; i++)
		if (!strcmp(fleet_table[i].name, entry)) {
			fleet_ptr = &fleet_table[i];
			if (verbose_logging == TRUE)
				printf("\tFound !\n");
			return;
		}

	printf_error("Could not find '%s' in fleet table", entry);
}

/***************************************************************************
* New for 4.0. Test if mouse pointer is within defined box. JN, 04OCT20.
***************************************************************************/
int test_mouse_pos(tkevent *k, int x, int y, int w, int h)
{
	int	x2, y2;

	x2 = x + w;
	y2 = y + h;
	if (k->mouse_x >= x && k->mouse_x <= x2 &&
				k->mouse_y >= y && k->mouse_y <= y2)
		return TRUE;

	return FALSE;
}
