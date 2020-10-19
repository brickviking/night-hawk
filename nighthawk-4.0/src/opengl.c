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
* OpenGL code
*
****************************************************************************
****************************************************************************
***************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <GL/glut.h>
#include <GL/glx.h>
#include <AL/al.h>
#include <AL/alc.h>
#include "defs.h"
#include "misc_externs.h"
#include "sprite_loader_externs.h"
#include "scores_externs.h"
#include "openal_externs.h"

typedef struct _tex_ll_t
{
  GLuint           id;
  struct _tex_ll_t *next;
} tex_ll_t;

int		halt_game_f = FALSE;
int		display_delay = DISPLAY_DELAY;
int		window_size_x = WINDOW_SIZE_X, /*Thanks Eric :). JN. 15SEP20*/
		window_size_y = WINDOW_SIZE_Y;

void		(*draw_hook)(void);

GLfloat	black_pixel[1][3],
	white_pixel[8][3],
	blue_pixel[8][3],
	yellow_pixel[1][3] = {{0.9375, 0.9375, 0.0}},
	green_pixel[MAX_SCORE_TABLE][3],
	red_pixel[MAX_SCORE_TABLE][3],
	intro_haze[2][3] = {{0.0, 0.9, 0.0}, {0.0, 0.35, 0.0}},
	purple_pixel[1][3] = {{0.7, 0.1, 8.0}};

GLfloat		bm_colour[4] = {1.0, 1.0, 1.0, 1.0},
		bm_colour2[4] = {1.0, 1.0, 1.0, 0.6};

char		*console_message[MAX_MESSAGES];
float		console_message_alpha_fade;

unsigned int	explosion_count;

static tex_ll_t	*tex_ll;
static int	mouse_grabber_f,
		fps_f;
static int	reshape_window_fg; /*This is for window reshaping. JN, 05SEP20*/

static int	shake_display;
static float	shake_x, shake_y, shake_decay, shake_phase;

static int	special_key_state;

/****************************************************************************
*
****************************************************************************/
void ramp_colour(
  GLfloat r1, float g1, float b1,
  GLfloat r2, float g2, float b2,
  GLfloat *p, int run)
{
  int     i, m1, m2;
  GLfloat rs, gs, bs;

  p[R] = r1;
  p[G] = g1;
  p[B] = b1;
  i = run - 1;
  rs = (r2 - r1) / (GLfloat)i;
  gs = (g2 - g1) / (GLfloat)i;
  bs = (b2 - b1) / (GLfloat)i;

  m1 = 3;
  m2 = 0;
  for(i = 1; i < run; i++)
  {
    p[m1 + R] = p[m2 + R] + rs;
    p[m1 + G] = p[m2 + G] + gs;
    p[m1 + B] = p[m2 + B] + bs;
    m1 += 3;
    m2 += 3;
  }
}

/****************************************************************************
*
****************************************************************************/
void blit_line(int x1, int y1, int x2, int y2)
{
  glDisable(GL_TEXTURE_2D);

  glBegin(GL_LINES);
    glVertex2i(x1, y1);
    glVertex2i(x2, y2);
  glEnd();

  glEnable(GL_TEXTURE_2D);
}

/****************************************************************************
*
****************************************************************************/
void blit_rect(int x, int y, int size_x, int size_y)
{
  size_x += x;
  size_y += y;

  glDisable(GL_TEXTURE_2D);

  glBegin(GL_POLYGON);
    glVertex2i(x, y);
    glVertex2i(x, size_y);
    glVertex2i(size_x, size_y);
    glVertex2i(size_x, y);
  glEnd();

  glEnable(GL_TEXTURE_2D);
}

/****************************************************************************
*
****************************************************************************/
void blit_rect_hollow(int x, int y, int size_x, int size_y)
{
  size_x += x;
  size_y += y;

  glDisable(GL_TEXTURE_2D);

  glBegin(GL_LINE_STRIP);
    glVertex2i(x, y);
    glVertex2i(x, size_y);
    glVertex2i(size_x, size_y);
    glVertex2i(size_x, y);
    glVertex2i(x, y);
  glEnd();

  glEnable(GL_TEXTURE_2D);
}

/****************************************************************************
* Bitmap blitters
****************************************************************************/
static const GLfloat blit_bm_texcoord[4][2] =
	{{0.0, 0.0}, {0.0, 1.0}, {1.0, 1.0}, {1.0, 0.0}};

