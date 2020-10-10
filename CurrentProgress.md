
# Table of Contents

1.  [Problems with ned4](#org302caed)
    1.  [Failure saving file when directory doesn't exist](#org62ad6c8)
    2.  [ned4 shows black squares in the place of active tiles when loading old maps, but not newly created ones with static door tiles added](#orgd8bd297)
    3.  [ned4 doesn't seem to use the mouse, even though a pointer is visible](#orgd822898)
    4.  [Missing help screen (never coded)](#org17c142e)
    5.  [Can't compile ned,](#org8815866)
    6.  [No protection against level already being present](#orgc3351ab)
2.  [Things that work with ned4](#org18299d2)
    1.  [loading files from disk works](#org68d9ba8)
    2.  [placing floor tiles works (anything selectable from the palette)](#org7a4e80d)
    3.  [the b (blank) key works, toggles palette and splash text](#org5694e19)
    4.  [save file works](#org9729cda)
3.  [nighthawk4 issues](#org26f1682)
    1.  [Sound for `6xx/7xx/8xx_voice` plays too high for 11025, sounds more like 14,000.](#org9d3fd4a)
    2.  [Fleet comment rendered too wide. Max length is 12 chars??](#org6ca6fc0)
    3.  [Wonky comment in misc.c](#orgd7bae29)
    4.  [Bug when completing game](#org3fcc940)
        1.  [I need to recompile with debugging enabled, set a breakpoint on game<sub>finale</sub><sub>draw</sub>, and watch the good times dra&#x2026; roll.](#orgc7bcd8e)
4.  [Stuff that works with nighthawk](#org19f3a75)
    1.  [Fleet support](#orga21ed13)



<a id="org302caed"></a>

# Problems with ned4


<a id="org62ad6c8"></a>

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


<a id="orgd8bd297"></a>

## ned4 shows black squares in the place of active tiles when loading old maps, but not newly created ones with static door tiles added

-   Loading a game file (something.f) from previous versions of NightHawk displays black squares instead of doors or recharge ports, as they're active sprites and not tiles
-   ned3 loads the same file and displays all the relevant tiles, including active sprites (unconfirmed)
-   A file created with ned4 retains whatever tiles (including doors) were loaded. Static doors confuse things but are available as tiles. Animated doors are sprites.
    -   Should the doors be removed from placeable tiles in ned?


<a id="orgd822898"></a>

## ned4 doesn't seem to use the mouse, even though a pointer is visible

Only keyboard commands seem to be able to be used (Space, Enter, Save, Quit). Probably works as intended.


<a id="org17c142e"></a>

## Missing help screen (never coded)

This could use the same code as for nighthawk's pause mode


<a id="org8815866"></a>

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


<a id="orgc3351ab"></a>

## No protection against level already being present

I can create a level, save it, quit the program, and create that level again. If I save the file, it overwrites the earlier file with no warning.
This may work as intended, but there's no "Are you sure" confirmation prompt.


<a id="org18299d2"></a>

# Things that work with ned4


<a id="org68d9ba8"></a>

## loading files from disk works

-   If level was saved using previous version of ned, doors and power chargers display only as black squares, because those are active sprites, so don't get loaded as tiles by ned4 [worksasimplemented]


<a id="org7a4e80d"></a>

## placing floor tiles works (anything selectable from the palette)


<a id="org5694e19"></a>

## the b (blank) key works, toggles palette and splash text


<a id="org9729cda"></a>

## save file works

-   need to create directory **first**, or ned4 bombs


<a id="org26f1682"></a>

# nighthawk4 issues


<a id="org9d3fd4a"></a>

## Sound for `6xx/7xx/8xx_voice` plays too high for 11025, sounds more like 14,000.

I'm rather wondering whether the sound was recorded at 8,000 and played back at 11025?


<a id="org6ca6fc0"></a>

## Fleet comment rendered too wide. Max length is 12 chars??


<a id="orgd7bae29"></a>

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


<a id="org3fcc940"></a>

## Bug when completing game

If I complete a whole fleet of ships, I get a congratulation message from game<sub>finale</sub><sub>draw</sub>(). I also get "Press Space to continue", but this does nothing.
In the verbose debug log, I get lots of the following lines:

    Loading scores file /var/tmp/nighthawk.scores,
    Saving scores file /var/tmp/nighthawk.scores.
    Loading scores file /var/tmp/nighthawk.scores,
    Saving scores file /var/tmp/nighthawk.scores.


<a id="orgc7bcd8e"></a>

### TODO I need to recompile with debugging enabled, set a breakpoint on game<sub>finale</sub><sub>draw</sub>, and watch the good times dra&#x2026; roll.


<a id="org19f3a75"></a>

# Stuff that works with nighthawk


<a id="orga21ed13"></a>

## Fleet support

