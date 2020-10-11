
# Table of Contents

1.  [Problems with ned4](#orga66254b)
    1.  [Failure saving file when directory doesn't exist](#org81e3599)
    2.  [ned4 shows black squares in the place of active tiles when loading old maps, but not newly created ones with static door tiles added](#org3eaf9d5)
    3.  [ned4 doesn't seem to use the mouse, even though a pointer is visible](#orgc9b32a0)
    4.  [Missing help screen (never coded)](#org36c1b59)
    5.  [No protection against level already being present](#org0366fda)
2.  [Things that work with ned4](#org8753060)
    1.  [loading files from disk works](#org1001917)
    2.  [placing floor tiles works (anything selectable from the palette)](#org30dc51e)
    3.  [the b (blank) key works, toggles palette and splash text](#orga3aa115)
    4.  [save file works](#org83d6a70)
3.  [nighthawk4 issues](#org6355989)
    1.  [Sound for `6xx/7xx/8xx_voice` plays too high for 11025, sounds more like 14,000.](#org0091719)
    2.  [Fleet comment rendered too wide. Max length is 12 chars??](#org147d196)
    3.  [Wonky comment in misc.c](#org5b3bd99)
    4.  [Bug when completing game](#org697b780)
        1.  [I need to recompile with debugging enabled, set a breakpoint on `game.cc:game_finale_draw`, and watch the good times dra&#x2026; roll.](#org8bb395a)
4.  [Stuff that works with nighthawk](#org6df44d5)
    1.  [Fleet support](#org33a2618)



<a id="orga66254b"></a>

# Problems with ned4


<a id="org81e3599"></a>

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


<a id="org3eaf9d5"></a>

## ned4 shows black squares in the place of active tiles when loading old maps, but not newly created ones with static door tiles added

-   Loading a game file (something.f) from previous versions of NightHawk displays black squares instead of doors or recharge ports, as they're active sprites and not tiles
-   ned3 loads the same file and displays all the relevant tiles, including active sprites (unconfirmed)
-   A file created with ned4 retains whatever tiles (including doors) were loaded. Static doors confuse things but are available as tiles. Animated doors are sprites.
    -   Should the doors be removed from placeable tiles in ned?


<a id="orgc9b32a0"></a>

## ned4 doesn't seem to use the mouse, even though a pointer is visible

Only keyboard commands seem to be able to be used (Space, Enter, Save, Quit). Probably works as intended.


<a id="org36c1b59"></a>

## Missing help screen (never coded)

This could use the same code as for nighthawk's pause mode


<a id="org0366fda"></a>

## No protection against level already being present

I can create a level, save it, quit the program, and create that level again. If I save the file, it overwrites the earlier file with no warning.
This may work as intended, but there's no "Are you sure" confirmation prompt.


<a id="org8753060"></a>

# Things that work with ned4


<a id="org1001917"></a>

## loading files from disk works

-   If level was saved using previous version of ned, doors and power chargers display only as black squares, because those are active sprites, so don't get loaded as tiles by ned4 [worksasimplemented]


<a id="org30dc51e"></a>

## placing floor tiles works (anything selectable from the palette)

I shouldn't be able to place doors, but they're in the tiles.


<a id="orga3aa115"></a>

## the b (blank) key works, toggles palette and splash text


<a id="org83d6a70"></a>

## save file works

-   need to create directory **first**, or ned4 bombs


<a id="org6355989"></a>

# nighthawk4 issues


<a id="org0091719"></a>

## Sound for `6xx/7xx/8xx_voice` plays too high for 11025, sounds more like 14,000.

I'm rather wondering whether the sound was recorded at 8,000 and played back at 11025?


<a id="org147d196"></a>

## Fleet comment rendered too wide. Max length is 12 chars??


<a id="org5b3bd99"></a>

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


<a id="org697b780"></a>

## Bug when completing game

If I complete a whole fleet of ships, I get a congratulation message from game.cc:game<sub>finale</sub><sub>draw</sub>(). 
I also get "Press Space to continue", but the game appears to do nothing except spin up Loading scores a lot of times.


<a id="org8bb395a"></a>

### TODO I need to recompile with debugging enabled, set a breakpoint on `game.cc:game_finale_draw`, and watch the good times dra&#x2026; roll.

And something now seems wrong with the score table, with my name ending up in the last 7 slots of the table, for the exact same game.

    Loading scores file '/var/tmp/nighthawk.scores'.
    Name     Fleet                Highest Ship         Time            Score
    Wolfgang Nighthawk            Tobruk               -               10000
    Rafael   Nighthawk            Zaxon                -                5000
    viking   Schiamano            Complete!            11/Oct/2020      3760
    viking   Schiamano            Complete!            11/Oct/2020      3760
    viking   Schiamano            Complete!            11/Oct/2020      3760
    viking   Schiamano            Complete!            11/Oct/2020      3760
    viking   Schiamano            Complete!            11/Oct/2020      3760
    viking   Schiamano            Complete!            11/Oct/2020      3760
    viking   Schiamano            Complete!            11/Oct/2020      3760

In the verbose debug log, I get lots of the following lines:

    Loading scores file /var/tmp/nighthawk.scores,
    Saving scores file /var/tmp/nighthawk.scores.
    Loading scores file /var/tmp/nighthawk.scores,
    Saving scores file /var/tmp/nighthawk.scores.

My score should only end up being added once, not fill the remainder of the table.


<a id="org6df44d5"></a>

# Stuff that works with nighthawk


<a id="org33a2618"></a>

## Fleet support