void blit_bm(bm_t *bm, int x, int y)
{
  int x1, y1;

  x1 = x + bm->size_x;
  y1 = y + bm->size_y;

  if(x1 < 0 || y1 < 0)
    return;
  if(x >= SCREEN_SIZE_X || y >= SCREEN_SIZE_Y)
    return;

  glBindTexture(GL_TEXTURE_2D, bm->id);
  glBegin(GL_POLYGON);
    glTexCoord2fv(blit_bm_texcoord[0]);
    glVertex2i(x, y);

    glTexCoord2fv(blit_bm_texcoord[1]);
    glVertex2i(x, y1);

    glTexCoord2fv(blit_bm_texcoord[2]);
    glVertex2i(x1, y1);

    glTexCoord2fv(blit_bm_texcoord[3]);
    glVertex2i(x1, y);
  glEnd();
}

/*
 * New function. JN, 31AUG20
 */
void blit_bm_scale(bm_t *bm, int x, int y, int scale)
{
	int x1, y1, sx, sy;

	/*
	 * Get bitmap size and scale size by half (because font has
	 * doubled in size.
	 */
	if (scale > 0) {
		sx = bm->size_x * scale;
		sy = bm->size_y * scale;
	} else if (scale < 0) {
		scale *= -1;
		sx = bm->size_x / scale;
		sy = bm->size_y / scale;
	} else {
		sx = bm->size_x;
		sy = bm->size_y;
	}

	x1 = x + sx;
	y1 = y + sy;
	/*
	 * Render the bitmap textured polygon.
	 */
	glBindTexture(GL_TEXTURE_2D, bm->id);
	glBegin(GL_POLYGON);
		glTexCoord2fv(blit_bm_texcoord[0]);
		glVertex2i(x, y);

		glTexCoord2fv(blit_bm_texcoord[1]);
		glVertex2i(x, y1);

		glTexCoord2fv(blit_bm_texcoord[2]);
		glVertex2i(x1, y1);

		glTexCoord2fv(blit_bm_texcoord[3]);
		glVertex2i(x1, y);
	glEnd();
}

/*
 * Special case for font bitmaps (based on gprof analysis). JN, 09OCT20
 */
void blit_bm_font(bm_t *bm, int x, int y)
{
	int x1, y1;

	x1 = x + (FONT_SIZE_X >> 1);
	y1 = y + (FONT_SIZE_Y >> 1);
	glBindTexture(GL_TEXTURE_2D, bm->id);
	glBegin(GL_POLYGON);
		glTexCoord2fv(blit_bm_texcoord[0]);
		glVertex2i(x, y);

		glTexCoord2fv(blit_bm_texcoord[1]);
		glVertex2i(x, y1);

		glTexCoord2fv(blit_bm_texcoord[2]);
		glVertex2i(x1, y1);

		glTexCoord2fv(blit_bm_texcoord[3]);
		glVertex2i(x1, y);
	glEnd();
}

/****************************************************************************
*
****************************************************************************/
void print_str_len(char *str, int l, int x, int y)
{
	int i;

	for (i = 0; i < l; i++) {
		blit_bm_font(&(font_bm[(int)str[i]]), x, y);
		x += 8;
	}
}

void print_str(char *str, int x, int y)
{
	int i;

	for (i = 0; i < strlen(str); i++) {
		blit_bm_font(&(font_bm[(int)str[i]]), x, y);
		x += 8;
	}
}

void print_str_hazey(char *str, int x, int y, GLfloat *c1, GLfloat *c2)
{
	int l;

	l = strlen(str); // Run strlen() once. JN, 10OCT20

	glColor3fv(c2);
	print_str_len(str, l, x - 1, y);
	print_str_len(str, l, x + 1, y);
	print_str_len(str, l, x, y - 1);
	print_str_len(str, l, x, y + 1);

	glColor3fv(c1);
	print_str_len(str, l, x, y);
}

/****************************************************************************
*
****************************************************************************/
void display_message(char *s, int y)
{
  int  l;
  char *s1;

  for(;;)
  {
    if(*s == '\0')
      return;

    s1 = strchr(s, '\n');
    if(s1 == NULL)
      return;

    l = (int)(s1 - s);
    if(l)
      print_str_len(s, l, SCREEN_HSIZE_X - (l << 2), y); // Bitwise fix. JN, 31AUG20 
    y += 16;
    s = s1 + 1;
  }
}

