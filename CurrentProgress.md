
# Table of Contents

1.  [Problems with ned4](#org3594677)
    1.  [Failure saving file when directory doesn't exist](#org0cc8768)
    2.  [ned4 shows black squares in the place of active tiles when loading old maps, but not newly created ones with static door tiles added](#org302655e)
    3.  [ned4 doesn't seem to use the mouse, even though a pointer is visible](#orgc4a2172)
    4.  [Missing help screen (never coded)](#org2d6423f)
    5.  [No protection against level already being present](#org7df65f9)
2.  [Things that work with ned4](#org9919155)
    1.  [loading files from disk works](#org05567db)
    2.  [placing floor tiles works (anything selectable from the palette)](#org3d54491)
    3.  [the b (blank) key works, toggles palette and splash text](#org79e3e25)
    4.  [save file works](#orgebf5331)
3.  [nighthawk4 issues](#org0e1d918)
    1.  [Sound for `6xx/7xx/8xx_voice` plays too high for 11025, sounds more like 14,000.](#orgccf3ffb)
    2.  [Fleet comment rendered too wide. Max length is 12 chars??](#org34e9298)
    3.  [Sound of dead droid haunts us by playing](#org994f61d)
4.  [Stuff that works with nighthawk](#org200fe7a)
    1.  [Fleet support](#orge50943a)



<a id="org3594677"></a>

# Problems with ned4


<a id="org0cc8768"></a>

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


<a id="org302655e"></a>

## ned4 shows black squares in the place of active tiles when loading old maps, but not newly created ones with static door tiles added

-   Loading a game file (something.f) from previous versions of NightHawk displays black squares instead of doors or recharge ports, as they're active sprites and not tiles
-   ned3 loads the same file and displays all the relevant tiles, including active sprites (unconfirmed)
-   A file created with ned4 retains whatever tiles (including doors) were loaded. Static doors confuse things but are available as tiles. Animated doors are sprites.
    -   Should the doors be removed from placeable tiles in ned?


<a id="orgc4a2172"></a>

## ned4 doesn't seem to use the mouse, even though a pointer is visible

Only keyboard commands seem to be able to be used (Space, Enter, Save, Quit). Probably works as intended.


<a id="org2d6423f"></a>

## Missing help screen (never coded)

This could use the same code as for nighthawk's pause mode


<a id="org7df65f9"></a>

## No protection against level already being present

I can create a level, save it, quit the program, and create that level again. If I save the file, it overwrites the earlier file with no warning.
This may work as intended, but there's no "Are you sure" confirmation prompt.


<a id="org9919155"></a>

# Things that work with ned4


<a id="org05567db"></a>

## loading files from disk works

-   If level was saved using previous version of ned, doors and power chargers display only as black squares, because those are active sprites, so don't get loaded as tiles by ned4 [worksasimplemented]


<a id="org3d54491"></a>

## placing floor tiles works (anything selectable from the palette)

I shouldn't be able to place doors, but they're in the tiles.


<a id="org79e3e25"></a>

## the b (blank) key works, toggles palette and splash text


<a id="orgebf5331"></a>

## save file works

-   need to create directory **first**, or ned4 bombs


<a id="org0e1d918"></a>

# nighthawk4 issues


<a id="orgccf3ffb"></a>

## Sound for `6xx/7xx/8xx_voice` plays too high for 11025, sounds more like 14,000.

I'm rather wondering whether the sound was recorded at 8,000 and played back at 11025?


<a id="org34e9298"></a>

## Fleet comment rendered too wide. Max length is 12 chars??


<a id="org994f61d"></a>

## Sound of dead droid haunts us by playing

A droid notices me and calls out, I shoot it, it dies, but the warning sound continues to play until it's complete.
It's weird to hear "Disarm and disengage" when the droid's no longer around.


<a id="org200fe7a"></a>

# Stuff that works with nighthawk


<a id="orge50943a"></a>

## Fleet support

