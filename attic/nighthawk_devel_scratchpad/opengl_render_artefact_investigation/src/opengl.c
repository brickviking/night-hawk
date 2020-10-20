#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <signal.h>
#include <math.h>
#include <sys/time.h>
#include <GL/glut.h>
#include <GL/glx.h>
#include "defs.h"
#include "misc_externs.h"

double		display_delay = DISPLAY_DELAY;
/*
 * As default, set window size to insane for screen size discovery.
 */
int		window_size_x = SCREEN_INSANE_SIZE,
		window_size_y = SCREEN_INSANE_SIZE;

GLfloat white_pixel[8][3];

void		(*draw_hook)(void);

static int	reshape_window_fg; /*This is for window reshaping. JN, 05SEP20*/

/****************************************************************************
* Timing functions.
****************************************************************************/
static double get_clock(void)
{
	struct timeval t;

	gettimeofday(&t, NULL);
	return ((double)t.tv_sec + (0.000001 * (double)t.tv_usec));
}

/*
 * Replaced usleep() in main_draw_h(), as SIGALRM signal (used by background
 * timer) was breaking it. JN, 04SEP20
 */
static void display_wait(void)
{
	double s;
	s = get_clock();
	for (;;) {
		double d;
		d = get_clock() - s;
		if (d >= display_delay)
			return;
		usleep(1000);
	}
}

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
void blit_rect(int x, int y, int size_x, int size_y)
{
  size_x += x;
  size_y += y;

//  glDisable(GL_TEXTURE_2D);

  glBegin(GL_POLYGON);
    glVertex2i(x, y);
    glVertex2i(x, size_y);
    glVertex2i(size_x, size_y);
    glVertex2i(size_x, y);
  glEnd();

//  glEnable(GL_TEXTURE_2D);
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
	gluOrtho2D(0, SCREEN_SIZE_X, SCREEN_SIZE_Y, 0.0);

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
  static int	frame_count;

	/*
	 * Low priority functions (processed once a second)
	 * JN, 04SEP20
	 */
	if ((frame_count++ % 25) == 0) {
		/*
		 * glutReshapeWindow() does not work in the resize_h() context,
		 * So resize_h() tells the foreground code to run it. A bit
		 * hacky, but oh well. JN, 05SEP20
		 */
		if (reshape_window_fg == TRUE) {
			reshape_window_fg = FALSE;
			printf("Calling glutReshapeWindow()\n");
			glutReshapeWindow(window_size_x, window_size_y);
		}
	}

	glClear(GL_COLOR_BUFFER_BIT);

	/*
	 * Don't really need this, but just incase some engines get offended
	 * without it. JN, 02SEP20
	 */
//	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	glEnable(GL_BLEND);
//	glEnable(GL_TEXTURE_2D);

	if (draw_hook != NULL)
		draw_hook();

//	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
	glPopMatrix(); /*JN, 02SEP20*/

	glutSwapBuffers();
//	display_wait();  /*New. JN, 04SEP20*/
}

/****************************************************************************
*
****************************************************************************/
static void idle_h(void)
{
	glutPostRedisplay();
}

/****************************************************************************
* Reworked this function. JN, 04SEP20
****************************************************************************/
void opengl_init(int argc, char *argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(window_size_x, window_size_y);
	glutCreateWindow("OpenGL Glitch fault finding.");

	glutReshapeFunc(resize_h);
	glutDisplayFunc(draw_h);
	glutIdleFunc(idle_h);

	glutSetCursor(GLUT_CURSOR_CROSSHAIR);
	glShadeModel(GL_SMOOTH);
	glFrontFace(GL_CCW);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_LIGHTING);

	glClearColor(0.0, 0.0, 0.0, 1.0);
}