/****************************************************************************
* tex management
****************************************************************************/
void append_tex(GLuint id)
{
  tex_ll_t *t;

  t = tex_ll;
  tex_ll = (tex_ll_t *)alloc_mem(sizeof(tex_ll_t));
  tex_ll->id = id;
  tex_ll->next = t;
}

void delete_tex(GLuint id)
{
  tex_ll_t *p, *t;

  p = tex_ll;
  for(;;)
  {
    if(p == NULL)
      break;

    if(p->id == id)
    {
      glDeleteTextures(1, &(p->id));

      p->id = p->next->id;
      t = p->next;
      p->next = p->next->next;
      free(t);
    }

    p = p->next;
  }
}

void free_textures(void)
{
  tex_ll_t *p;

  if(verbose_logging == TRUE)
    printf("Freeing Textures.\n");

  p = tex_ll;
  for(;;)
  {
    if(p == NULL)
      break;

    glDeleteTextures(1, &(p->id));

    p = p->next;
  }
}

/****************************************************************************
* Some words about what the aligned_f does. If set, this function sets the
* texture context via glTexImage2D(). Under older OpenGL libraries, this
* code required the width to be aligned in a certain way (normally had to
* be divisable by two or go up in powers of 2 etc). If this was not performed,
* substantial performance degradation was seen. So, when there are bitmaps
* that aren't aligned, the more memory hungry gluBuild2DMipmaps() function is
* normally called in by programs This function produces a set of different
* sized texture maps based on the sizes the library likes. But Some pics
* using gluBuild2DMipmaps() look a bit funny though and would require
* glTexParameteri() tweaking (and this function would get complicated. In the
* pasted 20 years or so (on Linux at least), glTexImage2D() doesn't seem as
* fussy.
*
* Nb/ I just had a quick look in my /usr/include/GL/gl.h file. There are no
* notes in there about special handling of glTexImage2D(), so my observation
* is probably correct. JN, 07OCT20
****************************************************************************/
void convert_2_tex(GLubyte *buf, bm_t *bm, int aligned_f) {
  glGenTextures(1, &(bm->id));
  if(!bm->id)
    printf_error("convert_2_tex()::glGenTextures(): Error.");

  glBindTexture(GL_TEXTURE_2D, bm->id);

/* Set texture parameters */
  if(aligned_f)
  {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  }
  else
  {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  }

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexEnvi(GL_TEXTURE_2D, GL_TEXTURE_ENV_MODE, GL_MODULATE);

  if(aligned_f)
    glTexImage2D(GL_TEXTURE_2D, 0, 4, bm->size_x, bm->size_y,
      0, GL_RGBA, GL_UNSIGNED_BYTE, buf);
  else
    gluBuild2DMipmaps(GL_TEXTURE_2D, 4, bm->size_x, bm->size_y, 
      GL_RGBA, GL_UNSIGNED_BYTE, buf);

  append_tex(bm->id);
}

/****************************************************************************
*
****************************************************************************/
void console_message_add(char *str)
{
  int x;

  if(console_message[0] != NULL)
    free(console_message[0]);

  for(x = 0; x < (MAX_MESSAGES - 1); x++)
    console_message[x] = console_message[x + 1];

  x = strlen(str) + 1;
  console_message[MAX_MESSAGES - 1] = (char *)alloc_mem(x);
  strncpy2(console_message[MAX_MESSAGES - 1], str, x);
  console_message_alpha_fade = 0.5;
}

void console_message_display(void)
{
  int x;

  if(console_message_alpha_fade < 0.0)
    return;

  glColor4f(0.0, 1.0, 0.0, console_message_alpha_fade);

  for(x = 0; x < MAX_MESSAGES; x++)
  {
    if(console_message[x] == NULL)
      continue;

    print_str(
      console_message[x],
      SCREEN_SIZE_X - (strlen(console_message[x]) << 3),
      (SCREEN_SIZE_Y - 80) + (x << 4));
  }
}

void console_message_free(void)
{
  int x;

  for(x = 0; x < MAX_MESSAGES; x++)
    if(console_message[x] != NULL)
    {
      free(console_message[x]);
      console_message[x] = NULL;
    }
}

