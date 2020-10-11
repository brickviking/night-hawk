
# Table of Contents

1.  [Problems with ned4](#org88b4871)
    1.  [Failure saving file when directory doesn't exist](#org9c0c282)
    2.  [ned4 shows black squares in the place of active tiles when loading old maps, but not newly created ones with static door tiles added](#orgdabd061)
    3.  [ned4 doesn't seem to use the mouse, even though a pointer is visible](#org0f1fb55)
    4.  [Missing help screen (never coded)](#orgd9694eb)
    5.  [No protection against level already being present](#org935cade)
2.  [Things that work with ned4](#org3b31225)
    1.  [loading files from disk works](#org8ad78ab)
    2.  [placing floor tiles works (anything selectable from the palette)](#orgc7b95da)
    3.  [the b (blank) key works, toggles palette and splash text](#orgea1c0d9)
    4.  [save file works](#orge49cfe5)
3.  [nighthawk4 issues](#orgc11408a)
    1.  [Sound for `6xx/7xx/8xx_voice` plays too high for 11025, sounds more like 14,000.](#orga6cd2fd)
    2.  [Fleet comment rendered too wide. Max length is 12 chars??](#orge3a0aca)
    3.  [Wonky comment in misc.c](#org0b45ce4)
    4.  [Bug when completing game](#org0202664)
        1.  [I need to recompile with debugging enabled, set a breakpoint on game.cc:game<sub>finale</sub><sub>draw</sub>, and watch the good times dra&#x2026; roll.](#org05fad8e)
4.  [Stuff that works with nighthawk](#org85a8a3b)
    1.  [Fleet support](#org4b35b68)



<a id="org88b4871"></a>

# Problems with ned4


<a id="org9c0c282"></a>

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


<a id="orgdabd061"></a>

## ned4 shows black squares in the place of active tiles when loading old maps, but not newly created ones with static door tiles added

-   Loading a game file (something.f) from previous versions of NightHawk displays black squares instead of doors or recharge ports, as they're active sprites and not tiles
-   ned3 loads the same file and displays all the relevant tiles, including active sprites (unconfirmed)
-   A file created with ned4 retains whatever tiles (including doors) were loaded. Static doors confuse things but are available as tiles. Animated doors are sprites.
    -   Should the doors be removed from placeable tiles in ned?


<a id="org0f1fb55"></a>

## ned4 doesn't seem to use the mouse, even though a pointer is visible

Only keyboard commands seem to be able to be used (Space, Enter, Save, Quit). Probably works as intended.


<a id="orgd9694eb"></a>

## Missing help screen (never coded)

This could use the same code as for nighthawk's pause mode


<a id="org935cade"></a>

## No protection against level already being present

I can create a level, save it, quit the program, and create that level again. If I save the file, it overwrites the earlier file with no warning.
This may work as intended, but there's no "Are you sure" confirmation prompt.


<a id="org3b31225"></a>

# Things that work with ned4


<a id="org8ad78ab"></a>

## loading files from disk works

-   If level was saved using previous version of ned, doors and power chargers display only as black squares, because those are active sprites, so don't get loaded as tiles by ned4 [worksasimplemented]


<a id="orgc7b95da"></a>

## placing floor tiles works (anything selectable from the palette)

I shouldn't be able to place doors, but they're in the tiles.


<a id="orgea1c0d9"></a>

## the b (blank) key works, toggles palette and splash text


<a id="orge49cfe5"></a>

## save file works

-   need to create directory **first**, or ned4 bombs


<a id="orgc11408a"></a>

# nighthawk4 issues


<a id="orga6cd2fd"></a>

## Sound for `6xx/7xx/8xx_voice` plays too high for 11025, sounds more like 14,000.

I'm rather wondering whether the sound was recorded at 8,000 and played back at 11025?


<a id="orge3a0aca"></a>

## Fleet comment rendered too wide. Max length is 12 chars??


<a id="org0b45ce4"></a>

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


<a id="org0202664"></a>

## Bug when completing game

If I complete a whole fleet of ships, I get a congratulation message from game.cc:game<sub>finale</sub><sub>draw</sub>(). 
I also get "Press Space to continue", but the game appears to do nothing except spin up Loading scores a lot of times.


<a id="org05fad8e"></a>

### TODO I need to recompile with debugging enabled, set a breakpoint on game.cc:game<sub>finale</sub><sub>draw</sub>, and watch the good times dra&#x2026; roll.

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


<a id="org85a8a3b"></a>

# Stuff that works with nighthawk


<a id="org4b35b68"></a>

## Fleet support

