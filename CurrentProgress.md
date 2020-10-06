
# Table of Contents

1.  [Problems with ned4](#orga98e19b)
    1.  [Failure saving file when directory doesn't exist](#org203afd0)
    2.  [ned4 shows black squares in the place of active tiles when loading old maps, but not newly created ones with static door tiles added](#orgfc16f68)
    3.  [ned4 doesn't seem to use the mouse, even though a pointer is visible](#org8dfa39b)
2.  [Things that work with ned4](#orgeec9bb8)
    1.  [loading files from disk works](#orgbfdacd2)
    2.  [placing floor tiles works (anything selectable from the palette)](#org358ab24)
    3.  [the b (blank) key works, toggles palette and splash text](#orgd31a406)
    4.  [save file works](#org6e4cc6a)
3.  [nighthawk4 issues](#org5cb066b)
    1.  [Sound for 6xx/7xx/8xx<sub>voice</sub> plays too high for 11025, sounds more like 14,000.](#org016e4a3)
    2.  [Fleet comment rendered too wide. Max length is 12 chars??](#org70c22ca)
    3.  [Misprint when asking for a ship in a fleet](#org4a4a382)
    4.  [Segfault when loading fleet from -f switch](#org4a120a5)
4.  [Stuff that works with nighthawk](#orgf2744e1)
    1.  [Fleet support](#org52044e8)



<a id="orga98e19b"></a>

# Problems with ned4


<a id="org203afd0"></a>

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


<a id="orgfc16f68"></a>

## ned4 shows black squares in the place of active tiles when loading old maps, but not newly created ones with static door tiles added

-   Loading a game file (something.f) from previous versions of NightHawk displays black squares instead of doors or recharge ports, as they're active sprites and not tiles
-   ned3 loads the same file and displays all the relevant tiles, including active sprites
-   A file created with ned4 retains whatever tiles (including doors) were loaded. Static doors confuse things but are available as tiles. Animated doors are sprites.


<a id="org8dfa39b"></a>

## ned4 doesn't seem to use the mouse, even though a pointer is visible

-   Only keyboard commands seem to be able to be used (Space, Enter, Save, Quit)
-   Missing help screen (never coded)
    ++ could use the same code as for nighthawk's pause mode


<a id="orgeec9bb8"></a>

# Things that work with ned4


<a id="orgbfdacd2"></a>

## loading files from disk works

-   If level was saved using previous version of ned, doors and power chargers display only as black squares, because those are active sprites, so don't get loaded as tiles by ned4 [worksasimplemented]


<a id="org358ab24"></a>

## placing floor tiles works (anything selectable from the palette)


<a id="orgd31a406"></a>

## the b (blank) key works, toggles palette and splash text


<a id="org6e4cc6a"></a>

## save file works

-   need to create directory **first**, or ned4 bombs


<a id="org5cb066b"></a>

# nighthawk4 issues


<a id="org016e4a3"></a>

## Sound for 6xx/7xx/8xx<sub>voice</sub> plays too high for 11025, sounds more like 14,000.

I'm rather wondering whether the sound was recorded at 8,000 and played back at 11025?


<a id="org70c22ca"></a>

## Fleet comment rendered too wide. Max length is 12 chars??


<a id="org4a4a382"></a>

## Misprint when asking for a ship in a fleet

nighthawk -d <datadir> -f Nighthawk -g -c Zaxon results in

    Finding Fleet Nighthawk
        Found !
    Finding fleet Zaxon.  <<-- should read "Finding ship Zaxon."
        Found ! (8)
    results in blank screen except for God Mode (if selected from commandline)

It's an easy fix:

    --- nighthawk-4.0/src/misc.c	2020-10-07 10:39:39.559699300 +1300
    +++ nighthawk-4.0/src/misc.c~	2020-10-04 19:53:55.165569756 +1000
    @@ -499,7 +499,7 @@
    	int i;
    
    	if (verbose_logging == TRUE)
    -		printf("Finding ship %s.\n", entry);
    +		printf("Finding fleet %s.\n", entry);
    
    	for (i = 0; *p != NULL; p++, i++) {
    		if (!strcmp(*p, entry)) {


<a id="org4a120a5"></a>

## Segfault when loading fleet from -f switch


<a id="orgf2744e1"></a>

# Stuff that works with nighthawk


<a id="org52044e8"></a>

## Fleet support

-   Need to create better icons (xpm format)