/****************************************************************************
*
****************************************************************************/
static void timer_h(int value)
{
	glutPostRedisplay();
	glutTimerFunc(display_delay, timer_h, 0);
}

/****************************************************************************
* OpenGL mouse grabber function. As requested by Eric (unfortunately his
* UAE code is Xlib, so couldn't be used). JN, 06SEP20
****************************************************************************/
static int rt_mouse_x, rt_mouse_y;

/*
 * This callback is run every time the mouse is moved, regardless if mouse
 * button is pressed or not.
 */
static void motion_h(int x, int y)
{
	rt_mouse_x = x;
	rt_mouse_y = y;
}

static void entry_h(int state)
{
	if (verbose_logging == TRUE)
		printf("Window focus change. Resetting special key state.\n");

	if (mouse_grabber_f == FALSE) {
		/*
		 * Reset special_key_state. This is to fix an uncommanded
		 * movement bug when game window loses focus. JN, 15OCT20
		 */
		special_key_state = 0;
		return;
	}

	if (state != GLUT_LEFT)
		return;
	glutWarpPointer(rt_mouse_x, rt_mouse_y);
}

/****************************************************************************
* OpenGL Mouse and keyboard handles. These handles use kb_event
****************************************************************************/
static inline int mouse_aspect(int v, int w, int r)
{
  return v * r / w;
}

static void keyboard_h(unsigned char key, int x, int y)
{
	switch(tolower(key)) {
		case KEY_FPS:
		fps_f ^= 1;
		sound_cmd_primary(FX_SELECT, 1.0, 1.0); /*New. JN, 17SEP20*/
		break;

		case MOUSE_GRAB: /* New for Eric. JN, 06SEP20*/
		mouse_grabber_f ^= 1;
		sound_cmd_primary(FX_SELECT, 1.0, 1.0); /*New. JN, 17SEP20*/
		break;

		default: {
			tkevent k;

			k.type    = KEYBOARD_EVENT;
			k.val     = key;
			k.mouse_x = mouse_aspect(x, window_size_x, SCREEN_SIZE_X);
			k.mouse_y = mouse_aspect(y, window_size_y, SCREEN_SIZE_Y);
			put_kb_event(&k);
			break;
		}
	}
}

static void mouse_h(int button, int state, int x, int y)
{
  tkevent k;

  if(!state)
    return;

  k.type    = MOUSE_EVENT;
  k.val     = button;
  k.mouse_x = mouse_aspect(x, window_size_x, SCREEN_SIZE_X);
  k.mouse_y = mouse_aspect(y, window_size_y, SCREEN_SIZE_Y);
  put_kb_event(&k);
}

/****************************************************************************
* With removal of XLib library, arrow keys need to be handled differently
* in paradroid(). So, for consoles, transport input, kvent is used. To
* move droid around, get_special_key_state() is used. JN, 01SEP20
****************************************************************************/
int get_special_key_state(void)
{
	return special_key_state;
}

static void special_h(int key, int x, int y)
{
	tkevent k;

	k.type    = SPECIAL_EVENT;
	k.val     = (unsigned int)key;
	k.mouse_x = mouse_aspect(x, window_size_x, SCREEN_SIZE_X);
	k.mouse_y = mouse_aspect(y, window_size_y, SCREEN_SIZE_Y);
	put_kb_event(&k);

	/*
	 * Added. JN, 01SEP20
	 */
	switch (key) {
		case GLUT_KEY_UP:
		special_key_state |= SP_KEY_STATE_UP;
		break;

		case GLUT_KEY_DOWN:
		special_key_state |= SP_KEY_STATE_DOWN;
		break;

		case GLUT_KEY_LEFT:
		special_key_state |= SP_KEY_STATE_LEFT;
		break;

		case GLUT_KEY_RIGHT:
		special_key_state |= SP_KEY_STATE_RIGHT;
		break;

		/*
		 * This is based on Eric's diagonal keyboard code.
		 * JN, 01SEP20
		 */
		case GLUT_KEY_HOME:
		special_key_state |= SP_KEY_STATE_UP_LF;
		break;

		case GLUT_KEY_PAGE_UP:
		special_key_state |= SP_KEY_STATE_UP_RT;
		break;

		case GLUT_KEY_END:
		special_key_state |= SP_KEY_STATE_DN_LF;
		break;

		case GLUT_KEY_PAGE_DOWN:
		special_key_state |= SP_KEY_STATE_DN_RT;
		break;
	}
}

