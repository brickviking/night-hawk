/**************************************************************************
* Nighthawk Options
**************************************************************************/

/*------------------------------------------------------------------------
--------------------------------------------------------------------------
You only need to configure theses directives.
--------------------------------------------------------------------------
--------------------------------------------------------------------------*/

/* Keys can be a letter, or XK_Up, XK_Down, XK_Left, XK_Right for arrows */
#define KEY_SELECT         ' '
#define KEY_TRANSFER_MODE  't'
#define KEY_HEADSUP        'h'
#define KEY_STATUS         's'
#define KEY_PAUSE          'p'
#define KEY_QUIT           'q'
#define KEY_GRAB           'g'
#define KEY_UP             XK_Up
#define KEY_DOWN           XK_Down
#define KEY_LEFT           XK_Left
#define KEY_RIGHT          XK_Right
#define KEY_DEMO           'd'
#define KEY_PPAGE          XK_Page_Up
#define KEY_NPAGE          XK_Page_Down
#define KEY_HOME           XK_Home
#define KEY_END            XK_End
#define KEY_KP_UP          XK_KP_Up
#define KEY_KP_DOWN        XK_KP_Down
#define KEY_KP_LEFT        XK_KP_Left
#define KEY_KP_RIGHT       XK_KP_Right
#define KEY_KP_HOME        XK_KP_Home
#define KEY_KP_END         XK_KP_End
#define KEY_KP_PPAGE       XK_KP_Page_Up
#define KEY_KP_NPAGE       XK_KP_Page_Down
#define KEY_KP_PLUS        XK_KP_Add
#define KEY_KP_MINUS       XK_KP_Subtract
#define KEY_KP_TRANSFER_MODE      XK_KP_Enter
#define KEY_KP_SELECT      XK_KP_Insert
#define KEY_KP_HEADSUP     XK_KP_Divide

/*
You don't really did to touch these,
unless i email you and tell you to.
*/
#define MAXIMUM_PATTERNS 128
#define MAXIMUM_CHANNELS 16

/*don't touch this unless you want to record a demo or edit levels.
#define DEVELOPMENT_MODE
*/

/*
another development thingy for testing
#define GOD_MODE 1
*/


/*------------------------------------------------------------------------
--------------------------------------------------------------------------
The rest is handled by configure.
I just left the stuff here for reference.
--------------------------------------------------------------------------
--------------------------------------------------------------------------*/

#if defined (HAVE_CONFIG_H)
#include <config.h>
#else  /* !defined (HAVE_CONFIG_H) */

/*********************************************************************
                     THIS IS NO LONGER USED HERE!
                   CONFIGURE WILL HANDLE IT THROUGH
                       Makefile.in AND config.h
*********************************************************************/

#define INSTALL_DIR "/home/jsno/proj/nighthawk"
#define SCORES_FILE "/usr/lib/games/nighthawk/nighthawk.scores"

/*
define this if you have a slow machine. this will reduce high intensive
sprites.
*/
#undef REDUCED_SPRITES

/*
sound options
*/
#define DEFAULT_SAM_RATE  11000
#define DEFAULT_PRECISION 8
#define DEFAULT_STEREO    0
#define NO_FX_CHANNELS    8

#endif /* !defined (HAVE_CONFIG_H) */
