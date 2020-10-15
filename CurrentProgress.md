
# Table of Contents

1.  [Problems with ned4](#org8ec7c3e)
    1.  [Failure saving file when directory doesn't exist](#orge47712e)
    2.  [ned4 shows black squares in the place of active tiles when loading old maps, but not newly created ones with static door tiles added](#org269023b)
    3.  [ned4 doesn't seem to use the mouse, even though a pointer is visible](#orgcdf0e5e)
    4.  [Missing help screen (never coded)](#org0af3fe9)
    5.  [No protection against level already being present](#orga1511d5)
2.  [Things that work with ned4](#org57cf8f6)
    1.  [loading files from disk works](#org50ea2d8)
    2.  [placing floor tiles works (anything selectable from the palette)](#org101a8bb)
    3.  [the b (blank) key works, toggles palette and splash text](#org336765c)
    4.  [save file works](#org40bfa7f)
3.  [nighthawk4 issues](#org310cb5e)
    1.  [Some sound issues](#org1270a2a)
        1.  [Sound for `6xx/7xx/8xx_voice` plays too high for 11025, sounds more like 14,000.](#org619379e)
        2.  [Sound isn't multi-threaded?](#org48971a6)
    2.  [Fleet comment rendered too wide. Max length is 12 chars??](#orgf62e7fc)
    3.  [Graphics don't render in centre of maximized/fullscreen](#orgc72ce60)
4.  [Stuff that works with nighthawk](#org7a951c9)
    1.  [Fleet support](#orgb5e8ad1)



<a id="org8ec7c3e"></a>

# Problems with ned4


<a id="orge47712e"></a>

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


<a id="org269023b"></a>

## ned4 shows black squares in the place of active tiles when loading old maps, but not newly created ones with static door tiles added

-   Loading a game file (something.f) from previous versions of NightHawk displays black squares instead of doors or recharge ports, as they're active sprites and not tiles
-   ned3 loads the same file and displays all the relevant tiles, including active sprites (unconfirmed)
-   A file created with ned4 retains whatever tiles (including doors) were loaded. Static doors confuse things but are available as tiles. Animated doors are sprites.
    -   Should the doors be removed from placeable tiles in ned?


<a id="orgcdf0e5e"></a>

## ned4 doesn't seem to use the mouse, even though a pointer is visible

Only keyboard commands seem to be able to be used (Space, Enter, Save, Quit). Probably works as intended.


<a id="org0af3fe9"></a>

## Missing help screen (never coded)

This could use the same code as for nighthawk's pause mode


<a id="orga1511d5"></a>

## No protection against level already being present

I can create a level, save it, quit the program, and create that level again. If I save the file, it overwrites the earlier file with no warning.
This may work as intended, but there's no "Are you sure" confirmation prompt.


<a id="org57cf8f6"></a>

# Things that work with ned4


<a id="org50ea2d8"></a>

## loading files from disk works

-   If level was saved using previous version of ned, doors and power chargers display only as black squares, because those are active sprites, so don't get loaded as tiles by ned4 [worksasimplemented]


<a id="org101a8bb"></a>

## placing floor tiles works (anything selectable from the palette)

I shouldn't be able to place doors, but they're in the tiles.


<a id="org336765c"></a>

## the b (blank) key works, toggles palette and splash text


<a id="org40bfa7f"></a>

## save file works

-   need to create directory **first**, or ned4 bombs


<a id="org310cb5e"></a>

# nighthawk4 issues


<a id="org1270a2a"></a>

## Some sound issues


<a id="org619379e"></a>

### Sound for `6xx/7xx/8xx_voice` plays too high for 11025, sounds more like 14,000.

I'm rather wondering whether the sound was recorded at 8,000 and played back at 11025?


<a id="org48971a6"></a>

### Sound isn't multi-threaded?

1.  Sound for theme tune disappears as soon as I hit "g" (grab)

    When I start up a game, I like to lock the screen if in windowed mode. If I lock while theme tune is playing, then
    theme tune stops, and the sound effect for "lock" is played.

2.  Sound of dead droid haunts us by playing

    A droid notices me and calls out, I shoot it, it dies, but the warning sound continues to play until it's complete.
    It's weird to hear "Disarm and disengage" when the droid's no longer around.


<a id="orgf62e7fc"></a>

## TODO Fleet comment rendered too wide. Max length is 12 chars??


<a id="orgc72ce60"></a>

## Graphics don't render in centre of maximized/fullscreen

[Issue 11](<https://github.com/brickviking/night-hawk/issues/11>) the TL;DR is, viewport is tied
to bottom left of window, even if viewport doesn't fill the window.


<a id="org7a951c9"></a>

# Stuff that works with nighthawk


<a id="orgb5e8ad1"></a>

## Fleet support

