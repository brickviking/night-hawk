/***************************************************************************
****************************************************************************
****************************************************************************
*
* NightHawk - By Jason Nunn - Oct 96 (jsno@dayworld.net.au)
* FREEWARE.
*
* Xwindows Demo (C++)
*
****************************************************************************
****************************************************************************
***************************************************************************/
extern "C" {
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <X11/Xatom.h>
#include <X11/xpm.h>
}
#include "defs.h"
#include "icon_bm.h"

char        *host = NULL;
Display     *display;
Window      window,root,parent;
int         depth,screen;
GC          gc_line,gc_dline,gc_bgblt,gc_bltblt;
Pixmap      render_screen;
XFontStruct *font_st;
Font        font;

int     window_width  = SCREEN_SIZE_X;
int     window_height = SCREEN_SIZE_Y;
int     window_x_pos  = 100;
int     window_y_pos  = 100;

int     score,sscore;

tsprite *flr_sprites;
int     sprites_size;
tbm     droid_bm;
tbm     paradroid_bm;
tbm     laser_l_bm;
tbm     laser_cb_bm;
tbm     laser_uv_bm;
tbm     laser_te_bm;
tbm     explosion_bm;
tbm     power_bay_bm;
tbm     ntitle_bm;
tbm     doorh_bm;
tbm     doorv_bm;
tbm     digit_bm;
tbm     digit2_bm;
tbm     ship_complete_bm;
tbm     game_over1_bm;
tbm     credit_1_bm;
tbm     credit_2_bm;
tbm     credit_3_bm;
tbm     get_ready2_bm;
tbm     neg_bm;
tbm     paused_bm;

unsigned long black_pixel;
unsigned long white_pixel[8];
unsigned long blue_pixel[8];
unsigned long yellow_pixel;
unsigned long green_pixel[MAX_SCORE_TABLE];
unsigned long red_pixel[MAX_SCORE_TABLE];