static void special_up_h(int key, int x, int y)
{
	switch (key) {
		case GLUT_KEY_UP:
		special_key_state &= ~SP_KEY_STATE_UP;
		break;

		case GLUT_KEY_DOWN:
		special_key_state &= ~SP_KEY_STATE_DOWN;
		break;

		case GLUT_KEY_LEFT:
		special_key_state &= ~SP_KEY_STATE_LEFT;
		break;

		case GLUT_KEY_RIGHT:
		special_key_state &= ~SP_KEY_STATE_RIGHT;
		break;

		/*
		 * This is based on Eric's diagonal keyboard code in v2.3.
		 * JN, 01SEP20
		 */
		case GLUT_KEY_HOME:
		special_key_state &= ~SP_KEY_STATE_UP_LF;
		break;

		case GLUT_KEY_PAGE_UP:
		special_key_state &= ~SP_KEY_STATE_UP_RT;
		break;

		case GLUT_KEY_END:
		special_key_state &= ~SP_KEY_STATE_DN_LF;
		break;

		case GLUT_KEY_PAGE_DOWN:
		special_key_state &= ~SP_KEY_STATE_DN_RT;
		break;
	}
}

/****************************************************************************
* New. Shake display
****************************************************************************/
static void detect_explosions(void)
{
	static int c;

	if ((c++ % 5) != 0)
		return;

	if (explosion_count >= 3) {
		shake_display = TRUE;
		if (explosion_count > 2)
			explosion_count = 2;
		shake_decay = (float)explosion_count;
	}
	explosion_count = 0;
}

void shake_reset(void)
{
	shake_x = shake_y = 0;
	shake_display  = FALSE;
}

void shake_calc(void)
{
	detect_explosions();

	if (shake_display == FALSE)
		return;

	shake_x = shake_decay * sinf(shake_phase);
	shake_y = shake_decay * cosf(shake_phase);
	shake_phase += 8.0;
	shake_decay -= 0.3;
	if (shake_decay > 0.0)
		return;

	shake_display = FALSE;
	shake_x = shake_y  = 0.0;
}

