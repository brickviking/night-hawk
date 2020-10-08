
# Table of Contents

1.  [Problems with ned4](#org82f1a29)
    1.  [Failure saving file when directory doesn't exist](#org68cb8a9)
    2.  [ned4 shows black squares in the place of active tiles when loading old maps, but not newly created ones with static door tiles added](#org4157e91)
    3.  [ned4 doesn't seem to use the mouse, even though a pointer is visible](#org1c4fb5a)
2.  [Things that work with ned4](#org6b4acf0)
    1.  [loading files from disk works](#orgfbbfb1b)
    2.  [placing floor tiles works (anything selectable from the palette)](#orgcfebae1)
    3.  [the b (blank) key works, toggles palette and splash text](#org7732838)
    4.  [save file works](#orgc1728db)
3.  [nighthawk4 issues](#orgd74cfa7)
    1.  [Sound for 6xx/7xx/8xx<sub>voice</sub> plays too high for 11025, sounds more like 14,000.](#org0a2aa26)
    2.  [Fleet comment rendered too wide. Max length is 12 chars??](#orgd25528d)
    3.  [Misprint when asking for a ship in a fleet](#orgfb50405)
    4.  [Segfault when loading fleet from -f switch](#orge7f50c3)
    5.  [Wonky comment in misc.c](#org17c9d02)
4.  [Stuff that works with nighthawk](#org48e92c6)
    1.  [Fleet support](#org2da5e9c)



<a id="org82f1a29"></a>

# Problems with ned4


<a id="org68cb8a9"></a>

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


<a id="org4157e91"></a>

## ned4 shows black squares in the place of active tiles when loading old maps, but not newly created ones with static door tiles added

-   Loading a game file (something.f) from previous versions of NightHawk displays black squares instead of doors or recharge ports, as they're active sprites and not tiles
-   ned3 loads the same file and displays all the relevant tiles, including active sprites
-   A file created with ned4 retains whatever tiles (including doors) were loaded. Static doors confuse things but are available as tiles. Animated doors are sprites.


<a id="org1c4fb5a"></a>

## ned4 doesn't seem to use the mouse, even though a pointer is visible

-   Only keyboard commands seem to be able to be used (Space, Enter, Save, Quit)
-   Missing help screen (never coded)
    ++ could use the same code as for nighthawk's pause mode
-   Can't compile ned, patch follows:

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


<a id="org6b4acf0"></a>

# Things that work with ned4


<a id="orgfbbfb1b"></a>

## loading files from disk works

-   If level was saved using previous version of ned, doors and power chargers display only as black squares, because those are active sprites, so don't get loaded as tiles by ned4 [worksasimplemented]


<a id="orgcfebae1"></a>

## placing floor tiles works (anything selectable from the palette)


<a id="org7732838"></a>

## the b (blank) key works, toggles palette and splash text


<a id="orgc1728db"></a>

## save file works

-   need to create directory **first**, or ned4 bombs


<a id="orgd74cfa7"></a>

# nighthawk4 issues


<a id="org0a2aa26"></a>

## Sound for 6xx/7xx/8xx<sub>voice</sub> plays too high for 11025, sounds more like 14,000.

I'm rather wondering whether the sound was recorded at 8,000 and played back at 11025?


<a id="orgd25528d"></a>

## Fleet comment rendered too wide. Max length is 12 chars??


<a id="orgfb50405"></a>

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


<a id="orge7f50c3"></a>

## Segfault when loading fleet from -f switch


<a id="org17c9d02"></a>

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


<a id="org48e92c6"></a>

# Stuff that works with nighthawk


<a id="org2da5e9c"></a>

## Fleet support

-   Need to create better icons (xpm format)