tdroid_stats droid_stats[] = {
  {
    "002",
    "Paradroid",
    1,
    1.0,
    27.0,
    "Neurologic",
    LASER_LINARITE,
    10,
    10,
    5,
    0
  },
  {
    "108",
    "Minor Cleaning Droid",
    1,
    0.6,
    38.3,
    "Embedded Logic",
    LASER_NONE,
    1,
    1,
    2,
    -1
  },
  {
    "176",
    "Standard Cleaning Droid",
    1,
    1.2,
    80.6,
    "Embedded Logic",
    LASER_NONE,
    5,
    5,
    3,
    -1
  },
  {
    "261",
    "Cargo Paletting Droid",
    2,
    3,
    800.75,
    "Embedded Logic",
    LASER_NONE,
    500,
    500,
    1,
    -1
  },
  {
    "275",
    "Servant Droid",
    2,
    0.4,
    15.3,
    "Embedded Logic",
    LASER_NONE,
    15,
    15,
    5,
    -1
  },
  {
    "355",
    "Messenger (Class C) Droid",
    3,
    0.5,
    10.0,
    "Embedded Logic",
    LASER_NONE,
    10,
    10,
    6,
    -1
  },
  {
    "368",
    "Messenger (Class D) Droid",
    3,
    0.76,
    11.0,
    "Embedded Logic",
    LASER_NONE,
    20,
    20,
    5,
    -1
  },
  {
    "423",
    "Standard Maintenance Droid",
    4,
    2,
    60.0,
    "Embedded Logic",
    LASER_NONE,
    20,
    20,
    3,
    -1
  },
  {
    "467",
    "Heavy Duty Maintenance Droid",
    4,
    1.5,
    100.0,
    "Embedded Logic",
    LASER_CROC_BENZ,
    25,
    25,
    3,
    -1
  },
  {
    "489",
    "Aerial Maintenance Droid",
    4,
    0.2,
    5.0,
    "Embedded Logic",
    LASER_LINARITE,
    25,
    25,
    6,
    -1
  },
  {
    "513",
    "Trauma Technician",
    5,
    1,
    40.0,
    "Neurologic",
    LASER_CROC_BENZ,
    30,
    30,
    3,
    0
  },
  {
    "520",
    "Nurse Droid",
    5,
    0.6,
    50.0,
    "Neurologic",
    LASER_CROC_BENZ,
    35,
    35,
    4,
    0
  },
  {
    "599",
    "Surgeon Droid",
    5,
    2,
    56.80,
    "Neurologic",
    LASER_UVAROVITE,
    45,
    45,
    4,
    0
  },
  {
    "606",
    "Guard Monitor",
    6,
    1.1,  
    20.0,
    "Neurologic",
    LASER_CROC_BENZ,
    15,
    15,
    4,
    20
  },
  {
    "691",
    "Sentinel Droid",
    6,
    1.9,
    45.0,
    "Neurologic",
    LASER_CROC_BENZ,
    40,
    40,
    5,
    17
  },
  {
    "693",
    "Centurion Droid",
    6,
    2.0,  
    50.0,
    "Neurologic",
    LASER_UVAROVITE,
    50,
    50,
    3,
    20
  },
  {
    "719",
    "Battle Droid",
    7,
    1.5,
    40.92,
    "Neurologic",
    LASER_CROC_BENZ,
    40,
    40,
    2,
    20
  },
  {
    "720",
    "Flack Droid",
    7,
    1.0,
    30.10,
    "Neurologic",
    LASER_CROC_BENZ,
    30,
    30,
    6,
    20
  },
  {
    "766",
    "Attack Droid",
    7,
    1.9,
    67.45,
    "Neurologic",
    LASER_UVAROVITE,
    70,
    70,
    4,
    15
  },
  {
    "799",
    "Heavy Battle Droid",
    7,
    2.0,
    90.98,
    "Neurologic",
    LASER_UVAROVITE,
    100,
    100,
    3,
    12
  },
  {
    "805",
    "Communications Droid",
    8,
    0.8,
    50.10,
    "Neurologic",
    LASER_UVAROVITE,
    70,
    70,
    3,
    15
  },
  {
    "810",
    "Engineer Droid",
    8,
    1.74,
    60.00,
    "Neurologic",
    LASER_UVAROVITE,
    80,
    80,
    2,
    14
  },
  {
    "820",
    "Science Droid",
    8,
    1.0,
    40.00,
    "Neurologic",
    LASER_UVAROVITE,
    90,
    90,
    3,
    13
  },
  {
    "899",
    "Pilot Droid",
    8,
    1.74,
    60.00,
    "Neurologic",
    LASER_UVAROVITE,
    90,
    90,
    2,
    12
  },
  {
    "933",
    "Minor Cyborg",
    9,
    3.1,
    120.0,
    "Neurologic",
    LASER_TIGER_EYE,
    20,
    20,
    4,
    20
  },
  {
    "949",
    "Command Cyborg",
    9,
    2.6,
    100.0,
    "Neurologic",
    LASER_TIGER_EYE,
    70,
    70,
    5,
    15
  },
  {
    "999",
    "Fleet Cyborg",
    9,
    3.1,
    90.95,
    "Neurologic",
    LASER_TIGER_EYE,
    200,
    200,
    6,
    10
  }
};

XRectangle rech_pos1[4] =
  {{-1,-17,1,1},{-1,15,1,1},{-16,-1,1,1},{14,-1,1,1}};
XRectangle rech_tmp[4] =
  {{0,0,1,1},{0,0,1,1},{0,0,1,1},{0,0,1,1}};
XRectangle rech_pos2[4] =
  {{-16 + SCREEN_HSIZE_X,-17 + SCREEN_HSIZE_Y,1,1},
   {-17 + SCREEN_HSIZE_X, 15 + SCREEN_HSIZE_Y,1,1},
   { 14 + SCREEN_HSIZE_X, 15 + SCREEN_HSIZE_Y,1,1},
   { 14 + SCREEN_HSIZE_X,-17 + SCREEN_HSIZE_Y,1,1}};

