/***************************************************************************
****************************************************************************
****************************************************************************
*
* NightHawk - By Jason Nunn - Oct 96 (jsno@dayworld.net.au)
* Updated by Eric Gillespie (viking at users dot sourceforge dot net)
* FREEWARE.
*
****************************************************************************
****************************************************************************
***************************************************************************/
#include "defs.h"

#define MAX_SHIPS      10

#define UPDATE_DELAY   50000 
#define REALTIME_DELAY 50000

#define SCREEN_SIZE_X  320
#define SCREEN_SIZE_Y  200
#define SCREEN_HSIZE_X (SCREEN_SIZE_X >> 1)
#define SCREEN_HSIZE_Y (SCREEN_SIZE_Y >> 1)
#define SPRITE_SIZE_X  32
#define SPRITE_SIZE_Y  32
#define SPRITE_HSIZE_X (SPRITE_SIZE_X >> 1)
#define SPRITE_HSIZE_Y (SPRITE_SIZE_Y >> 1)

#define MOVE_DRADIUS   (MOVE_RADIUS << 1)

#ifdef REDUCED_SPRITES
  #define MAX_LASERS     8
#else
  #define MAX_LASERS     24
#endif
#define MAX_DOORS      24
#define MAX_TRANSPORTS 8
#define MAX_DROIDS     65
#define MAX_POWER      8
#define MAX_CONSOLES   32

#define POWER_BAY_SPD  8
#define DOOR_SPD       16
#define GENERAL_SPD    8

#define MAX_SCORE_TABLE 9
//#define GOD_MODE 1

typedef struct
{
  Pixmap pixmap;
  Pixmap mask;
  int    width;
  int    height;
} tbm;

typedef struct
{
  char bound;
  tbm  bm_ptr;
} tsprite;

typedef struct
{
  char name[9];
  char highest_ship[16];
  char time[12];
  int  score;
} tscore_table;

typedef struct
{
  char  *type;
  char  *name;
  int   entry;
  float height;
  float weight;
  char  *brain;
  int   armament;
  int   shielding;
  int   max_shielding;
  int   speed;
  int   attack_rate;
} tdroid_stats;

// keyboard events (used in ship.cc and object.cc)
#define KEVENT_SIZE 128
typedef struct
{
  char type;
  int  val;
  int  mouse_x;
  int  mouse_y;
} tkevent; 

typedef struct 
{
  int     demo_count;
  tkevent kevent;
} tkevent_buf;

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

/* Added by ECG to integrate mousegrabbing - got from uae xwin.c */
static int grabbed;
static Cursor blankCursor,  xhairCursor;
void nighthawk_mousegrab(void);

