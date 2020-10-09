
# Table of Contents

1.  [Problems with ned4](#org9d87757)
    1.  [Failure saving file when directory doesn't exist](#org066702b)
    2.  [ned4 shows black squares in the place of active tiles when loading old maps, but not newly created ones with static door tiles added](#orgbe9cae4)
    3.  [ned4 doesn't seem to use the mouse, even though a pointer is visible](#org4dc08dd)
    4.  [Missing help screen (never coded)](#org633dfd9)
    5.  [Can't compile ned,](#org026e40d)
    6.  [No protection against level already being present](#orgeaf2883)
2.  [Things that work with ned4](#org12ffbcd)
    1.  [loading files from disk works](#org4492011)
    2.  [placing floor tiles works (anything selectable from the palette)](#org2c34d7f)
    3.  [the b (blank) key works, toggles palette and splash text](#org8eb965f)
    4.  [save file works](#org363749f)
3.  [nighthawk4 issues](#org2ba550c)
    1.  [Sound for 6xx/7xx/8xx<sub>voice</sub> plays too high for 11025, sounds more like 14,000.](#org9aa9eda)
    2.  [Fleet comment rendered too wide. Max length is 12 chars??](#org19544c0)
    3.  [Misprint when asking for a ship in a fleet](#org954089b)
    4.  [Segfault when loading fleet from -f switch](#org430b3d2)
    5.  [Game locks up when specifying both a fleet and a ship](#orgee121a9)
    6.  [Wonky comment in misc.c](#orgbd67f8d)
4.  [Stuff that works with nighthawk](#org1a12e8b)
    1.  [Fleet support](#org38c3e5f)



<a id="org9d87757"></a>

# Problems with ned4


<a id="org066702b"></a>

## Failure saving file when directory doesn't exist

-   Trying to save a file without having created the directory first coughs up the following error:
    
        Nighthawk version 4.0dev, Copyright (C) 1997 Jason Nunn, Adelaide South Australia.
              Data path: /home/viking/src/c++/night-hawk/nh4-candidate-27sep2020/data
             Score path: /var/tmp/nighthawk.scores
          Preserve path: /home/viking/.nighthawk.preserve
          Display Delay: 25 ms
        Initing the OpenGL graphics system..
        Loading floor sprites..
        Loading xpm: xpm/standard/flr_base_cyan.xpm
        ...
        Loading xpm: xpm/standard/flr_pattern4_cyan.xpm
        Loading sprites..
        Loading xpm: xpm/standard/droid_ani.xpm
        ...
        Loading xpm: xpm/v4/eric_the_mouse.xpm
        Loading font..
        Loading xpm: xpm/font/font.xpm
        Entering floor editor..
        Window resized from 800x600 to 800x600
        Adjusting OpenGL 2D mapping 800x600.
        Window resized from 800x600 to 1600x882
        Adjusting screen aspect to 4:3 1176x882.
        Regained visibility. Game resumed.
        Saving floor: /Palette/Tiles.f
        Error: save:fopen(map.f)- No such file or directory
        Freeing Textures.
        Shutting down the OpenAL sound system..
-   Creating the directory before running ned allows the file to be saved.


<a id="orgbe9cae4"></a>

## ned4 shows black squares in the place of active tiles when loading old maps, but not newly created ones with static door tiles added

-   Loading a game file (something.f) from previous versions of NightHawk displays black squares instead of doors or recharge ports, as they're active sprites and not tiles
-   ned3 loads the same file and displays all the relevant tiles, including active sprites (unconfirmed)
-   A file created with ned4 retains whatever tiles (including doors) were loaded. Static doors confuse things but are available as tiles. Animated doors are sprites.
    -   Should the doors be removed from placeable tiles in ned?


<a id="org4dc08dd"></a>

## ned4 doesn't seem to use the mouse, even though a pointer is visible

Only keyboard commands seem to be able to be used (Space, Enter, Save, Quit). Probably works as intended.


<a id="org633dfd9"></a>

## Missing help screen (never coded)

This could use the same code as for nighthawk's pause mode


<a id="org026e40d"></a>

## Can't compile ned,

Makefile doesn't include png<sub>loader.o</sub> for OBJS<sub>NED</sub>, causing a linking error; patch follows

    --- nighthawk-4.0/src/Makefile~	2020-10-09 10:53:21.339678785 +1300
    +++ nighthawk-4.0/src/Makefile	2020-10-09 10:24:29.798729231 +1300
    @@ -46,6 +46,7 @@
    	sprite_loader.o \
    	opengl.o \
    	xpm_loader.o \
    +	png_loader.o \
    	openal.o \
    	power_bay.o \
    	door.o \


<a id="orgeaf2883"></a>

## No protection against level already being present

I can create a level, save it, quit the program, and create that level again. If I save the file, it overwrites the earlier file with no warning.
This may work as intended, but there's no "Are you sure" confirmation prompt.


<a id="org12ffbcd"></a>

# Things that work with ned4


<a id="org4492011"></a>

## loading files from disk works

-   If level was saved using previous version of ned, doors and power chargers display only as black squares, because those are active sprites, so don't get loaded as tiles by ned4 [worksasimplemented]


<a id="org2c34d7f"></a>

## placing floor tiles works (anything selectable from the palette)


<a id="org8eb965f"></a>

## the b (blank) key works, toggles palette and splash text


<a id="org363749f"></a>

## save file works

-   need to create directory **first**, or ned4 bombs


<a id="org2ba550c"></a>

# nighthawk4 issues


<a id="org9aa9eda"></a>

## Sound for 6xx/7xx/8xx<sub>voice</sub> plays too high for 11025, sounds more like 14,000.

I'm rather wondering whether the sound was recorded at 8,000 and played back at 11025?


<a id="org19544c0"></a>

## Fleet comment rendered too wide. Max length is 12 chars??


<a id="org954089b"></a>

## Misprint when asking for a ship in a fleet

nighthawk -d <datadir> -f Nighthawk -g -c Zaxon results in

    Finding Fleet Nighthawk
        Found !
    Finding fleet Zaxon.  <<-- should read "Finding ship Zaxon."
        Found ! (8)
    results in blank screen except for God Mode (if selected from commandline)

It's an easy fix:

    --- nighthawk-4.0/src/misc.c~	2020-10-04 19:53:55.165569756 +1000
    +++ nighthawk-4.0/src/misc.c	2020-10-07 10:39:39.559699300 +1300
    @@ -499,7 +499,7 @@
    	int i;
    
    	if (verbose_logging == TRUE)
    -		printf("Finding fleet %s.\n", entry);
    +		printf("Finding ship %s.\n", entry);
    
    	for (i = 0; *p != NULL; p++, i++) {
    		if (!strcmp(*p, entry)) {


<a id="org430b3d2"></a>

## Segfault when loading fleet from -f switch

Game loads ship list, then dies some time after that with a segfault.
0x000000000041462a in `game_cont_draw` () at intro.cc:257

    247  static void game_cont_draw(void)
    248  {
    249  	const char *mess =
    250  		"You have already played Nighthawk.\n"
    251  		"Would you like to continue from\n"
    252  		"where you left ? (y/n)\n";
    253  	char str[STR_LEN + 1];
    254  
    255  	glColor4f(0.0, 0.7, 0.0, game_cont_fade);
    256  	display_message((char *)mess, 20);
    257  	snprintf(str, STR_LEN,        /* Error is here, STR_LEN = 256 */
    258  			"Name: %s\n"
    259  			"Ship completed: %s\n"
    260  			"Score: %u\n"
    261  			"Date: %s\n",
    262  			preserved_session->name,
    263  			preserved_session->highest_ship,
    264  			preserved_session->score,
    265  			preserved_session->time);
    266  	glColor4f(0.0, 1.0, 0.0, game_cont_fade);
    267  	display_message(str, 80);
    268  
    ...


<a id="orgee121a9"></a>

## Game locks up when specifying both a fleet and a ship

    bash$ nighthawk -v -d /home/myuser/nighthawk/data -f Nighthawk -c Haldeck
    AL lib: (WW) alGetError: Querying error state on null context (implicitly 0xa004)
    Nighthawk v4.0dev, Copyright (C) 1997 Jason Nunn, Adelaide South Australia.
    Selected fleet 'Nighthawk'.
    Starting at ship 'Haldeck'. (Nb/ High scores will indicate that you have cheated)
          Data path: /home/viking/src/nighthawk/data
         Score path: /var/tmp/nighthawk.scores
      Preserve path: /home/viking/.nighthawk.preserve
      Display Delay: 25 ms
    Loading fleet List.
    	Parsing fleet: Nighthawk
    	Parsing fleet: Schiamano
    1. Nighthawk
    2. Schiamano
    Loading scores file '/var/tmp/nighthawk.scores'.
    Name     Fleet                Highest Ship         Time            Score
    viking   Nighthawk            Seafarer              4/Oct/2020      3852
    viking   Seafarer             p/2020               fgang               0
    			      king                                     0
            afael                                                  1634030336
    farer                                              ery                 0
    				    ck                   0            1768647030
    ng       eck                  20                                       0
    	 020                  Esperence            -                 500
    Tanel    Nighthawk            Anoyle               -                 300
    Initing the OpenGL graphics system..
    Loading fleet logos..
    Loading png: /Nighthawk/logo.png
    Loading png: /Schiamano/logo.png
    Loading floor sprites..
    Loading xpm: xpm/standard/flr_base_cyan.xpm
    Loading xpm: xpm/standard/flr_crosses_cyan.xpm
    Loading xpm: xpm/standard/flr_door_h_cyan.xpm
    Loading xpm: xpm/standard/flr_door_v_cyan.xpm
    Loading xpm: xpm/standard/flr_sqrs_cyan.xpm
    Loading xpm: xpm/standard/flr_wall_bj_cyan.xpm
    Loading xpm: xpm/standard/flr_wall_bl_cyan.xpm
    Loading xpm: xpm/standard/flr_wall_br_cyan.xpm
    Loading xpm: xpm/standard/flr_wall_h_cyan.xpm
    Loading xpm: xpm/standard/flr_wall_lj_cyan.xpm
    Loading xpm: xpm/standard/flr_wall_rj_cyan.xpm
    Loading xpm: xpm/standard/flr_wall_tj_cyan.xpm
    Loading xpm: xpm/standard/flr_wall_tl_cyan.xpm
    Loading xpm: xpm/standard/flr_wall_tr_cyan.xpm
    Loading xpm: xpm/standard/flr_wall_v_cyan.xpm
    Loading xpm: xpm/standard/flr_elev_cyan.xpm
    Loading xpm: xpm/standard/flr_con_b_cyan.xpm
    Loading xpm: xpm/standard/flr_con_r_cyan.xpm
    Loading xpm: xpm/standard/flr_store1_cyan.xpm
    Loading xpm: xpm/standard/flr_con_l_cyan.xpm
    Loading xpm: xpm/standard/flr_con_t_cyan.xpm
    Loading xpm: xpm/standard/nothing.xpm
    Loading xpm: xpm/standard/flr_chair_l_cyan.xpm
    Loading xpm: xpm/standard/flr_dashes_cyan.xpm
    Loading xpm: xpm/standard/flr_pattern1_cyan.xpm
    Loading xpm: xpm/standard/flr_pattern2_cyan.xpm
    Loading xpm: xpm/standard/flr_pattern3_cyan.xpm
    Loading xpm: xpm/standard/flr_pattern4_cyan.xpm
    Loading sprites..
    Loading xpm: xpm/standard/droid_ani.xpm
    Loading xpm: xpm/standard/paradroid_ani.xpm
    Loading xpm: xpm/standard/laser_l.xpm
    Loading xpm: xpm/standard/laser_cb.xpm
    Loading xpm: xpm/standard/laser_uv.xpm
    Loading xpm: xpm/standard/laser_te.xpm
    Loading xpm: xpm/standard/explosion.xpm
    Loading xpm: xpm/standard/power_bay.xpm
    Loading png: xpm/standard/ntitle.png
    Loading xpm: xpm/standard/flr_door_h_cyan_ani.xpm
    Loading xpm: xpm/standard/flr_door_v_cyan_ani.xpm
    Loading xpm: xpm/standard/digits.xpm
    Loading png: xpm/standard/ship_complete.png
    Loading png: xpm/standard/credit_6.png
    PNG file not long aligned. Scaling.
    Loading xpm: xpm/standard/neg.xpm
    Loading png: xpm/standard/trans_terminated.png
    PNG file not long aligned. Scaling.
    Loading png: xpm/standard/docking_to_ship.png
    PNG file not long aligned. Scaling.
    Loading png: xpm/standard/intro_back_1.png
    Loading png: xpm/standard/intro_back_2.png
    Loading png: xpm/standard/intro_back_3.png
    Loading png: xpm/standard/intro_back_4.png
    Loading png: xpm/v4/eric_the_mouse.png
    Loading font..
    Loading xpm: xpm/font/font.xpm
    Initing the OpenAL sound system..
    Loading RAW FX file: /fx/select.8
    Loading RAW FX file: /fx/weapon_reload.8
    Loading RAW FX file: /fx/ship_complete.8
    Loading RAW FX file: /fx/no_weapon.8
    Loading RAW FX file: /fx/power_up.8
    Loading RAW FX file: /fx/low_shields.8
    Loading RAW FX file: /fx/door_open.8
    Loading RAW FX file: /fx/explosion_1.8
    LoaAL lib: (EE) alc_cleanup: 1 device not closed
    ding RAW FX file: /fx/explosion_2.8
    Loading RAW FX file: /fx/droid_hit_1.8
    Loading RAW FX file: /fx/laser_linarite.8
    Loading RAW FX file: /fx/laser_crocoite_benzol.8
    Loading RAW FX file: /fx/laser_uvarovite.8
    Loading RAW FX file: /fx/laser_tiger_eye.8
    Loading RAW FX file: /fx/laser_buzz.8
    Loading RAW FX file: /fx/droid_evase.8
    Loading RAW FX file: /fx/6xx_voice.8
    Loading RAW FX file: /fx/7xx_voice.8
    Loading RAW FX file: /fx/8xx_voice.8
    Loading RAW FX file: /fx/trans_terminated.8
    Loading RAW FX file: /fx/rand.8
    Loading RAW FX file: /fx/alert_siren.8
    Loading RAW FX file: /fx/snoise1.raw
    Loading RAW FX file: /fx/snoise2.raw
    Loading RAW FX file: /fx/snoise3.raw
    Loading RAW FX file: /fx/snoise4.raw
    Loading RAW FX file: /fx/snoise5.raw
    Loading RAW FX file: /fx/snoise6.raw
    Loading RAW FX file: /fx/snoise7_science.raw
    Loading RAW FX file: /fx/droid_hit_2.8
    Loading RAW FX file: /fx/floor_complete.8
    Loading RAW FX file: /fx/9xx_voice.8
    Running intro_start_init().
    Fleet specified. Skipping fleet selection screen.
    Finding Fleet Nighthawk.
    	Found !
    Running reset_game().
    Loading ship list.
    1. Haldeck
    2. Seafarer
    3. Anoyle
    4. Esperence
    5. Ophukus
    6. Mearkat
    7. Friendship
    8. Discovery
    9. Zaxon
    10. Tobruk
    Finding fleet Haldeck.
    	Found ! (0)
    Entering Nighthawk..
    Window resized from 800x600 to 800x600
    Adjusting OpenGL 2D mapping 800x600.
    Window resized from 800x600 to 1600x882
    Adjusting screen aspect to 4:3 1176x882.
    Regained visibility. Game resumed.
    Window resized from 1176x882 to 1596x878
    Adjusting screen aspect to 4:3 1171x878.
    Window resized from 1170x878 to 1170x878
    Adjusting OpenGL 2D mapping 1170x878.


<a id="orgbd67f8d"></a>

## Wonky comment in misc.c

Comment for `test_mouse_pos` is duplicated from `find_fleet_entry()`

    --- nighthawk-4.0/src/misc.c~	2020-10-03 21:34:01.000000000 +1300
    +++ nighthawk-4.0/src/misc.c	2020-10-07 12:11:22.011444416 +1300
    @@ -640,7 +640,7 @@
     }
    
     /***************************************************************************
    -* New for 4.0. Find entry fleet table. set fleet_ptr. JN, 02OCT20.
    +* New for 4.0. Test if mouse pointer is within game boundaries. JN, 02OCT20.
     ***************************************************************************/
     int test_mouse_pos(tkevent *k, int x, int y, int w, int h)
     {


<a id="org1a12e8b"></a>

# Stuff that works with nighthawk


<a id="org38c3e5f"></a>

## Fleet support

-   Need to create better icons (xpm format)

