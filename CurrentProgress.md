
# Table of Contents

1.  [Problems with ned4](#org840f040)
    1.  [Failure saving file when directory doesn't exist](#org1141ca5)
    2.  [ned4 shows black squares in the place of active tiles when loading old maps, but not newly created ones with static door tiles added](#orgc3d34f5)
    3.  [ned4 doesn't seem to use the mouse, even though a pointer is visible](#org7d35821)
    4.  [Missing help screen (never coded)](#org32ac081)
    5.  [Can't compile ned,](#org523bd38)
    6.  [No protection against level already being present](#orgc90c4ba)
2.  [Things that work with ned4](#org8c28dda)
    1.  [loading files from disk works](#org8f85b52)
    2.  [placing floor tiles works (anything selectable from the palette)](#org7ae411c)
    3.  [the b (blank) key works, toggles palette and splash text](#org1eaa3d5)
    4.  [save file works](#orgbda6a72)
3.  [nighthawk4 issues](#orgcfd6372)
    1.  [Sound for `6xx/7xx/8xx_voice` plays too high for 11025, sounds more like 14,000.](#orgb7d7dfc)
    2.  [Fleet comment rendered too wide. Max length is 12 chars??](#org3730665)
    3.  [Misprint when asking for a ship in a fleet](#org80734d2)
    4.  [Game locks up when specifying both a fleet and a ship](#org8ca8cea)
    5.  [Wonky comment in misc.c](#org95bc067)
4.  [Stuff that works with nighthawk](#org52e1906)
    1.  [Fleet support](#org5071241)



<a id="org840f040"></a>

# Problems with ned4


<a id="org1141ca5"></a>

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


<a id="orgc3d34f5"></a>

## ned4 shows black squares in the place of active tiles when loading old maps, but not newly created ones with static door tiles added

-   Loading a game file (something.f) from previous versions of NightHawk displays black squares instead of doors or recharge ports, as they're active sprites and not tiles
-   ned3 loads the same file and displays all the relevant tiles, including active sprites (unconfirmed)
-   A file created with ned4 retains whatever tiles (including doors) were loaded. Static doors confuse things but are available as tiles. Animated doors are sprites.
    -   Should the doors be removed from placeable tiles in ned?


<a id="org7d35821"></a>

## ned4 doesn't seem to use the mouse, even though a pointer is visible

Only keyboard commands seem to be able to be used (Space, Enter, Save, Quit). Probably works as intended.


<a id="org32ac081"></a>

## Missing help screen (never coded)

This could use the same code as for nighthawk's pause mode


<a id="org523bd38"></a>

## Can't compile ned,

Makefile doesn't include `png_loader.o` for `OBJS_NED`, causing a linking error; patch follows

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


<a id="orgc90c4ba"></a>

## No protection against level already being present

I can create a level, save it, quit the program, and create that level again. If I save the file, it overwrites the earlier file with no warning.
This may work as intended, but there's no "Are you sure" confirmation prompt.


<a id="org8c28dda"></a>

# Things that work with ned4


<a id="org8f85b52"></a>

## loading files from disk works

-   If level was saved using previous version of ned, doors and power chargers display only as black squares, because those are active sprites, so don't get loaded as tiles by ned4 [worksasimplemented]


<a id="org7ae411c"></a>

## placing floor tiles works (anything selectable from the palette)


<a id="org1eaa3d5"></a>

## the b (blank) key works, toggles palette and splash text


<a id="orgbda6a72"></a>

## save file works

-   need to create directory **first**, or ned4 bombs


<a id="orgcfd6372"></a>

# nighthawk4 issues


<a id="orgb7d7dfc"></a>

## Sound for `6xx/7xx/8xx_voice` plays too high for 11025, sounds more like 14,000.

I'm rather wondering whether the sound was recorded at 8,000 and played back at 11025?


<a id="org3730665"></a>

## Fleet comment rendered too wide. Max length is 12 chars??


<a id="org80734d2"></a>

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


<a id="org8ca8cea"></a>

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


<a id="org95bc067"></a>

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


<a id="org52e1906"></a>

# Stuff that works with nighthawk


<a id="org5071241"></a>

## Fleet support

