/***************************************************************************
 * -*- mode: C++; tab-width: 2 -*-
 ****************************************************************************
 ****************************************************************************
 *
 * NightHawk - By Jason Nunn - Oct 1996  (jsno at arafura dot net dot au)
 * Eric Gillespie and others - Jan 2003  (viking667 at users dot sourceforge dot net)
 * CopyRight GPL version 2, 2003.
 *
 * Xwindows (C++)
 *
 * ==================================================================
 * The Floor Editor
 *
 ****************************************************************************
 ****************************************************************************
 ***************************************************************************/
extern "C" {
#include <stdio.h>
#include <string.h>
#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/xpm.h>
#include <X11/keysym.h>
}
#include "nighthawk_defs.h"
#include "misc.h"
#include "tedit_floor.h"

tedit_floor edit_floor;
int         end_game = 0;

/***************************************************************************
 *
 ***************************************************************************/
void fg_loop(void)
{
  edit_floor.draw();
  XCopyArea(display,render_screen,window,gc_bgblt,0,0,
	    window_width,window_height,0,0);
  while(!end_game)
    {
      XEvent event;
      /* Some sort of sleep mode in here to slow things down a bit? */
      while(XPending(display))
	{
	  XNextEvent(display,&event);
	  edit_floor.action(&event);
	  switch(event.xany.type) 
	    {
	    case KeyPress:
	      switch(XLookupKeysym((XKeyEvent *)&event,0))
		{
		case KEY_QUIT:
		  end_game = 1;
		  break;
		}
	      break;
	    }
	  if(event.xany.type == KeyPress)
	    {
	      edit_floor.draw();
	      XCopyArea(display,render_screen,window,gc_bgblt,0,0,
			window_width,window_height,0,0);
	    }
	}
    }
}

/***************************************************************************
 *
 ***************************************************************************/
void print_help(void)
{
  printf("Usage: ned <options>\n"
	 " -h            Display this help info\n"
	 " -n Width Height Indice <floor_map>\n"
	 "    Create a new floor map of Width x Height, using Indice as\n"
	 "    the tile number to use to initialise the map with\n"
	 " -l <floor_map>\n"
	 "    Load an existing Floor map\n"
	 "\n");
}

int main(int argc,char *argv[])
{
  register int x,new_f = 1;
  int map_x_size = 2,map_y_size = 2;
  char *map_filename = ""; /* Should this actually be '\0' so it triggers the if(!map_filename) down below? */

  printf("Welcome to NightHawk (Floor Editor) - By Jason Nunn\n");
  printf("Updated by Eric Gillespie and others \n(viking at users dot sourceforge dot net)\n");
  if(argc == 1)
    {
      print_help();
      return 1;
    }
  edit_floor.default_map_value = 0;
  for(x = 0;x < argc;x++)
    {
      if(argv[x][0] == '-')
	{
	  switch(argv[x][1])
	    {
	    case 'h':
	      print_help();
	      return 1;
	    case 'n':
	      new_f = 1;
	      /* I note there's no testing of these values for validity */
	      sscanf(argv[x + 1],"%d",&map_x_size);
	      sscanf(argv[x + 2],"%d",&map_y_size);
	      sscanf(argv[x + 3],"%d",&edit_floor.default_map_value);
	      map_filename = (char *)argv[x + 4];
	      if(!map_filename)  { // Oooops - we forgot to specify a filename value
		printf("Ooops - you forgot to specify a filename, didn't you?\n");
		print_help();
		return 1;
	      }
	      break;
	    case 'l':
	      new_f = 0;
	      map_filename = (char *)argv[x + 1];
	      break;
	    }
	}
    }
  if(x_init())
    {
      XAutoRepeatOn(display);
      if(load_flr_xpms())
	{
	  if(load_sprite_xpms())
	    {
	      edit_floor.init_colours();
	      x = 1;
	      if(new_f)
		{ // We really should check if the file exists already, and confirm 
		  // if the user wishes to replace it with the new map
		  // So far, all we do is abort if file exists already
		  if(fopen(map_filename, "r"))  {
		    printf("Ooops - the file already exists!\n");
		    /*						fclose(map_filename); */
		  }
		  if(!edit_floor.create(map_filename,map_x_size,map_y_size))
		    x = 0;
		}
	      else
		if(!edit_floor.load(map_filename,"Edit"))
		  x = 0;
	      if(x)
		{
		  XMapWindow(display,window);
		  XSync(display,0);
		  printf("Running..\n");
		  fg_loop();
		  edit_floor.unload();
		}
	      free_sprite_xpms();
	    }
	  free_flr_xpms();
	}
      Xreaper();
    }
  printf("See ya\n");
  return 1;
}
/* 
   vim:tw=0:ww=0:ts=2 
*/