/***************************************************************************
*
***************************************************************************/
int x_init(void)
{
  char *error_m = "x_init() ";
  char *wname = "Nighthawk " VERSION;
  Pixmap iconPixmap;
  XWMHints xwmhints;
  XVisualInfo visual_info;
  XGCValues gcv;
  char *font_name = "-*-fixed-*-*-*-*-12-*-*-*-*-*-*-*";
  XSizeHints sizehints;

  if((host = (char *)getenv("DISPLAY")) == NULL)
  {
    perror(error_m);
    return -1;
  }
  if((display = XOpenDisplay(host)) == NULL) 
  {
    perror(error_m);
    return -1;
  }
  screen = DefaultScreen(display);
  root = parent = RootWindow(display,screen);
  depth = DefaultDepth(display,screen);
  if(!XMatchVisualInfo(display,screen, 
    DefaultDepth(display,screen),PseudoColor,&visual_info))
    if(!XMatchVisualInfo(display,screen, 
      DefaultDepth(display,screen),DirectColor,&visual_info))
      if(!XMatchVisualInfo(display,screen, 
        DefaultDepth(display,screen),TrueColor,&visual_info))
      {
        perror(error_m);
        return -1;
      }
      else
        printf("Colour Mode: TrueColor\n");
    else
      printf("Colour Mode: DirectColor\n");
  else
    printf("Colour Mode: PseudoColor\n");
  window = XCreateSimpleWindow(
    display,root,
    window_x_pos,window_y_pos,
    window_width,window_height,
    1,0,0);
  XSelectInput(display,window,
    ButtonPressMask | KeyPressMask | KeyReleaseMask);
  XChangeProperty(
    display,window,XA_WM_NAME,XA_STRING,8,
    PropModeReplace,(unsigned char *)wname,strlen(wname));
  iconPixmap = XCreateBitmapFromData(display,window,
    (char *)icon_bm_bits,icon_bm_width,icon_bm_height);
  xwmhints.icon_pixmap = iconPixmap;
  xwmhints.initial_state = NormalState;
  xwmhints.flags = IconPixmapHint | StateHint;
  XSetWMHints(display,window,&xwmhints);

  sizehints.flags = PSize | PMinSize | PMaxSize;
  sizehints.min_width = window_width;
  sizehints.min_height = window_height;
  sizehints.max_width = window_width;
  sizehints.max_height = window_height;
  XSetWMNormalHints(display,window,&sizehints);

  render_screen =
    XCreatePixmap(display,window,window_width,window_height,depth);

  gcv.line_width = 3;
  gcv.line_style = LineSolid;
  gc_line = XCreateGC(display,window,GCLineWidth | GCLineStyle,&gcv);

  gcv.line_width = 3;
  gcv.line_style = LineOnOffDash;
  gc_dline = XCreateGC(display,window,GCLineWidth | GCLineStyle,&gcv);

  gc_bgblt = XCreateGC(display,window,0,NULL);
  gcv.function = GXcopy;
  gcv.fill_style = FillTiled;
  gcv.graphics_exposures = False;
  gc_bltblt = XCreateGC(display,window,
    GCFunction | GCFillStyle | GCGraphicsExposures,&gcv);
  XFillRectangle(display,render_screen,gc_bgblt,
    0,0,window_width,window_height);

  font_st = XLoadQueryFont(display,font_name);
  if(font_st != NULL)
  {
    XFreeFont(display,font_st);
    font = XLoadFont(display,font_name);
    XSetFont(display,gc_line,font);
    XSetFont(display,gc_dline,font);
    XSetFont(display,gc_bgblt,font);
    XSetFont(display,gc_bltblt,font);
  }
  return 1;
}

