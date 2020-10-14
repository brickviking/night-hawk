
# Table of Contents

1.  [Problems with ned4](#org8502fbe)
    1.  [Failure saving file when directory doesn't exist](#orgd4f23b7)
    2.  [ned4 shows black squares in the place of active tiles when loading old maps, but not newly created ones with static door tiles added](#orgf126767)
    3.  [ned4 doesn't seem to use the mouse, even though a pointer is visible](#org2e86b59)
    4.  [Missing help screen (never coded)](#org6feac1d)
    5.  [No protection against level already being present](#org7de9ef2)
2.  [Things that work with ned4](#org50c18c6)
    1.  [loading files from disk works](#org055cdce)
    2.  [placing floor tiles works (anything selectable from the palette)](#orgd47bf23)
    3.  [the b (blank) key works, toggles palette and splash text](#org5cf5ad8)
    4.  [save file works](#org890c792)
3.  [nighthawk4 issues](#org122755a)
    1.  [Some sound issues](#org8e663fd)
        1.  [Sound for `6xx/7xx/8xx_voice` plays too high for 11025, sounds more like 14,000.](#orgbeedba4)
        2.  [Sound isn't multi-threaded?](#org47c3592)
    2.  [Fleet comment rendered too wide. Max length is 12 chars??](#org5286ed5)
4.  [Stuff that works with nighthawk](#org34ca329)
    1.  [Fleet support](#org7c0d538)



<a id="org8502fbe"></a>

# Problems with ned4


<a id="orgd4f23b7"></a>

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


<a id="orgf126767"></a>

## ned4 shows black squares in the place of active tiles when loading old maps, but not newly created ones with static door tiles added

-   Loading a game file (something.f) from previous versions of NightHawk displays black squares instead of doors or recharge ports, as they're active sprites and not tiles
-   ned3 loads the same file and displays all the relevant tiles, including active sprites (unconfirmed)
-   A file created with ned4 retains whatever tiles (including doors) were loaded. Static doors confuse things but are available as tiles. Animated doors are sprites.
    -   Should the doors be removed from placeable tiles in ned?


<a id="org2e86b59"></a>

## ned4 doesn't seem to use the mouse, even though a pointer is visible

Only keyboard commands seem to be able to be used (Space, Enter, Save, Quit). Probably works as intended.


<a id="org6feac1d"></a>

## Missing help screen (never coded)

This could use the same code as for nighthawk's pause mode


<a id="org7de9ef2"></a>

## No protection against level already being present

I can create a level, save it, quit the program, and create that level again. If I save the file, it overwrites the earlier file with no warning.
This may work as intended, but there's no "Are you sure" confirmation prompt.


<a id="org50c18c6"></a>

# Things that work with ned4


<a id="org055cdce"></a>

## loading files from disk works

-   If level was saved using previous version of ned, doors and power chargers display only as black squares, because those are active sprites, so don't get loaded as tiles by ned4 [worksasimplemented]


<a id="orgd47bf23"></a>

## placing floor tiles works (anything selectable from the palette)

I shouldn't be able to place doors, but they're in the tiles.


<a id="org5cf5ad8"></a>

## the b (blank) key works, toggles palette and splash text


<a id="org890c792"></a>

## save file works

-   need to create directory **first**, or ned4 bombs


<a id="org122755a"></a>

# nighthawk4 issues


<a id="org8e663fd"></a>

## Some sound issues


<a id="orgbeedba4"></a>

### Sound for `6xx/7xx/8xx_voice` plays too high for 11025, sounds more like 14,000.

I'm rather wondering whether the sound was recorded at 8,000 and played back at 11025?


<a id="org47c3592"></a>

### Sound isn't multi-threaded?

1.  Sound for theme tune disappears as soon as I hit "g" (grab)

    When I start up a game, I like to lock the screen if in windowed mode. If I lock while theme tune is playing, then
    theme tune stops, and the sound effect for "lock" is played.

2.  Sound of dead droid haunts us by playing

    A droid notices me and calls out, I shoot it, it dies, but the warning sound continues to play until it's complete.
    It's weird to hear "Disarm and disengage" when the droid's no longer around.


<a id="org5286ed5"></a>

## TODO Fleet comment rendered too wide. Max length is 12 chars??


<a id="org34ca329"></a>

# Stuff that works with nighthawk


<a id="org7c0d538"></a>

## Fleet support