/****************************************************************************
* Resize window (with auto discovery code). JN, 05SEP20
****************************************************************************/
static void set_2d_mapping(int w, int h)
{
	if (verbose_logging == TRUE)
		printf("Adjusting OpenGL 2D mapping %dx%d.\n", w, h);

	/*
	 * Set Viewport to window dimensions
	 */
	glViewport(0, 0, w, h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	/*
	 * Set clipping volume
	 */
	glOrtho(0.0, SCREEN_SIZE_X, SCREEN_SIZE_Y, 0.0, 1.0, -1.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

static void resize_h(int w, int h)
{
	float wf, hf, a;

	if (verbose_logging == TRUE)
		printf("Window resized from %dx%d to %dx%d\n",
			window_size_x, window_size_y, w, h);

	/*
	 * Window size we told it to set has been set. Tell OpenGL and we're
	 * done.
	 */
	if (window_size_x == w && window_size_y == h) {
		set_2d_mapping(w, h);
		return;
	}

	/*
	 * Set correct aspect
	 */
	wf = w;
	hf = h;
	a = wf / hf;
	if (a > SCR_ASPECT_43_TOLERANCE)
		wf = hf * SCR_ASPECT_43;
	else if (a < SCR_ASPECT_43_TOLERANCE)
		hf = wf / SCR_ASPECT_43;
	else {
		/*
		 * Resized window is correct aspect. Tell OpenGL our decided
		 * screen size and we're done.
		 */
		window_size_x = w;
		window_size_y = h;
		set_2d_mapping(w, h);
		return;
	}

	/*
	 * Tell OpenGL what screen size we want and see if it likes it.
	 * This will cause this function to be called again. It's a bit of
	 * a negotiation (a bit like transferring to a droid :)
	 */
	if (verbose_logging == TRUE)
		printf("Adjusting screen aspect to 4:3 %.0fx%.0f.\n", wf, hf);
	window_size_x = wf;
	window_size_y = hf;
	reshape_window_fg = TRUE; /*tell draw_h() to run glutReshapeWindow()*/
}

/****************************************************************************
* Main drawing function
****************************************************************************/
static void draw_h(void)
{
	static char	str[STR_LABEL_LEN + 1],
			*mess = "You cheat!",
			*mess_mouse_grabber = "M LCK"; // ECG, 17OCT20
	static int	frame_count;

	if (halt_game_f == TRUE)
		return;

	/*
	 * Low priority functions (processed once a second)
	 * JN, 04SEP20
	 */
	if ((frame_count++ % 25) == 0) {
		if (fps_f) {
			static double old_clock, x;

			x = get_clock();
			sprintf(str, "%.2f fps", 25.0 / (x - old_clock));
			old_clock = x;
		}

		/*
		 * glutReshapeWindow() does not work in the resize_h() context,
		 * So resize_h() tells the foreground code to run it. A bit
		 * hacky, but oh well. JN, 05SEP20
		 */
		if (reshape_window_fg == TRUE) {
			reshape_window_fg = FALSE;
			glutReshapeWindow(window_size_x, window_size_y);
		}
	}

	glClear(GL_COLOR_BUFFER_BIT);
	glPushMatrix();  // JN, 02SEP20
	glEnable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);

	/*
	 * New feature. Shake display using the power of OpenGL. Here we
	 * simply change the point of view (POV). JN, 06SEP20
	 */
	if (shake_display == TRUE)
		glTranslatef(shake_x, shake_y, 0.0);

	if (draw_hook != NULL)
		draw_hook();

	/*
	 * Draw mode icons.
	 */
	if (fps_f == TRUE) {
		glColor4f(1.0, 1.0, 1.0, 0.3);
		print_str(str, 0, 184);
	}
	if (god_mode == TRUE || ship_start_cmdline_f == TRUE) {  // Modified. JN 10OCT20
		glColor4fv(bm_colour);
		print_str(mess, 120, SCREEN_SIZE_Y - (FONT_SIZE_X + 2));
	}
	if (mouse_grabber_f == TRUE) {
		glColor4f(1.0, 1.0, 0.0, 0.3);
		print_str(mess_mouse_grabber, 80, 0); // ECG, 17OCT20
	}

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
	glPopMatrix(); /*JN, 02SEP20*/
	glutSwapBuffers();

	/*
	 * Process the sounds and music commands the background code put
	 * on the snd_queue. JN, 14SEP20
	 */
	sound_process_fg();
}

/****************************************************************************
* New. Suspends game if game window looses visibility. Designed to stop
* unpredictable behaviour when virtual desktop is changed. JN 21SEP20
****************************************************************************/
static void visibility_h(int state)
{
	if (state == GLUT_NOT_VISIBLE) {
		if (verbose_logging)
			printf("Lost visibility. Game paused.\n");
		halt_game_f = TRUE;
		return;
	}

	if (verbose_logging)
		printf("Regained visibility. Game resumed.\n");
	halt_game_f = FALSE;
}

/****************************************************************************
* Reworked this function. JN, 04SEP20
****************************************************************************/
void opengl_init(int argc, char *argv[])
{
	if (verbose_logging == TRUE)
		printf("Initing the OpenGL graphics system..\n");

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(window_size_x, window_size_y);
	glutCreateWindow("Nighthawk " VERSION);

	glutIgnoreKeyRepeat(TRUE);
	glutKeyboardFunc(keyboard_h);
	glutSpecialFunc(special_h);
	glutSpecialUpFunc(special_up_h);
	glutMouseFunc(mouse_h);

	/*
	 * This is Eric's mouse grabber function. JN, 06SEP20
	 */
	glutMotionFunc(motion_h); /*sense mouse when button pressed*/
	glutPassiveMotionFunc(motion_h); /*ditto, when not pressed*/
	glutEntryFunc(entry_h); /*when mouse leaves/enters window*/

	glutTimerFunc(display_delay, timer_h, 1);
	glutReshapeFunc(resize_h);
	glutDisplayFunc(draw_h);
	glutVisibilityFunc(visibility_h);

	glutSetCursor(GLUT_CURSOR_CROSSHAIR);
	glShadeModel(GL_SMOOTH);
	glFrontFace(GL_CCW);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_LIGHTING);

	glClearColor(0.0, 0.0, 0.0, 1.0);
}