/***************************************************************************
*
***************************************************************************/
int loadxpm(char *filename,tbm *bm)
{
  XWindowAttributes root_attr;
  XpmAttributes xpm_attributes;
  register int val;
  XpmImage xpmimage;

  XGetWindowAttributes(display,root,&root_attr); 
  xpm_attributes.colormap = root_attr.colormap;
  xpm_attributes.valuemask = XpmSize | XpmReturnPixels | XpmColormap;
  val = XpmReadFileToPixmap(
    display,root,filename,&bm->pixmap,&bm->mask,&xpm_attributes);
  switch(val)
  {
    case XpmColorError:
      printf("Error: Xpm colour error. Aborting.\n");
      break;
    case XpmSuccess:
      bm->width = xpm_attributes.width;
      bm->height = xpm_attributes.height;
      XpmCreateXpmImageFromPixmap(
        display,bm->pixmap,bm->mask,&xpmimage,&xpm_attributes);
      return 1;
    case XpmOpenFailed:
      printf("Error: Xpm open failed. Aborting.\n");
      break;
    case XpmFileInvalid:
      printf("Error: Xpm file invalid. Aborting.\n");
      break;
    case XpmNoMemory:
      printf("Error: Xpm no memory left. Aborting.\n");
      break;
    case XpmColorFailed:
      printf("Error: Xpm Colour failed. Aborting.\n");
      break;
  }
  return 0;
}

int loadxpm_xpm(char *filename,tbm *bm)
{
  char str[STR_LEN];

  strcpy(str,INSTALL_LIB_DIR "/xpm/");
  strcat(str,filename);
  return loadxpm(str,bm);
}

void drawxpm_ani(tbm *bm,int x,int y,int i,int total)
{
  if((x > -64) && (x < (SCREEN_SIZE_X + 64)) &&
     (y > -64) && (y < (SCREEN_SIZE_Y + 64)))
  {
    register int sy,o;

    sy = bm->height / total;
    o = i * sy;
    XSetClipOrigin(display,gc_bltblt,x,y - o);
    XSetClipMask(display,gc_bltblt,bm->mask);
    XCopyArea(display,bm->pixmap,render_screen,gc_bltblt,
      0,o,bm->width,sy,x,y);
  }
}

/***************************************************************************
*
***************************************************************************/
void ramp_colour(
  unsigned short r1,unsigned short g1,unsigned short b1,
  unsigned short r2,unsigned short g2,unsigned short b2,
  unsigned long *base,int run)
{
  register int x;
  XColor C;
  double rs,gs,bs;
  Colormap DefaultCMap;

  DefaultCMap = DefaultColormap(display,screen);
  rs = (r2 - r1) / run;
  gs = (g2 - g1) / run;
  bs = (b2 - b1) / run;
  C.flags = DoRed | DoGreen | DoBlue;
  for(x = 0;x < run;x++)
  {
    C.red = r1 + (unsigned short)(x * rs);
    C.green = g1 + (unsigned short)(x * gs);
    C.blue = b1 + (unsigned short)(x * bs);
    XAllocColor(display,DefaultCMap,&C);
    *(base + x) = C.pixel;
  }
}

/***************************************************************************
*
***************************************************************************/
void init_colours(void)
{
  ramp_colour(0x0,0x0,0x0,0x0,0x0,0x0,&black_pixel,1);
  ramp_colour(0x7000,0x7000,0x7000,0xffff,0xffff,0xffff,white_pixel,8);
  ramp_colour(0x0,0x0,0x7000,0x0,0x0,0xffff,blue_pixel,8);
  ramp_colour(0xf000,0xf000,0x0,0xf000,0xf000,0x0,&yellow_pixel,1);
  ramp_colour(
    0x0000,0xffff,0x0000,
    0x0000,0x8000,0x0000,green_pixel,MAX_SCORE_TABLE);
  ramp_colour(
    0xffff,0x8000,0x8000,
    0x9000,0x1000,0x1000,red_pixel,MAX_SCORE_TABLE);
}

