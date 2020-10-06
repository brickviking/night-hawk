
# Table of Contents

1.  [Problems with ned4](#org2db195f)
    1.  [Failure saving file when directory doesn't exist](#orgbf22a30)
    2.  [ned4 shows black squares in the place of active tiles when loading old maps, but not newly created ones with static door tiles added](#org344e9e2)
    3.  [ned4 doesn't seem to use the mouse, even though a pointer is visible](#orgb5baa36)
2.  [Things that work with ned4](#org93dffa4)
    1.  [loading files from disk works](#org18445cb)
    2.  [placing floor tiles works (anything selectable from the palette)](#org446df61)
    3.  [the b (blank) key works, toggles palette and splash text](#org751813d)
    4.  [save file works](#orgaf676fe)
3.  [nighthawk4 issues](#org2cc9d05)
    1.  [Sound for 6xx/7xx/8xx<sub>voice</sub> plays too high for 11025, sounds more like 14,000.](#orge6c8596)
    2.  [Fleet comment rendered too wide. Max length is 12 chars??](#org48995c6)
    3.  [Misprint when asking for a ship in a fleet](#orgf1c9e3b)
    4.  [Segfault when loading fleet from -f switch](#org60c8397)
4.  [Stuff that works with nighthawk](#org009393a)
    1.  [Fleet support](#org9140756)



<a id="org2db195f"></a>

# Problems with ned4


<a id="orgbf22a30"></a>

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


<a id="org344e9e2"></a>

## ned4 shows black squares in the place of active tiles when loading old maps, but not newly created ones with static door tiles added

-   Loading a game file (something.f) from previous versions of NightHawk displays black squares instead of doors or recharge ports, as they're active sprites and not tiles
-   ned3 loads the same file and displays all the relevant tiles, including active sprites
-   A file created with ned4 retains whatever tiles (including doors) were loaded. Static doors confuse things but are available as tiles. Animated doors are sprites.


<a id="orgb5baa36"></a>

## ned4 doesn't seem to use the mouse, even though a pointer is visible

-   Only keyboard commands seem to be able to be used (Space, Enter, Save, Quit)
-   Missing help screen (never coded)
    ++ could use the same code as for nighthawk's pause mode


<a id="org93dffa4"></a>

# Things that work with ned4


<a id="org18445cb"></a>

## loading files from disk works

-   If level was saved using previous version of ned, doors and power chargers display only as black squares, because those are active sprites, so don't get loaded as tiles by ned4 [worksasimplemented]


<a id="org446df61"></a>

## placing floor tiles works (anything selectable from the palette)


<a id="org751813d"></a>

## the b (blank) key works, toggles palette and splash text


<a id="orgaf676fe"></a>

## save file works

-   need to create directory **first**, or ned4 bombs


<a id="org2cc9d05"></a>

# nighthawk4 issues


<a id="orge6c8596"></a>

## Sound for 6xx/7xx/8xx<sub>voice</sub> plays too high for 11025, sounds more like 14,000.

I'm rather wondering whether the sound was recorded at 8,000 and played back at 11025?


<a id="org48995c6"></a>

## Fleet comment rendered too wide. Max length is 12 chars??


<a id="orgf1c9e3b"></a>

## Misprint when asking for a ship in a fleet

    nighthawk -d <datadir> -f Nighthawk -g -c Zaxon results in
      Finding Fleet Nighthawk
          Found !
      Finding fleet Zaxon.  <<-- should read "Finding ship Zaxon."
          Found ! (8)
      results in blank screen except for God Mode (if selected from commandline)


<a id="org60c8397"></a>

## Segfault when loading fleet from -f switch


<a id="org009393a"></a>

# Stuff that works with nighthawk


<a id="org9140756"></a>

## Fleet support

-   Need to create better icons (xpm format)

