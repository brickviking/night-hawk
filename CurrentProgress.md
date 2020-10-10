
# Table of Contents

1.  [Problems with ned4](#orge487ca3)
    1.  [Failure saving file when directory doesn't exist](#org8d036f1)
    2.  [ned4 shows black squares in the place of active tiles when loading old maps, but not newly created ones with static door tiles added](#orgb095392)
    3.  [ned4 doesn't seem to use the mouse, even though a pointer is visible](#orged10177)
    4.  [Missing help screen (never coded)](#org37e2432)
    5.  [Can't compile ned,](#org35046ef)
    6.  [No protection against level already being present](#orge5de0c6)
2.  [Things that work with ned4](#org9dbb54d)
    1.  [loading files from disk works](#org299be23)
    2.  [placing floor tiles works (anything selectable from the palette)](#org10fbbec)
    3.  [the b (blank) key works, toggles palette and splash text](#org3077f0d)
    4.  [save file works](#org8166856)
3.  [nighthawk4 issues](#orga516223)
    1.  [Sound for `6xx/7xx/8xx_voice` plays too high for 11025, sounds more like 14,000.](#orgd09eba0)
    2.  [Fleet comment rendered too wide. Max length is 12 chars??](#org2d49cfd)
    3.  [Wonky comment in misc.c](#orga7b872a)
4.  [Stuff that works with nighthawk](#org062c35b)
    1.  [Fleet support](#org9c55f63)



<a id="orge487ca3"></a>

# Problems with ned4


<a id="org8d036f1"></a>

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


<a id="orgb095392"></a>

## ned4 shows black squares in the place of active tiles when loading old maps, but not newly created ones with static door tiles added

-   Loading a game file (something.f) from previous versions of NightHawk displays black squares instead of doors or recharge ports, as they're active sprites and not tiles
-   ned3 loads the same file and displays all the relevant tiles, including active sprites (unconfirmed)
-   A file created with ned4 retains whatever tiles (including doors) were loaded. Static doors confuse things but are available as tiles. Animated doors are sprites.
    -   Should the doors be removed from placeable tiles in ned?


<a id="orged10177"></a>

## ned4 doesn't seem to use the mouse, even though a pointer is visible

Only keyboard commands seem to be able to be used (Space, Enter, Save, Quit). Probably works as intended.


<a id="org37e2432"></a>

## Missing help screen (never coded)

This could use the same code as for nighthawk's pause mode


<a id="org35046ef"></a>

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


<a id="orge5de0c6"></a>

## No protection against level already being present

I can create a level, save it, quit the program, and create that level again. If I save the file, it overwrites the earlier file with no warning.
This may work as intended, but there's no "Are you sure" confirmation prompt.


<a id="org9dbb54d"></a>

# Things that work with ned4


<a id="org299be23"></a>

## loading files from disk works

-   If level was saved using previous version of ned, doors and power chargers display only as black squares, because those are active sprites, so don't get loaded as tiles by ned4 [worksasimplemented]


<a id="org10fbbec"></a>

## placing floor tiles works (anything selectable from the palette)


<a id="org3077f0d"></a>

## the b (blank) key works, toggles palette and splash text


<a id="org8166856"></a>

## save file works

-   need to create directory **first**, or ned4 bombs


<a id="orga516223"></a>

# nighthawk4 issues


<a id="orgd09eba0"></a>

## Sound for `6xx/7xx/8xx_voice` plays too high for 11025, sounds more like 14,000.

I'm rather wondering whether the sound was recorded at 8,000 and played back at 11025?


<a id="org2d49cfd"></a>

## Fleet comment rendered too wide. Max length is 12 chars??


<a id="orga7b872a"></a>

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


<a id="org062c35b"></a>

# Stuff that works with nighthawk


<a id="org9c55f63"></a>

## Fleet support