/***************************************************************************
*
***************************************************************************/
void free_bm(tbm *bm)
{
  if(bm->pixmap)
    XFreePixmap(display,bm->pixmap);
  if(bm->mask)
    XFreePixmap(display,bm->mask);
}

/***************************************************************************
*
***************************************************************************/
int load_flr_xpms(void)
{
  register int status = 1;
  FILE *fp;
  char *error_m = "load_flr_xpms() ";
  char str[STR_LEN];
  register int sprite_c;

  if((fp = fopen(INSTALL_LIB_DIR "/xpm/xpm.i","r")) == NULL)
  {
    perror(error_m);
    return 0;
  }
  else
  {
    sprites_size = 0;
    while(!feof(fp))
    {
      str[0] = 0;
      fgets(str,STR_LEN,fp);
      if(str[0])
        sprites_size++;
    }
    if((flr_sprites = 
      (tsprite *)malloc(sprites_size * sizeof(tsprite))) == NULL)
    {
      perror(error_m);
      return 0;
    }
    rewind(fp);
    for(sprite_c = 0;sprite_c < sprites_size;sprite_c++)
    {
      char fn[STR_LEN];

      str[0] = 0;
      fgets(str,STR_LEN,fp);
      sscanf(str,"%s %c",fn,&((flr_sprites + sprite_c)->bound));
      if(fn[0])
        if(!loadxpm_xpm(fn,&((flr_sprites + sprite_c)->bm_ptr)))
        {
          (flr_sprites + sprite_c)->bm_ptr.width = -1;
          status = 0;
          break;
        }
    }
    fclose(fp);
  }
  return status;
}

void free_flr_xpms(void)
{
  register int x;

  for(x = 0;x < sprites_size;x++)
    if((flr_sprites + x)->bm_ptr.width != -1)
      free_bm(&(flr_sprites + x)->bm_ptr);
    else
      break;
  free(flr_sprites);
}

/***************************************************************************
*
***************************************************************************/
void free_sprite_xpms(void)
{
  if(droid_bm.width != -1)
    free_bm(&droid_bm);
  else
    return;
  if(paradroid_bm.width != -1)
    free_bm(&paradroid_bm);
  else
    return;
  if(laser_l_bm.width != -1)
    free_bm(&laser_l_bm);
  else
    return;
  if(laser_cb_bm.width != -1)
    free_bm(&laser_cb_bm);
  else
    return;
  if(laser_uv_bm.width != -1)
    free_bm(&laser_uv_bm);
  else
    return;
  if(laser_te_bm.width != -1)
    free_bm(&laser_te_bm);
  else
    return;
  if(explosion_bm.width != -1)
    free_bm(&explosion_bm);
  else
    return;
  if(power_bay_bm.width != -1)
    free_bm(&power_bay_bm);
  else
    return;
  if(ntitle_bm.width != -1)
    free_bm(&ntitle_bm);
  else
    return;
  if(doorh_bm.width != -1)
    free_bm(&doorh_bm);
  else
    return;
  if(doorv_bm.width != -1)
    free_bm(&doorv_bm);
  else
    return;
  if(digit_bm.width != -1)
    free_bm(&digit_bm);
  else
    return;
  if(digit2_bm.width != -1)
    free_bm(&digit2_bm);
  else
    return;
  if(ship_complete_bm.width != -1)
    free_bm(&ship_complete_bm);
  else
    return;
  if(game_over1_bm.width != -1)
    free_bm(&game_over1_bm);
  else
    return;
  if(credit_1_bm.width != -1)
    free_bm(&credit_1_bm);
  else
    return;
  if(credit_2_bm.width != -1)
    free_bm(&credit_2_bm);
  else
    return;
  if(credit_3_bm.width != -1)
    free_bm(&credit_3_bm);
  else
    return;
  if(get_ready2_bm.width != -1)
    free_bm(&get_ready2_bm);
  else
    return;
  if(neg_bm.width != -1)
    free_bm(&neg_bm);
  else
    return;
  if(paused_bm.width != -1)
    free_bm(&paused_bm);
  else
    return;
}

