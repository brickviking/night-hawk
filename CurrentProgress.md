
# Table of Contents

1.  [Problems with ned4](#orgb335d71)
    1.  [there's no -? switch, easily fixed](#orga6dfa1a)
    2.  [Failure saving file when directory doesn't exist](#org4db39d7)
    3.  [ned4 shows black squares in the place of active tiles when loading old maps, but not newly created ones](#org5dd74c4)
    4.  [ned4 doesn't seem to use the mouse, even though a pointer is visible](#org0df6ba7)
    5.  [ned4's "b" command sends ned into a tailspin, refuses to respond afterwards](#orga87e5d8)
2.  [Things that work with ned4](#org8998b30)
    1.  [loading files from disk works, unless previously saved from ned 1.0/2.x/3.0](#org191f435)
    2.  [placing floor tiles works (anything selectable from the palette)](#org0c79f39)
    3.  [save file works](#org40aef5c)



<a id="orgb335d71"></a>

# Problems with ned4


<a id="orga6dfa1a"></a>

## there's no -? switch, easily fixed

    --- nighthawk-4.0/src/ned.cc~	2020-09-28 01:22:41.000000000 +1000
    +++ nighthawk-4.0/src/ned.cc	2020-09-30 09:13:55.808026798 +1300
    @@ -125,6 +125,7 @@
    
    		switch (argv[i][1]) {
    			case 'h':
    +      case '?':
    			print_help_and_quit();
    
    			case 'u':


<a id="org4db39d7"></a>

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


<a id="org5dd74c4"></a>

## ned4 shows black squares in the place of active tiles when loading old maps, but not newly created ones

-   Loading a game file (something.f) from previous versions of NightHawk displays black squares instead of doors or recharge ports

<del>+ ned3 loads the same file and displays all the relevant tiles, including active tiles
+</del> A file created with ned4 retains whatever tiles (including doors) were loaded


<a id="org0df6ba7"></a>

## ned4 doesn't seem to use the mouse, even though a pointer is visible

-   Only keyboard commands seem to be able to be used (Space, Enter, Save, Quit)
-   Missing help screen (never coded)
    ++ could use the same code as for nighthawk's pause mode


<a id="orga87e5d8"></a>

## ned4's "b" command sends ned into a tailspin, refuses to respond afterwards


<a id="org8998b30"></a>

# Things that work with ned4


<a id="org191f435"></a>

## loading files from disk works, unless previously saved from ned 1.0/2.x/3.0

-   If level was saved using previous version of ned, doors and power chargers display only as black squares


<a id="org0c79f39"></a>

## placing floor tiles works (anything selectable from the palette)


<a id="org40aef5c"></a>

## save file works

-   need to create directory **first**

