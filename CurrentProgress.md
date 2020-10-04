
# Table of Contents

1.  [Problems with ned4](#org938894c)
    1.  [Failure saving file when directory doesn't exist](#org535f674)
    2.  [ned4 shows black squares in the place of active tiles when loading old maps, but not newly created ones with static door tiles added](#orga8efc90)
    3.  [ned4 doesn't seem to use the mouse, even though a pointer is visible](#orgf570ec3)
2.  [Things that work with ned4](#org1009738)
    1.  [loading files from disk works](#org8e10f54)
    2.  [placing floor tiles works (anything selectable from the palette)](#org8ed30e8)
    3.  [the b (blank) key works, toggles palette and splash text](#org048d3da)
    4.  [save file works](#org1d67944)
3.  [nighthawk4 issues](#orge283299)
    1.  [Sound for 6xx/7xx/8xx<sub>voice</sub> plays too high for 11025, sounds more like 14,000.](#orga3d0ddc)
    2.  [Fleet comment rendered too wide. Max length is 12 chars??](#orgf51a8c9)
4.  [Stuff that works with nighthawk](#orgbd6e985)
    1.  [Fleet support](#org5a3a03b)



<a id="org938894c"></a>

# Problems with ned4


<a id="org535f674"></a>

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


<a id="orga8efc90"></a>

## ned4 shows black squares in the place of active tiles when loading old maps, but not newly created ones with static door tiles added

-   Loading a game file (something.f) from previous versions of NightHawk displays black squares instead of doors or recharge ports, as they're active sprites and not tiles
-   ned3 loads the same file and displays all the relevant tiles, including active sprites
-   A file created with ned4 retains whatever tiles (including doors) were loaded. Static doors confuse things but are available as tiles. Animated doors are sprites.


<a id="orgf570ec3"></a>

## ned4 doesn't seem to use the mouse, even though a pointer is visible

-   Only keyboard commands seem to be able to be used (Space, Enter, Save, Quit)
-   Missing help screen (never coded)
    ++ could use the same code as for nighthawk's pause mode


<a id="org1009738"></a>

# Things that work with ned4


<a id="org8e10f54"></a>

## loading files from disk works

-   If level was saved using previous version of ned, doors and power chargers display only as black squares, because those are active sprites, so don't get loaded as tiles by ned4 [worksasimplemented]


<a id="org8ed30e8"></a>

## placing floor tiles works (anything selectable from the palette)


<a id="org048d3da"></a>

## the b (blank) key works, toggles palette and splash text


<a id="org1d67944"></a>

## save file works

-   need to create directory **first**, or ned4 bombs


<a id="orge283299"></a>

# nighthawk4 issues


<a id="orga3d0ddc"></a>

## Sound for 6xx/7xx/8xx<sub>voice</sub> plays too high for 11025, sounds more like 14,000.

I'm rather wondering whether the sound was recorded at 8,000 and played back at 11025?


<a id="orgf51a8c9"></a>

## Fleet comment rendered too wide. Max length is 12 chars??


<a id="orgbd6e985"></a>

# Stuff that works with nighthawk


<a id="org5a3a03b"></a>

## Fleet support

-   Need to create better icons (xpm format)

