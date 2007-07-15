/****************************************************************************
* Small Precludes OpenGL demo
*
* By Jason Nunn <jsno8192@yahoo.com.au>
* (C) Copyright 2000-2003
* Darwin, NT, Australia
* http://jsno.arafuraconnect.com.au
*
* =====================================================================
* misc stuff
*
****************************************************************************/
#include "defs.h"
#include "protos.p"

typedef struct _tex_ll_t
{
  GLuint           id;
  struct _tex_ll_t *next;
} tex_ll_t;

PUBLIC int       window_size_x = SCREEN_SIZE_X,
                 window_size_y = SCREEN_SIZE_Y;

PUBLIC void      (*draw_h_hook)(void);
PUBLIC void      (*calc_hook)(void);

PUBLIC GLfloat   black_pixel[1][3],
                 white_pixel[8][3],
                 blue_pixel[8][3],
                 yellow_pixel[1][3] = {{0.9375, 0.9375, 0.0}},
                 green_pixel[MAX_SCORE_TABLE][3],
                 red_pixel[MAX_SCORE_TABLE][3],
                 intro_haze[2][3] = {{0.0, 0.9, 0.0}, {0.0, 0.35, 0.0}},
                 purple_pixel[1][3] = {{0.7, 0.1, 8.0}};

PUBLIC GLfloat       bm_colour[4] = {1.0, 1.0, 1.0, 1.0},
                     bm_colour2[4] = {1.0, 1.0, 1.0, 0.6};

PUBLIC unsigned int  update_speed = UPDATE_SPEED;

PUBLIC char          *console_message[MAX_MESSAGES];
PUBLIC float         console_message_alpha_fade;

PRIVATE GLdouble     time_start;
PRIVATE tex_ll_t     *tex_ll;
PRIVATE int          fps_f;
PRIVATE Display      *x_display;
PRIVATE unsigned int xkb_delay, xkb_rate;

/****************************************************************************
*
****************************************************************************/
PUBLIC void ramp_colour(
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
PUBLIC GLdouble get_clock(void)
{
  struct timeval t;

  gettimeofday(&t,NULL);
  return ((GLdouble)t.tv_sec + (0.000001 * (GLdouble)t.tv_usec));
}

PUBLIC void sync_time_lapse(void)
{
  time_start = get_clock();
}

PUBLIC GLdouble get_time_lapse(void)
{
  return (get_clock() - time_start) * 32;
}

/****************************************************************************
*
****************************************************************************/
PUBLIC void blit_line(int x1, int y1, int x2, int y2)
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
PUBLIC void blit_rect(int x, int y, int size_x, int size_y)
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
PUBLIC void blit_rect_hollow(int x, int y, int size_x, int size_y)
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
*
****************************************************************************/
PUBLIC void blit_bm(bm_t *bm, int x, int y)
{
  int                  x1, y1;
  static const GLfloat tc[4][2] =
    {{0.0, 0.0}, {0.0, 1.0}, {1.0, 1.0}, {1.0, 0.0}};

  x1 = x + bm->size_x;
  y1 = y + bm->size_y;

  if(x1 < 0 || y1 < 0)
    return;
  if(x >= SCREEN_SIZE_X || y >= SCREEN_SIZE_Y)
    return;

  glBindTexture(GL_TEXTURE_2D, bm->id);
  glBegin(GL_POLYGON);
    glTexCoord2fv(tc[0]);
    glVertex2i(x, y);

    glTexCoord2fv(tc[1]);
    glVertex2i(x, y1);

    glTexCoord2fv(tc[2]);
    glVertex2i(x1, y1);

    glTexCoord2fv(tc[3]);
    glVertex2i(x1, y);
  glEnd();
}

/****************************************************************************
*
****************************************************************************/
PUBLIC void print_str_len(char *str, int l, int x, int y)
{
  int i;

  for(i = 0; i < l; i++)
  {
    if(str[i] < 32)
      continue;

    blit_bm(&(font_bm[(int)(str[i] - ' ')]), x, y);
    x += 8;
  }
}

PUBLIC void print_str(char *str, int x, int y)
{
  print_str_len(str, strlen(str), x, y);
}

PUBLIC void print_str_hazey(char *str, int x, int y, GLfloat *c1, GLfloat *c2)
{
  glColor3fv(c2);
  print_str_len(str, strlen(str), x - 1, y);
  print_str_len(str, strlen(str), x + 1, y);
  print_str_len(str, strlen(str), x, y - 1);
  print_str_len(str, strlen(str), x, y + 1);

  glColor3fv(c1);
  print_str_len(str, strlen(str), x, y);
}

/****************************************************************************
*
****************************************************************************/
PUBLIC void display_message(char *s, int y)
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
      print_str_len(s, l, SCREEN_HSIZE_X - ((l << 3) >> 1), y);
    y += 16;
    s = s1 + 1;
  }
}

