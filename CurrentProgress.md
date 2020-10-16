
# Table of Contents

1.  [Problems with ned4](#org1e1859a)
    1.  [Failure saving file when directory doesn't exist](#orgfcfd9e9)
    2.  [ned4 shows black squares in the place of active tiles when loading old maps, but not newly created ones with static door tiles added](#org0d2d36f)
    3.  [ned4 doesn't seem to use the mouse, even though a pointer is visible](#org807d193)
    4.  [Missing help screen (never coded)](#orgdfbef51)
    5.  [No protection against level already being present](#org880c59e)
2.  [Things that work with ned4](#orgcc11eda)
    1.  [loading files from disk works](#org4cbf797)
    2.  [placing floor tiles works (anything selectable from the palette)](#org37b80d7)
    3.  [the b (blank) key works, toggles palette and splash text](#org3d34e42)
    4.  [save file works](#org3f2f1d8)
3.  [nighthawk4 issues](#org00d12ac)
    1.  [Some sound issues](#org7526d4e)
        1.  [Sound for `6xx/7xx/8xx_voice` plays too high for 11025, sounds more like 14,000.](#org06a59e2)
        2.  [Sound isn't multi-threaded?](#org4ab0457)
    2.  [Fleet comment rendered too wide. Max length is 12 chars??](#org035c508)
    3.  [Graphics don't render in centre of maximized/fullscreen](#org24da853)
    4.  [Doors have this strange "pulling" effect](#org11c336c)
    5.  [Several droid beats are going to have to be realigned for Haldeck](#orgfc403b7)
    6.  [If I switch away from game while static plays, static continues playing](#org065ad07)
4.  [Stuff that works with nighthawk](#orgdfb37e7)
    1.  [Fleet support](#org3aac8ea)
5.  [Stuff I tried](#org0b5621a)
    1.  [Changed `WINDOW_SCREEN_X/Y` and `SCREEN_SIZE_X/Y`](#org9421a79)
        1.  [The aspect ratio is off with the new figures for both Nighthawk and Ned.](#org45721e9)
    2.  [I should work on `eric_mouse.png`](#org8d504fb)
    3.  [I want to put the `M_LCK` logo to the left of the score.](#org557f03f)



<a id="org1e1859a"></a>

# Problems with ned4


<a id="orgfcfd9e9"></a>

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


<a id="org0d2d36f"></a>

## ned4 shows black squares in the place of active tiles when loading old maps, but not newly created ones with static door tiles added

-   Loading a game file (something.f) from previous versions of NightHawk displays black squares instead of doors or recharge ports, as they're active sprites and not tiles
-   ned3 loads the same file and displays all the relevant tiles, including active sprites (unconfirmed)
-   A file created with ned4 retains whatever tiles (including doors) were loaded. Static doors confuse things but are available as tiles. Animated doors are sprites.
    -   Should the doors be removed from placeable tiles in ned?


<a id="org807d193"></a>

## ned4 doesn't seem to use the mouse, even though a pointer is visible

Only keyboard commands seem to be able to be used (Space, Enter, Save, Quit). Probably works as intended.


<a id="orgdfbef51"></a>

## Missing help screen (never coded)

This could use the same code as for nighthawk's pause mode


<a id="org880c59e"></a>

## No protection against level already being present

I can create a level, save it, quit the program, and create that level again. If I save the file, it overwrites the earlier file with no warning.
This may work as intended, but there's no "Are you sure" confirmation prompt.


<a id="orgcc11eda"></a>

# Things that work with ned4


<a id="org4cbf797"></a>

## loading files from disk works

-   If level was saved using previous version of ned, doors and power chargers display only as black squares, because those are active sprites, so don't get loaded as tiles by ned4 [worksasimplemented]


<a id="org37b80d7"></a>

## placing floor tiles works (anything selectable from the palette)

I shouldn't be able to place doors, but they're in the tiles.


<a id="org3d34e42"></a>

## the b (blank) key works, toggles palette and splash text


<a id="org3f2f1d8"></a>

## save file works

-   need to create directory **first**, or ned4 bombs


<a id="org00d12ac"></a>

# nighthawk4 issues


<a id="org7526d4e"></a>

## Some sound issues


<a id="org06a59e2"></a>

### Sound for `6xx/7xx/8xx_voice` plays too high for 11025, sounds more like 14,000.

I'm rather wondering whether the sound was recorded at 8,000 and played back at 11025?


<a id="org4ab0457"></a>

### Sound isn't multi-threaded?

1.  Sound for theme tune disappears as soon as I hit "g" (grab)

    When I start up a game, I like to lock the screen if in windowed mode. If I lock while theme tune is playing, then
    theme tune stops, and the sound effect for "lock" is played.

2.  Sound of dead droid haunts us by playing

    A droid notices me and calls out, I shoot it, it dies, but the warning sound continues to play until it's complete.
    It's weird to hear "Disarm and disengage" when the droid's no longer around.


<a id="org035c508"></a>

## TODO Fleet comment rendered too wide. Max length is 12 chars??


<a id="org24da853"></a>

## Graphics don't render in centre of maximized/fullscreen

[Issue 11](<https://github.com/brickviking/night-hawk/issues/11>) the TL;DR is, viewport is tied
to bottom left of window, even if viewport doesn't fill the window.


<a id="org11c336c"></a>

## Doors have this strange "pulling" effect

-   If me (002) is next to a door, as it opens, it centres me on the door so I can go through.
-   The effect seems to be quite wide, and seems to happen if any part of the collision box for the
    droid collides with the collision box for the door.


<a id="orgfc403b7"></a>

## Several droid beats are going to have to be realigned for Haldeck

-   Lots of droids are banging into each other on Cargo (606E/423, 606W/108, 355/108)
-   Analyse all beats (perhaps pretty graphic here would do)


<a id="org065ad07"></a>

## If I switch away from game while static plays, static continues playing

All other sounds I've tried don't seem to have this glitch.


<a id="orgdfb37e7"></a>

# Stuff that works with nighthawk


<a id="org3aac8ea"></a>

## Fleet support


<a id="org0b5621a"></a>

# Stuff I tried


<a id="org9421a79"></a>

## Changed `WINDOW_SCREEN_X/Y` and `SCREEN_SIZE_X/Y`


<a id="org45721e9"></a>

### The aspect ratio is off with the new figures for both Nighthawk and Ned.

-   I changed `SCREEN_SIZE_X` to 640, `SCREEN_SIZE_Y` to 400, roughly doubling the rendered resolution.
    This should quadruple the number of visible tiles (twice in each direction). Great for ned (aside
    from the aspect ratio), not quite so good for nighthawk until I un-fullscreen the window in xmonad.
    Great everywhere else.

-   I also changed up the `WINDOW_SCREEN_X/Y` to 1600x900 from its previous 800x600.
    This makes scale look off until I move (reset) the ned or nighthawk window.

-   Scrolling text was stuck to the left hand side, remainder of objects were centred except static.
    Shots were off until I un-maximised the window (restoring its 4:3 aspect ratio) at which stage the
    tiles probably looked correct again.
-   If I reset the window, then the aspect ratio gets reset to the proper 4:3. 
    -   In xmonad I need to grab window with Win-LeftMouseButton and move it to restore the window

to normal aspect ratio.

-   At least for xmonad's "monocle" view, the game doesn't get a choice of what size it thinks it is.


<a id="org8d504fb"></a>

## DONE I should work on `eric_mouse.png`

-   I settled on "M LCK" instead of LOCKSCREEN or a padlock. Image is fine now, though I should probably
    reduce its width to match the rest of the font. Even better would be to put in three letters
    from the font itself, makes the job much easier.


<a id="org557f03f"></a>

## DONE I want to put the `M_LCK` logo to the left of the score.

    --- nighthawk-4.0/src/opengl.c~	2020-10-15 12:09:43.000000001 +1300
    +++ nighthawk-4.0/src/opengl.c	2020-10-16 17:29:44.313147659 +1300
    @@ -804,7 +804,8 @@
     static void draw_h(void)
     {
    	static char	str[STR_LABEL_LEN + 1],
    -			*mess = "You cheat!";
    +			*mess = "You cheat!",
    +      *game_win_lock = "M LCK";
    	static int	frame_count;
    
    	if (halt_game_f == TRUE)
    @@ -861,8 +862,9 @@
    		print_str(mess, 120, SCREEN_SIZE_Y - (FONT_SIZE_X + 2));
    	}
    	if (mouse_grabber_f == TRUE) {
    -		glColor4f(1.0, 1.0, 0.0, 0.3);
    -		blit_bm(&mouse_grabber_bm, 1, 200 - 33);
    +		glColor4f(0.0, 1.0, 0.0, 0.3);
    +/*		blit_bm(&mouse_grabber_bm, 1, 200 - 33); *//* ECG: 16 Oct 2020 */
    +		print_str(game_win_lock, 80, 0);
    	}
    
    	glDisable(GL_TEXTURE_2D);