int load_sprite_xpms(void)
{
  if(!loadxpm_xpm("standard/droid_ani.xpm",&droid_bm))
  {
    droid_bm.width = -1;
    return 0;
  }
  if(!loadxpm_xpm("standard/paradroid_ani.xpm",&paradroid_bm))
  {
    paradroid_bm.width = -1;
    return 0;
  }
  if(!loadxpm_xpm("standard/laser_l.xpm",&laser_l_bm))
  {
    laser_l_bm.width = -1;
    return 0;
  }
  if(!loadxpm_xpm("standard/laser_cb.xpm",&laser_cb_bm))
  {
    laser_cb_bm.width = -1;
    return 0;
  }
  if(!loadxpm_xpm("standard/laser_uv.xpm",&laser_uv_bm))
  {
    laser_uv_bm.width = -1;
    return 0;
  }
  if(!loadxpm_xpm("standard/laser_te.xpm",&laser_te_bm))
  {
    laser_te_bm.width = -1;
    return 0;
  }
  if(!loadxpm_xpm("standard/explosion.xpm",&explosion_bm))
  {
    explosion_bm.width = -1;
    return 0;
  }
  if(!loadxpm_xpm("standard/power_bay.xpm",&power_bay_bm))
  {
    power_bay_bm.width = -1;
    return 0;
  }
  if(!loadxpm_xpm("standard/ntitle.xpm",&ntitle_bm))
  {
    ntitle_bm.width = -1;
    return 0;
  }
  if(!loadxpm_xpm("standard/flr_door_h_cyan_ani.xpm",&doorh_bm))
  {
    doorh_bm.width = -1;
    return 0;
  }
  if(!loadxpm_xpm("standard/flr_door_v_cyan_ani.xpm",&doorv_bm))
  {
    doorv_bm.width = -1;
    return 0;
  }
  if(!loadxpm_xpm("standard/digits.xpm",&digit_bm))
  {
    digit_bm.width = -1;
    return 0;
  }
  if(!loadxpm_xpm("standard/digits2.xpm",&digit2_bm))
  {
    digit2_bm.width = -1;
    return 0;
  }
  if(!loadxpm_xpm("standard/ship_complete.xpm",&ship_complete_bm))
  {
    ship_complete_bm.width = -1;
    return 0;
  }
  if(!loadxpm_xpm("standard/game_over_mess1.xpm",&game_over1_bm))
  {
    game_over1_bm.width = -1;
    return 0;
  }
  if(!loadxpm_xpm("standard/credit_1.xpm",&credit_1_bm))
  {
    credit_1_bm.width = -1;
    return 0;
  }
  if(!loadxpm_xpm("standard/credit_2.xpm",&credit_2_bm))
  {
    credit_2_bm.width = -1;
    return 0;
  }
  if(!loadxpm_xpm("standard/credit_3.xpm",&credit_3_bm))
  {
    credit_3_bm.width = -1;
    return 0;
  }
  if(!loadxpm_xpm("standard/get_ready_2.xpm",&get_ready2_bm))
  {
    get_ready2_bm.width = -1;
    return 0;
  }
  if(!loadxpm_xpm("standard/neg.xpm",&neg_bm))
  {
    neg_bm.width = -1;
    return 0;
  }
  if(!loadxpm_xpm("standard/paused.xpm",&paused_bm))
  {
    paused_bm.width = -1;
    return 0;
  }
  return 1;
}

/***************************************************************************
*    
***************************************************************************/
void Xreaper(void)
{
  XFreeGC(display,gc_dline);
  XFreeGC(display,gc_bgblt);
  XFreeGC(display,gc_bltblt);
  if(font_st != NULL)
    XUnloadFont(display,font);
  XCloseDisplay(display);
}