/****************************************************************************
* tex management
****************************************************************************/
PUBLIC void append_tex(GLuint id)
{
  tex_ll_t *t;

  t = tex_ll;
  tex_ll = (tex_ll_t *)alloc_mem(sizeof(tex_ll_t));
  tex_ll->id = id;
  tex_ll->next = t;
}

PUBLIC void delete_tex(GLuint id)
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

PUBLIC void free_textures(void)
{
  tex_ll_t *p;

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
*
****************************************************************************/
PUBLIC void convert_2_tex(GLubyte *buf, bm_t *bm, int aligned_f)
{
  glGenTextures(1, &(bm->id));
  if(!bm->id)
  {
    fprintf(stderr, "convert_2_tex()::glGenTextures(): Error.\n");
    end_game(-1);
  }
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
PUBLIC void console_message_add(char *str)
{
  int x;

  if(console_message[0] != NULL)
    free(console_message[0]);

  for(x = 0; x < (MAX_MESSAGES - 1); x++)
    console_message[x] = console_message[x + 1];

  console_message[MAX_MESSAGES - 1] = (char *)alloc_mem(strlen(str) + 1);
  strcpy(console_message[MAX_MESSAGES - 1], str);
  console_message_alpha_fade = 0.8;
}

PUBLIC void console_message_display(void)
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

PUBLIC void console_message_free(void)
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
PRIVATE void on_exit_h(int code, void *arg)
{
  free_textures();

  if(!code)
    printf("See ya\n");
}

/*
 * have to have this as XkbSetAutoRepeatRate() doesn't work after exit 
 * called
 */
PUBLIC void end_game(int code)
{
  if(x_display != NULL)
  {
    printf("Resetting X keyboard.\n");
    XkbSetAutoRepeatRate(x_display, XkbUseCoreKbd, xkb_delay, xkb_rate);
  }

  on_exit_h(0, 0);

  exit(code);
}

/****************************************************************************
*
****************************************************************************/
PRIVATE inline int mouse_aspect(int v, int w, int r)
{
  return v * r / w;
}

PRIVATE void keyboard_h(unsigned char key, int x, int y)
{
  switch(key)
  {
    case 27:
      end_game(0);

    case KEY_FPS:
        fps_f ^= 1;
      break;

    case KEY_REDUCED:
        reduced_f ^= 1;
      break;

    default:
    {
      tkevent k;

      k.type    = KEYBOARD_EVENT;
      k.val     = key;
      k.mouse_x = mouse_aspect(x, window_size_x, 320);
      k.mouse_y = mouse_aspect(y, window_size_y, 200);
      put_kb_event(&k);
      break;
    }
  }
}

PRIVATE void special_h(int key, int x, int y)
{
  tkevent k;

  k.type    = SPECIAL_EVENT;
  k.val     = (unsigned int)key;
  k.mouse_x = mouse_aspect(x, window_size_x, 320);
  k.mouse_y = mouse_aspect(y, window_size_y, 200);
  put_kb_event(&k);
}

PRIVATE void mouse_h(int button, int state, int x, int y)
{
  tkevent k;

  if(!state)
    return;

  k.type    = MOUSE_EVENT;
  k.val     = button;
  k.mouse_x = mouse_aspect(x, window_size_x, 320);
  k.mouse_y = mouse_aspect(y, window_size_y, 200);
  put_kb_event(&k);
}

/****************************************************************************
*
****************************************************************************/
PRIVATE void timer_h(int value)
{
  glutPostRedisplay();
  glutTimerFunc(update_speed, timer_h, 1);
}

/****************************************************************************
*
****************************************************************************/
PRIVATE void resize_h(int w, int h)
{
  window_size_x = w;
  window_size_y = h;

/*Prevent a divide by zero*/
  if(h == 0)
    h = 1;

/*Set Viewport to window dimensions*/
  glViewport(0, 0, w, h);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

/*clipping volume*/
  glOrtho(0.0, SCREEN_SIZE_X, SCREEN_SIZE_Y, 0.0, 1.0, -1.0);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

/****************************************************************************
*
****************************************************************************/
PRIVATE void main_draw_h(void)
{
  static int    frame_count;
  static double old_clock;
  double        x;
  static char   str[40],
                *mess = "You fucking cheat !\n";

  if(fps_f)
  {
    if((frame_count % 50) == 0)
    {
      x = get_clock();
      sprintf(str, "%.2f fps", 50 / (x - old_clock));
      old_clock = x;
    }
    frame_count++;
  }

  glClear(GL_COLOR_BUFFER_BIT);

  glEnable(GL_BLEND);
  glEnable(GL_TEXTURE_2D);

    if(draw_h_hook != NULL)
      draw_h_hook();

  if(fps_f)
  {
    glColor4f(1.0, 1.0, 1.0, 0.5);
    print_str(str, 0, 184);
  }

  if(god_mode)
  {
    glColor4fv(bm_colour);
    display_message(mess, 50);
    display_message(mess, 120);
  }

  glDisable(GL_TEXTURE_2D);
  glDisable(GL_BLEND);

  glutSwapBuffers();

 /*
  * do calc
  */
  if(calc_hook != NULL)
    calc_hook();
  sync_time_lapse();
}

/****************************************************************************
*
****************************************************************************/
PUBLIC void opengl_init(int argc, char *argv[], int mode)
{
  int x, fs_f = 0;
// ECG: Commented out, as we're now calling it from within end_game()
//  on_exit(on_exit_h,0);
  glutInit(&argc, argv);
  glutInitDisplayMode(mode);

  window_size_x = 640;
  window_size_y = 480;

  for(x = 1; x < argc; x++)
  {
    if(argv[x][0] == '-')
    {
      switch(argv[x][1])
      {
        case '3':
          window_size_x = 320;
          window_size_y = 200;
          printf("320x200 mode.\n");
          break;

        case '6':
          window_size_x = 640;
          window_size_y = 480;
          printf("640x480 mode.\n");
          break;

        case '1':
          window_size_x = 1024;
          window_size_y = 768;
          printf("1024x768 mode.\n");
          break;

        case 'f':
          window_size_x = 320;
          window_size_y = 200;
          fs_f = 1;
          printf("Full screen mode.\n");
          break;
      }
    }
  }

  glutInitWindowSize(window_size_x, window_size_y);
  glutCreateWindow("Nighthawk " VERSION);

  glutKeyboardFunc(keyboard_h);
  glutSpecialFunc(special_h);
  glutMouseFunc(mouse_h);

  glutTimerFunc(update_speed, timer_h, 1);
  glutReshapeFunc(resize_h);   
  glutDisplayFunc(main_draw_h);

  if(fs_f)
    glutFullScreen();

  glutSetCursor(GLUT_CURSOR_CROSSHAIR);

  glShadeModel(GL_SMOOTH);
  glFrontFace(GL_CCW);
  glDepthFunc(GL_LEQUAL);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glDisable(GL_LIGHTING);

  glClearColor(0.0, 0.0, 0.0, 1.0);

  x_display = glXGetCurrentDisplay();
  if(x_display != NULL)
  {
    XkbGetAutoRepeatRate(x_display, XkbUseCoreKbd, &xkb_delay, &xkb_rate);
    XkbSetAutoRepeatRate(x_display, XkbUseCoreKbd, 200, 33);
  }
}
