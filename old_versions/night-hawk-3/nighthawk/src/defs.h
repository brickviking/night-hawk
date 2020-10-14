/***************************************************************************
****************************************************************************
****************************************************************************
* Nighthawk
* Copyright (C) 1996-2004 Jason Nunn et al
* Developed by Jason Nunn et al (see README for Acknowledgements)
*
* Project head and maintainer--
*   Eric Gillespie (The Viking)
*   Email: viking667@users.sourceforge.net
*   http://night-hawk.sourceforge.net
*
* Original Author--
*   Jason Nunn
*   http://jsno.leal.com.au
*
*   C/- Jason Nunn
*   PO Box 15
*   Birdwood SA 5234
*   Australia
*
* This software is Open Source GPL. Please read COPYING notice.
*
****************************************************************************
****************************************************************************
***************************************************************************/
extern "C" {
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <pwd.h>
#include <time.h>
#include <ctype.h>
#include <math.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <X11/Xlib.h>
#include <X11/XKBlib.h>
#include <GL/glut.h>
#include <GL/glx.h>
}
#include "../version.h"

#define PRIVATE static
#define PUBLIC

#define DATED    __DATE__
#define WELCOME \
"Nighthawk version " VERSION ", Copyright (C) 1996-2004 Jason Nunn et al\n\n"

#define STR_LABEL_LEN 40
#define STR_LEN       256

#define UPDATE_SPEED   20

#define X              0
#define Y              1
#define Z              2

#define R              0
#define G              1
#define B              2
#define A              3

#define FONT_SIZE      (128 - 32)

#define RAD2DEG(x) ((x * 180.0) / M_PI)
#define DEG2RAD(x) ((x * M_PI) / 180.0)

#define MAX_SHIPS      10

#define SCREEN_SIZE_X  320
#define SCREEN_SIZE_Y  200
#define SCREEN_HSIZE_X (SCREEN_SIZE_X >> 1)
#define SCREEN_HSIZE_Y (SCREEN_SIZE_Y >> 1)
#define SPRITE_SIZE_X  32
#define SPRITE_SIZE_Y  32
#define SPRITE_HSIZE_X (SPRITE_SIZE_X >> 1)
#define SPRITE_HSIZE_Y (SPRITE_SIZE_Y >> 1)

#define MOVE_DRADIUS   (MOVE_RADIUS << 1)

#define MAX_LASERS     24
#define MAX_DOORS      24
#define MAX_TRANSPORTS 8
#define MAX_DROIDS     65
#define MAX_MESSAGES   5

#define GENERAL_SPD    0.5

#define MAX_SCORE_TABLE 9

#define KEVENT_SIZE     128
#define KEYBOARD_EVENT  0
#define SPECIAL_EVENT   1
#define MOUSE_EVENT     2

#define KEY_SELECT        ' '
#define KEY_TRANSFER_MODE 't'
#define KEY_STATUS        's'
#define KEY_PAUSE         'p'
#define KEY_QUIT          'q'
#define KEY_CASCADE_MODE  'c'
#define KEY_FPS           'f'
#define KEY_REDUCED       'r'

typedef struct
{
  void (*bg)(void);
  void (*fg)(void);
} sequence_tab_t;

typedef struct
{
  char type;
  int  val;
  int  mouse_x;
  int  mouse_y;
} tkevent; 

typedef struct
{
  GLfloat r, g, b;
} colour_pixel_t;

typedef struct
{
  unsigned int size_x,
               size_y;
  GLuint       id;
} bm_t;

typedef struct
{
  char   bound;
  bm_t   bm;
} tflr_bm;

typedef struct
{
  char name[9];
  char highest_ship[16];
  int  score;
} tscore_table;

typedef struct
{
  char   *type;
  char   *name;
  int    entry;
  float  height;
  float  weight;
  char   *brain;
  int    armament;
  int    shielding;
  int    max_shielding;
  double speed;
  double attack_rate;
} tdroid_stats;

#define DROID_002 0   
#define DROID_108 1 
#define DROID_176 2 
#define DROID_261 3 
#define DROID_275 4 
#define DROID_355 5 
#define DROID_368 6 
#define DROID_423 7 
#define DROID_467 8 
#define DROID_489 9 
#define DROID_513 10
#define DROID_520 11
#define DROID_599 12
#define DROID_606 13
#define DROID_691 14
#define DROID_693 15 
#define DROID_719 16
#define DROID_720 17
#define DROID_766 18
#define DROID_799 19
#define DROID_805 20
#define DROID_810 21
#define DROID_820 22
#define DROID_899 23
#define DROID_933 24
#define DROID_949 25 
#define DROID_999 26

#define LASER_NONE      0
#define LASER_LINARITE  1
#define LASER_CROC_BENZ 2
#define LASER_UVAROVITE 3
#define LASER_TIGER_EYE 4

#include "tpower_bay.h"
#include "tdoor.h"
#include "tfloor.h"
#include "tentity.h"
#include "tlaser.h"
#include "tdroid.h"
#include "tparadroid.h"
#include "tdroid_1xx.h"
#include "tdroid_2xx.h"
#include "tdroid_3xx.h"
#include "tdroid_4xx.h"
#include "tdroid_5xx.h"
#include "tdroid_6xx.h"
#include "tdroid_7xx.h"
#include "tdroid_8xx.h"
#include "tdroid_9xx.h"
#include "tship.h"
