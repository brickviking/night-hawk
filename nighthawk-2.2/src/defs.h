/***************************************************************************
****************************************************************************
****************************************************************************
*
* NightHawk - By Jason Nunn - Oct 96 (jsno@arafura.net.au)
* Updated by Eric Gillespie (viking at users dot sourceforge dot net)
* FREEWARE.
*
****************************************************************************
****************************************************************************
***************************************************************************/
#include "../options.h"
#include "../src_sound/nighthawk_sound.h"

#define DATED    __DATE__
#define WELCOME \
"Nighthawk version " VERSION " (" DATED "), Copyright (C) 97,98,1999 Jason Nunn.\nEric Gillespie 2003\n"

#ifndef LIBEXEC_DIR
#define LIBEXEC_DIR "/usr/local/libexec"
#endif /* ! LIBEXEC_DIR */

#define FUNKPLAY_PATH LIBEXEC_DIR "/nighthawk_sound"

#define STR_LABEL_LEN 40
#define STR_LEN       256
