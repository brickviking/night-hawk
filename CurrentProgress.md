
# Table of Contents

1.  [Problems with ned4](#org36a3caa)
    1.  [Failure saving file when directory doesn't exist](#orgfe32d2f)
    2.  [ned4 shows black squares in the place of active tiles when loading old maps, but not newly created ones with static door tiles added](#org7e1e08a)
    3.  [ned4 doesn't seem to use the mouse, even though a pointer is visible](#org36cc1b2)
2.  [Things that work with ned4](#org8c1c191)
    1.  [loading files from disk works](#org3d6e121)
    2.  [placing floor tiles works (anything selectable from the palette)](#orgd49185f)
    3.  [the b (blank) key works, toggles palette and splash text](#org4ba358c)
    4.  [save file works](#org5202601)
3.  [nighthawk4 issues](#orgc0799a8)
    1.  [Sound for 6xx/7xx/8xx<sub>voice</sub> plays too high for 11025, sounds more like 14,000.](#org985ad24)
    2.  [Fleet comment rendered too wide. Max length is 12 chars??](#org1394b92)
    3.  [Misprint when asking for a ship in a fleet](#org2c99aca)
    4.  [Segfault when loading fleet from -f switch](#org3387a0f)
    5.  [Wonky comment in misc.c](#orgffa9d21)
4.  [Stuff that works with nighthawk](#orgff76a7c)
    1.  [Fleet support](#org990efe8)



<a id="org36a3caa"></a>

# Problems with ned4


<a id="orgfe32d2f"></a>

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


<a id="org7e1e08a"></a>

## ned4 shows black squares in the place of active tiles when loading old maps, but not newly created ones with static door tiles added

-   Loading a game file (something.f) from previous versions of NightHawk displays black squares instead of doors or recharge ports, as they're active sprites and not tiles
-   ned3 loads the same file and displays all the relevant tiles, including active sprites
-   A file created with ned4 retains whatever tiles (including doors) were loaded. Static doors confuse things but are available as tiles. Animated doors are sprites.


<a id="org36cc1b2"></a>

## ned4 doesn't seem to use the mouse, even though a pointer is visible

-   Only keyboard commands seem to be able to be used (Space, Enter, Save, Quit)
-   Missing help screen (never coded)
    ++ could use the same code as for nighthawk's pause mode


<a id="org8c1c191"></a>

# Things that work with ned4


<a id="org3d6e121"></a>

## loading files from disk works

-   If level was saved using previous version of ned, doors and power chargers display only as black squares, because those are active sprites, so don't get loaded as tiles by ned4 [worksasimplemented]


<a id="orgd49185f"></a>

## placing floor tiles works (anything selectable from the palette)


<a id="org4ba358c"></a>

## the b (blank) key works, toggles palette and splash text


<a id="org5202601"></a>

## save file works

-   need to create directory **first**, or ned4 bombs


<a id="orgc0799a8"></a>

# nighthawk4 issues


<a id="org985ad24"></a>

## Sound for 6xx/7xx/8xx<sub>voice</sub> plays too high for 11025, sounds more like 14,000.

I'm rather wondering whether the sound was recorded at 8,000 and played back at 11025?


<a id="org1394b92"></a>

## Fleet comment rendered too wide. Max length is 12 chars??


<a id="org2c99aca"></a>

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


<a id="org3387a0f"></a>

## Segfault when loading fleet from -f switch


<a id="orgffa9d21"></a>

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


<a id="orgff76a7c"></a>

# Stuff that works with nighthawk


<a id="org990efe8"></a>

## Fleet support

-   Need to create better icons (xpm format)

