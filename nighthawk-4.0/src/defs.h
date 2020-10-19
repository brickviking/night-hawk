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
* defs.h - Global definitions file
*
****************************************************************************
****************************************************************************
***************************************************************************/
/*
 * Switch off gcc's strncpy() check and g++'s snprintf() check. These
 * are not helping and identifying imaginary warnings :( (See
 * looney rant in MAINTAINERS.README). JN, 05OCT20
 */
#if __GNUC__ > 7
#pragma GCC diagnostic ignored "-Wstringop-truncation"
#pragma GCC diagnostic ignored "-Wformat-truncation="
#endif

#include <GL/glut.h>
#include <GL/glx.h>
#include "version.h"
#include "config.h"

#define DATED    __DATE__
#define WELCOME \
"Nighthawk v" VERSION ", Copyright (C) 1997 Jason Nunn, Adelaide South Australia.\n"

#define STR_LABEL_LEN 40
#define STR_LEN       256
#define STR_LEN_HUGE  1024

#define DISPLAY_DELAY	25 /*ms*/
#define CALC_DELAY      50000 /*us*/

#define TRUE		1
#define FALSE		0

#define X              0
#define Y              1
#define Z              2

#define R              0
#define G              1
#define B              2
#define A              3

/*
 * Font loader constants
 */
#define FONT_TABLE_X    16
#define FONT_TABLE_Y    8
#define FONT_SIZE_X     (256 / FONT_TABLE_X)
#define FONT_SIZE_Y     (256 / FONT_TABLE_Y)
#define FONT_SIZE	(FONT_TABLE_X * FONT_TABLE_Y)

#define RAD2DEG(x) ((x * 180.0) / M_PI)
#define DEG2RAD(x) ((x * M_PI) / 180.0)

/*
 * Don't change these defs. They are the original screen dimensions of
 * the game. OpenGL uses these to scale the OpenGL screen to other sizes.
 * JN
 */
#define SCR_ASPECT_43	(4.0 / 3.0)  /*JN, 04SEP20*/
#define SCR_ASPECT_43_TOLERANCE 0.01
#define WINDOW_SIZE_X 800  /*For screen size discovery. JN, 05SEP20*/
#define WINDOW_SIZE_Y 600
#define SCREEN_SIZE_X  320
#define SCREEN_SIZE_Y  200
#define SCREEN_HSIZE_X (SCREEN_SIZE_X >> 1)
#define SCREEN_HSIZE_Y (SCREEN_SIZE_Y >> 1)
#define SPRITE_SIZE_X  32
#define SPRITE_SIZE_Y  32
#define SPRITE_HSIZE_X (SPRITE_SIZE_X >> 1)
#define SPRITE_HSIZE_Y (SPRITE_SIZE_Y >> 1)
#define LASER_SIZE_X   15 /*Added some constants for laser.cc. JN, 02OCT20*/
#define LASER_SIZE_Y   15
#define EXPLODE_SIZE_X 40
#define EXPLODE_SIZE_Y 40

#define DOOR_VERTICAL	0
#define	DOOR_HORIZONTAL	1
#define DOOR_STATE_THRES 5
#define DOOR_ADJ_DIV	4.0
#define DOOR_ADJ_LIM	4.0

#define DROID_COL_BXL_X	18
#define DROID_COL_BXL_Y	18
#define COL_DROID_PER_SIZE	64

#define BLOCK_DETECT_DISTANCE 11
#define BLOCK_DETECT_WAIT 50 /*Changed from 100 to 50 JN, 14OCT20*/

#define MAX_LASERS     24
#define MAX_DOORS      24
#define MAX_TRANSPORTS 8
#define MAX_DROIDS     65
#define MAX_MESSAGES   5

#define POWER_BAY_SPD  8  /*Reverted to 2.2. JN, 04SEP20*/
#define DOOR_SPD       16
#define GENERAL_SPD    8

#define MAX_SCORE_TABLE 9

/*
 * KEVENT_SIZE changed from 128 to 64 as arrow keys aren't queued anymore
 * and keys are set for non-repeat. This buffer size must be in powers of 2.
 * JN, 15SEP20
 */
#define KEVENT_SIZE     64
#define KEYBOARD_EVENT  0
#define SPECIAL_EVENT   1
#define MOUSE_EVENT     2

#define KEY_SELECT		' '
#define KEY_TRANSFER_MODE	't'
#define KEY_TRANSFER_MODE_ALT	13  /*Eric's reference*/
#define KEY_STATUS		's'
#define KEY_PAUSE		'p'
#define KEY_HELP		'h'
#define KEY_QUIT		'q'
#define KEY_CASCADE_MODE	'c'  /*Rafael Laboissiere's cascade mode.*/
#define KEY_FPS			'f'
#define KEY_YES			'y'
#define KEY_NO			'n'
#define MOUSE_GRAB		'g' /*Eric's mouse grabber function*/

#define DIAGONAL_MOVE_CO	0.707 /*Inverse square root of 2 for diagonal moves*/

/*
 * Special keys states.
 * Used by special_h(), special_up_h(), and get_special_key_state().
 * New keyboard handling for OpenGL (since XLib's XkbSetAutoRepeatRate()
 * have XkbSetAutoRepeatRate() have been removed from v4.0. JN, 01SEP20
 */
#define SP_KEY_STATE_UP		0x01
#define SP_KEY_STATE_DOWN	0x02
#define SP_KEY_STATE_LEFT	0x04
#define SP_KEY_STATE_RIGHT	0x08

/*
 * This is based on Eric's diagonal code in v2.3. Because XLib support has been
 * removed, none of his original code has been transferred to this version.
 * JN, 01SEP20
 */
#define SP_KEY_STATE_UP_LF	0x10
#define SP_KEY_STATE_UP_RT	0x20
#define SP_KEY_STATE_DN_LF	0x40
#define SP_KEY_STATE_DN_RT	0x80

typedef struct
{
  void (*bg)(void);
  void (*fg)(void);
} sequence_tab_t;

typedef struct /* Input (keyboard, mouse etc), event queue.*/
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

#define SCORES_FILENAME		"/nighthawk.scores"
#define PRESERVE_FILENAME	"/.nighthawk.preserve"

#define SCORES_NAME_LEN		8       /*Added constants. JN,25AUG20*/
#define SCORES_FLEET_LEN	20
#define SCORES_HSHIP_LEN	20
#define SCORES_TIME_LEN		12

typedef struct {
	char		name[SCORES_NAME_LEN + 1];
	char		fleet[SCORES_HSHIP_LEN + 1];
	char		highest_ship[SCORES_HSHIP_LEN + 1];
	char		time[SCORES_TIME_LEN + 1];
	unsigned int	score;
} tscore_table;

typedef struct { /*New 4.0, JN 02OCT20*/
	char	name[SCORES_FLEET_LEN + 1];
	char	*comment;
	char	*scroll_story;
	bm_t	logo;
} fleet_st;

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
  float  speed;  // Changed from double to float. JN, 05SEP20
  int    attack_rate;  // Changed from double to int. JN, 05SEP20
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

/*
 * I got these names from a crystal chart I had hanging up in my Valley View
 * flat when I was writing V1.0 in Oct/Nov of 1997. JN 25AUG20
 */
#define LASER_NONE      0
#define LASER_LINARITE  1
#define LASER_CROC_BENZ 2
#define LASER_UVAROVITE 3
#define LASER_TIGER_EYE 4

/*
 * New for 4.0. For enhanced colision detection for doors and droids.
 * JN, 26SEP20
 */
typedef struct {
	int x;
	int y;
} col_droid_perimeter_st;

/*
 * New. OpenAL defs. JN 16SEP20
 */
#define MUSIC_FILEPATH		"/music/rescue_from_vega.ogg"
#define OV_READ_BLOCK_SIZE	4096
#define OGG_DATA_SIZE		(OV_READ_BLOCK_SIZE * 32)
#define MUSIC_IS_LITTLEENDIAN	0 /*0 = little endian*/
#define	MUSIC_16BIT_PRECISION	2 /*2 = 16 bit*/
#define	MUSIC_IS_SIGNED		1 /*1 = signed*/
#define	NO_OF_MUSIC_BUFFERS	4
#define NO_OF_FX_MIXED_SOURCES	16
#define SND_EVENT_SIZE		64 /* Must be in powers of 2*/

/*
 * For fine tuning. JN, 18SEP20
 */
#define FX_REFERENCE_DISTANCE	10.0   /*See ./tools/openal_atten_calcs.ods*/
#define FX_MAX_DISTANCE		1000.0 /* JN, 19SEP20*/
#define FX_ROLLOFF_FACTOR	0.2

#define SND_CMD_FX_PRI		'a'
#define SND_CMD_FX_PRI_STOP	'b'
#define SND_CMD_FX_MIXED	'c'
#define SND_CMD_FX_MIXED_STOP	'd'
#define SND_CMD_FX_SNOISE	'e'
#define SND_CMD_FX_SNOISE_STOP	'f'
#define SND_CMD_RANDOM		'g'
#define SND_CMD_PLAY_SONG	'h'
#define SND_CMD_STOP_SONG	'i'
#define SND_CMD_FX_STOP_ALL	'j'

#define MAX_FX_BUFFER_ENTRIES	32
#define FX_SELECT           0
#define FX_WEAPON_RELOAD    1
#define FX_SHIP_COMPLETE    2
#define FX_NO_WEAPON        3
#define FX_POWER_UP         4
#define FX_LOW_SHIELDS      5
#define FX_OPEN_DOOR        6
#define FX_EXPLOSION_1      7
#define FX_EXPLOSION_2      8
#define FX_DROID_HIT_1      9
#define FX_LASER_LINARITE   10
#define FX_LASER_CROC_BENZ  11
#define FX_LASER_UVAROVITE  12
#define FX_LASER_TIGER_EYE  13
#define FX_LASER_BUZZ       14
#define FX_DROID_EVASE      15
#define FX_6XX_VOICE        16
#define FX_7XX_VOICE        17
#define FX_8XX_VOICE        18
#define FX_TRANS_TERM       19
#define FX_RANDOM           20
#define FX_ALERT_SIREN      21
#define FX_SHIP_NOISE_START 22  /*ship noises*/
#define FX_SHIP_NOISE1      22
#define FX_SHIP_NOISE2      23
#define FX_SHIP_NOISE3      24
#define FX_SHIP_NOISE4      25
#define FX_SHIP_NOISE5      26
#define FX_SHIP_NOISE6      27
#define FX_SHIP_NOISE7      28
#define FX_DROID_HIT_2      29
#define FX_FLOOR_COMPLETE   30
#define FX_9XX_VOICE        31

/*
 * New. Sound event queue.
 * This is probably going to change to take advantage of OpenGL's powerful
 * features (ie: Setting FX's position, orientation, velocity etc).
 * As a 1st cut, I'll keep it the same at v2.2 as I'm learning OpenGL.
 * "baby little steps". JN, 14SEP20
 */
typedef struct {
	char	cmd;
	int	fx_no;
	float	vol;
	float	freq;
	int	rx, ry;   /*relative sound position to paradroid*/
} snd_event_st;
