/***************************************************************************
****************************************************************************
****************************************************************************
*
* NightHawk - By Jason Nunn - Oct 96  (jsno@dayworld.net.au)
* FREEWARE.
*
* Xwindows (C++)
*
* ==================================================================
* Main
*
****************************************************************************
****************************************************************************
***************************************************************************/
extern "C" {
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>
#include <pwd.h>
#include <sys/types.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/xpm.h>
#include <X11/keysym.h>
}
#include "defs.h"
#include "misc.h"
#include "tship.h"

struct itimerval oldtimer,timer;
tship  *ship = NULL;
tscore_table score_table[MAX_SCORE_TABLE];
int ship_ptr,ship_start = 0;
char *ship_table[] = {
  "Haldeck",
  "Seafarer",
  "Anoyle",
  "Esperence",
  "Ophukus",
  "Mearkat",
  "Friendship",
  "Discovery",
  "Zaxon",
  "Tobruk",
  NULL};

int intro_count = 0;
char *scroll_base_ptr;
int scroll_pos;
char *scroll_story = 
"          -= Paradroid =-           "
"                                    "
" Chapter II - The Rescue from Vega! "
"                                    "
"                                    "
"The Earth forces  have just ambushed"
"an enemy fleet transporting captured"
"Earth vessels bound for  Vega. While"
"crews were boarding  these ships for"
"repairs,  surviving   Vegarian  crew"
"destroyed  each   ships  warp  core."
"This  has   made   the   ships  very"
"radioactive,  causing  many  of  the"
"droids on  board  these  ships to go"
"'rogue'.  Our last  contact with the"
"crew indicated heavy  loss  of life."
"All attempts  at  contact  or rescue"
"have proved futile. We can only fear"
"the worst..                         "
"                                    "
"..However, we have depatched a Class"
"002 Paradroid. Improved over the 001"
"class, the 002 will teleport to each"
"ship and annihilate all droids.     ";

char *scroll_keys = 
"               =Keys=               "
"                                    "
"  Arrow keys                        "
"    -> Move                         "
"                                    "
"  Mouse button & Pointer            "
"    -> Shoot laser                  "
"    -> Capture droid                "
"                                    "
"  'p'                               "
"    -> Pause                        "
"                                    "
"  's'                               "
"    -> Display stats                "
"                                    "
"  't'                               "
"    -> Toggle transfer mode         "
"                                    "
"  Space                             "
"    -> Change levels                "
"    -> view console                 "
"    -> Boost power                  ";

char *scroll_game_play =
"            =Game play=             "
"                                    "
"The mission  is  simple-   Slate all"
"droids.                             "
"                                    "
"You start out as  a   002 Paradroid."
"In   the    original   game,     you"
"controlled a  001  Influence device."
"They differ in the  way they control"
"their  hosts.   The  001  droid  was"
"basically a helmet  that fitted over"
"the  host  droids   head,  and could"
"control it for a   certain length of"
"time.   The    002   droid   however"
"actually   transfers  it's  programs"
"over to the   new host,  effectively"
"'becoming' it,  and   destroying the"
"old host it occupied.               "
"                                    "
"There are  good  and  bad  points to"
"this.  The  good  point  is that you"
"can occupy  the  host   droid for an"
"unlimited length  of  time,  the bad"
"point is  that  you   are stuck with"
"the new hosts  weaponary, speed  and"
"shields.   You    also   get  points"
"for transferring, which  is 50 times"
"by the host droids entry value.     "
"                                    "
"To  transfer,  you  have   to  first"
"enable transfer mode by  hitting the"
"'t' key (you  can   disenable  it by"
"pressing   't'   again).   While  in"
"transfer  mode, your  lasers will be"
"inactive.                           "
"                                    "
"If   you   are   in   range,  simply"
"point and  click  on   the  host you"
"want  to   capture.   A  red  dotted"
"line will  protrude  to the captured"
"droid,  and  a  progress   box  will"
"appear  indicating  the  progress of"
"the   takeover.  You   can  win  the"
"negotiation,  hence  taking over the"
"captured droid,  or  you   can lose,"
"hence  burning   out    and  getting"
"destroyed.                          "
"                                    "
"Another thing  to  note  is that any"
"captured  hostile   droids  will not"
"attack you  while  captured. You can"
"even capture through walls.         "
"                                    "
"You can  replemish  your shields  by"
"going  to  power  bays  and  hitting"
"spacebar.  Each  increment of shield"
"will cost you 10 points.            "
"                                    "
"You can change  levels  by  going to"
"the elevators and hitting the  space"
"bar.  Select  level  with  the arrow"
"keys,  and   hit  space  to  select."
"                                    "
"There are also consoles  to give you"
"droid   information.   Operate  with"
"the   spacebar   and   arrow   keys."
"                                    "
"Firing  Lasers  (if   occupied droid"
"is fitted with  lasers)   is done by"
"using the Pointer and mouse  button."
"A destroyed droid will earn  you (in"
"points)  25  times  by   their entry"
"level.  A hit  is  worth  the droids"
"Entry value.  Points scored by other"
"go to you.                          ";

char *scroll_tips =
"            =Game Tips=             "
"                                    "
"If possible, create 'friendly fire'."
"This  is  very  easy   to  do.   Get"
"inbetween any two  armed droids.  If"
"one of  them   preemptively attacks,"
"simply duck  at  the last minute (if"
"not, then just  shoot at one). Their"
"laser   fire   will/should/hopefully"
"hit the other droid. The other droid"
"will then fire  back   at  the droid"
"that  accidently  attacked it.  When"
"the  attacking droid is hit, it will"
"fire  back  at  the  droid firing at"
"it....hence  a shoot out will occur."
"The result is  either  two destroyed"
"droids,  or one left  staying with a"
"reduced shield.                     "
"                                    "
"Another good  thing   about friendly"
"fire  is  that  it's   an  excellent"
"distraction. Any 6xx,7xx, 9xx droids"
"involved in one,  won't be concerned"
"with you at all;   they  will be too"
"busy blowing away their mate.       "
"                                    "
"Always  duck  for   cover when fired"
"upon,  and  when  you shoot  a droid"
"fitted  with   weapons,  be ready to"
"duck for cover.  Don't just take it."
"Armed  droids  will   always  return"
"fire immediately   with  an equal or"
"greater rate.                       "
"                                    "
"Get into the habit  of  firing a 4-6"
"round volley, then  ducking behind a"
"wall, door, storage  unit... or even"
"another droid.  261's  are excellent"
"to hide  behind.  These  droids  are"
"industrial cargo movers. Being heavy"
"machinery,  they   have a  very high"
"shield rating  and  can  buffer many"
"laser blows.   Medical  Droids  (5xx"
"series)  are  another good  example."
"Medical   droids    are   armed  but"
"passive.   They  won't attack anyone"
"unless fired upon.   When  an attack"
"droid is firing  at you, duck behind"
"a medical droid.   If the  attacking"
"droid  hits  the  medic,  then   the"
"medic   will    fire    (one   shot)"
"back  at  the attacking  droid  (yet"
"another  'friendly  fire'  example)."
"The  599 Surgeon  droids  are fitted"
"with the  powerful Uvarovite lasers,"
"and  will  cheerfully  sodomise  any"
"minor  class  6xx's  or  7xx's  that"
"accidently attack it.               "
"                                    "
"You   can   go   through   walls  by"
"transferring to  another  droid.  In"
"certain parts  of the game, you will"
"have to do this  inorder to proceed."
"                                    "
"I haven't  tried  this  but  you can"
"use transfers to stop  an  assailant"
"droid from firing at  you.  This has"
"many applications,  all of which can"
"buy  you a  distraction  or  time to"
"escape.                             ";

char *scroll_misc =
"            =Misc Info=             "
"                                    "
"Laser ratings are as follows:       "
"                                    "
"                   Speed  Damage    "
"  Linarite         8      4         "
"  Crocoite-Benzol  10     10        "
"  Uvarovite        12     20        "
"  Tiger-Eye Quartz 9      50        "
"                                    "
"                                    "
"Droid descriptions are as follows:  "
"                                    "
" 0xx - Prototype Class              "
"                                    "
"These   droids    are     prototype/"
"experimental   class  that  vary  in"
"function considerably. Approach with"
"caution.                            "
"                                    "
" 1xx - Cleaning Droids              "
"                                    "
"Mindless,   slow,    low   shielded,"
"unarmed   droids   that   clean  the"
"ships. Harmless.                    "
"                                    "
" 2xx - Logistic/Servant Droids      "
"                                    "
"Again,   brainless  droids  that  do"
"various tasks.  These  type can vary"
"in shield and Strength.  This  class"
"is harmless as well.                "
"                                    "
" 3xx - Messenger Droids             "
"                                    "
"Mindless,  but very fast. Low shield"
"rating, and are not armed.          "
"                                    "
" 4xx - Maintenance Droids           "
"                                    "
"Designed  to repair  the ships. Vary"
"in   shield   and   speed   ratings."
"Sometimes armed.                    "
"                                    "
" 5xx - Medical Droids               "
"                                    "
"These  droids   have  a  high  entry"
"level, and are  difficult  to crack."
"All of them are armed,  but are  not"
"hostile. However, they will shoot at"
"any droids that attack them.        "
"                                    "
" 6xx - Sentinel Droids              "
"                                    "
"These    droids    'guard'   certain"
"important  area's of the  ships like"
"lifts,  power bays and other droids."
"They  vary  in  shield rating, speed"
"and fire  power,  but  all are armed"
"and   will  attack.  Approach   with"
"caution.                            "
"                                    "
" 7xx - Battle Droids                "
"                                    "
"This  class of  droid 'hunts'.  When"
"confronted, any  will  attack.  They"
"vary  in  speed, shield   rating and"
"weaponary,  but  all  are  extremely"
"dangerous.                          "
"                                    "
" 8xx - Crew Droids                  "
"                                    "
"These are armed  droids that control"
"the ship.  These like  the 5xx class"
"are  not hostile,  but  will  become"
"hostile to  any  droids  that attack"
"them.   8xx's   are     armed   with"
"Uvarovite lasers, and  pack a rather"
"powerful wallop.                    "
"                                    "
" 9xx - Command Cyborgs              "
"                                    "
"Each  ship will  have  one of these."
"They command  the  ships.  Extremely"
"armed,   extremely  shielded,  often"
"very fast, and very deadly.         ";

char *scroll_disclaimer =
"               =GPL=                "
"                                    "
"Nighthawk   comes   under  the   GNU"
"GENERAL PUBLIC LICENSE AGREEMENT,and"
"comes  with  ABSOLUTELY NO WARRANTY."
"This is free software,  and  you are"
"welcome  to  redistribute  it  under"
"certain conditions.                 "
"                                    "
"      Support Free Software!        "
"                                    "
"                                    "
"            =Contacts=              "
"                                    "
"  Postal: Jason Nunn                "
"          32 Rothdale Rd            "
"          Moil Darwin NT 0810       "
"          AUSTRALIA                 "
"                                    "
"   Email: jsno@dayworld.net.au or   "
"          jsno@stormfront.com.au    "
"                                    "
"     WWW: www.downunder.net.au/~jsno"
"                                    "
" OZForum: www.downunder.net.au/forum"
"    (in the 'Unix_Programming' area)"
"                                    "
"     Tel: 61 08 82640593            "
"                                    "
"The number  is available  for people"
"who   wish   to  contact   me  about"
"Nighthawk. Please make sure you call"
"at   a    reasonable   hour   (South"
"Australian time).                   ";


/***************************************************************************
*
***************************************************************************/
void init(void)
{
  register int x;

  umask(~0666);
  for(x = 0;x < MAX_SCORE_TABLE;x++)
  {
    strcpy(score_table[x].name,"-");
    strcpy(score_table[x].highest_ship,"-");
    strcpy(score_table[x].time,"-");
    score_table[x].score = 0;
  }
}

/***************************************************************************
*
***************************************************************************/
FILE *lopen(char *filename,char *mode)
{
  FILE *fp;

  if((fp = fopen(filename,mode)) != NULL)
    flock(fp->_fileno,LOCK_EX);
  return fp;
}

FILE *load_scores_sub(void)
{
  FILE *fp;

  fp = lopen(SCORES_FILE,"r+b");
  if(fp != NULL)
  {
    rewind(fp);
    fread(score_table,sizeof(tscore_table),MAX_SCORE_TABLE,fp);
    return fp;
  }
  else
  {
    fp = lopen(SCORES_FILE,"w+b");
    if(fp != NULL)
      return fp;
  }
  return NULL;
}

void load_scores(void)
{
  FILE *fp;

  fp = load_scores_sub();
  if(fp != NULL)
    fclose(fp);
}

void update_score(int complete)
{
  FILE *fp;

  if(!score)
  {
    intro_count = 8 << 5;
    return;
  }
  fp = load_scores_sub();
  if(fp != NULL)
  {
    register int x;

    intro_count = 640 << 5;
    for(x = 0;x < MAX_SCORE_TABLE;x++)
      if(score > score_table[x].score)
      {
        register int y;
        struct passwd *ps;
        time_t t;
        char *c;

        for(y = MAX_SCORE_TABLE - 1;y > x;y--)
          memcpy(score_table + y,score_table + y - 1,sizeof(tscore_table));
        ps = getpwuid(getuid());
        if(ps != NULL)
          strncpy(score_table[x].name,ps->pw_name,8);
        else
          strcpy(score_table[x].name,"unknown");

        if(ship_start)
        {
          register int a;

          strncpy(score_table[x].highest_ship,ship_table[ship_ptr],15);
          a = strlen(score_table[x].highest_ship);
          score_table[x].highest_ship[a] = '*';
          score_table[x].highest_ship[a + 1] = '0' + (char)ship_start;
        }
        else
        {
          if(complete)
            strcpy(score_table[x].highest_ship,"Complete!");
          else
            strncpy(score_table[x].highest_ship,ship_table[ship_ptr],15);
        }
        score_table[x].highest_ship[15] = 0;
        t = time(NULL);
        c = ctime(&t);
        memcpy(score_table[x].time,c + 8,2);
        memcpy(score_table[x].time + 2,"-",1);
        memcpy(score_table[x].time + 3,c + 4,3);
        memcpy(score_table[x].time + 6,"-",1);
        memcpy(score_table[x].time + 7,c + 20,4);
        score_table[x].time[11] = 0;
        score_table[x].score = score;
        break;
      }
    rewind(fp);
    fwrite(score_table,sizeof(tscore_table),MAX_SCORE_TABLE,fp);
    fclose(fp);
  }
}

/***************************************************************************
*
***************************************************************************/
void bg_init(void (*ptr)(int))
{
  signal(SIGALRM,ptr);
  timer.it_value.tv_sec = 0; 
  timer.it_interval.tv_sec = 0;
  timer.it_value.tv_usec = REALTIME_DELAY;
  timer.it_interval.tv_usec = REALTIME_DELAY;
  setitimer(ITIMER_REAL,&timer,&oldtimer);
}

void bg_deinit(void)
{
  setitimer(ITIMER_REAL,&oldtimer,&timer);
}

/***************************************************************************
*
***************************************************************************/
void intro_bg_calc(int sig)
{
  signal(sig,&intro_bg_calc);
  intro_count++;
  scroll_pos++;
}

void draw_scroll(void)
{
  register int a,b,c,d;
  int co_tab[14] = {7,6,5,4,3,2,1,1,2,3,4,5,6,7};

  d = strlen(scroll_base_ptr);
  b = (scroll_pos >> 2) / 12;
  c = (scroll_pos >> 2) % 12;
  for(a = 0;a < 14;a++)
  {
    register int p;

    p = (a + b) * 36;
    if((p >= 0) && (p < d))
    {
      XSetForeground(display,gc_dline,green_pixel[co_tab[a]]);
      XDrawString(display,render_screen,gc_dline,
        55,50 + (a * 12) - c,scroll_base_ptr + p,36);
    }
  }
  XSetForeground(display,gc_dline,red_pixel[7]);
  XFillRectangle(display,render_screen,gc_bgblt,
    55,28,220,12);
  drawxpm_ani(&ntitle_bm,
    SCREEN_HSIZE_X - (ntitle_bm.width >> 1),5,0,1);
  XFillRectangle(display,render_screen,gc_bgblt,
    55,190,220,10);
}

void display_main_credit(void)
{
  char *any_key_mess = "Hit any key to play, Q to Quit";

  drawxpm_ani(&credit_3_bm,
    SCREEN_HSIZE_X - (credit_3_bm.width >> 1),
    30,0,1);
  XSetForeground(display,gc_dline,green_pixel[0]);
  XDrawString(display,render_screen,gc_dline,
    70,140,any_key_mess,strlen(any_key_mess));
}

void display_scores(void)
{
  char str[STR_LEN];
  register int x;

  drawxpm_ani(&ntitle_bm,
    SCREEN_HSIZE_X - (ntitle_bm.width >> 1),5,0,1);
  XSetForeground(display,gc_dline,red_pixel[0]);
  sprintf(str,"%-8s %-15s %-11s    Score","Name","Ship","Date");
  XDrawString(display,render_screen,gc_dline,25,52,str,strlen(str));
  for(x = 0;x < MAX_SCORE_TABLE;x++)
  {
    register int yp;

    yp = 70 + (x * 13);
    XSetForeground(display,gc_dline,green_pixel[x]);
    sprintf(str,"%-8s %-15s %-11s %8d",
      score_table[x].name,
      score_table[x].highest_ship,
      score_table[x].time,
      score_table[x].score);
    XDrawString(display,render_screen,gc_dline,25,yp,str,strlen(str));
  }
}

int draw_intro(void)
{
  register int intro_f = 0;

  load_scores();
  bg_init(&intro_bg_calc);
  while(!intro_f)
  {
    XEvent event;
    register int i;

    XFillRectangle(display,render_screen,gc_bgblt,
      0,0,window_width,window_height);
    i = intro_count >> 5;
    switch(i)
    {
      case 0:
      case 1:
        drawxpm_ani(&credit_1_bm,
          SCREEN_HSIZE_X - (credit_1_bm.width >> 1),
          50,0,1);
        break;
      case 3:
      case 4:
        drawxpm_ani(&credit_2_bm,
          SCREEN_HSIZE_X - (credit_2_bm.width >> 1),
          50,0,1);
        break;
      case 6:
      case 7:
        drawxpm_ani(&credit_3_bm,
          SCREEN_HSIZE_X - (credit_3_bm.width >> 1),
          30,0,1);
        break;
      case 8:
      case 9:
      case 10:
      case 11:
        display_main_credit();
        break;
      case 13:
        if(intro_count == (13 << 5))
          load_scores();
      case 14:
      case 15:
      case 16:
        display_scores();
        break;
      case 17:
        drawxpm_ani(&ntitle_bm,
        SCREEN_HSIZE_X - (ntitle_bm.width >> 1),5,0,1);
        break;
      case 18:
        drawxpm_ani(&ntitle_bm,
        SCREEN_HSIZE_X - (ntitle_bm.width >> 1),5,0,1);
        scroll_base_ptr = scroll_story;
        scroll_pos = -(145 << 2);
        intro_count += 1 << 5;
        break;
      case 75:
        drawxpm_ani(&ntitle_bm,
        SCREEN_HSIZE_X - (ntitle_bm.width >> 1),5,0,1);
        break;
      case 76:
        if(intro_count == (76 << 5))
          load_scores();
      case 77:
      case 78:
      case 79:
        display_scores();
        break;
      case 80:
        drawxpm_ani(&ntitle_bm,
        SCREEN_HSIZE_X - (ntitle_bm.width >> 1),5,0,1);
        break;
      case 81:
        drawxpm_ani(&ntitle_bm,
        SCREEN_HSIZE_X - (ntitle_bm.width >> 1),5,0,1);
        scroll_base_ptr = scroll_keys;
        scroll_pos = -(145 << 2);
        intro_count += 1 << 5;
        break;
      case 136:
      case 137:
      case 138:
      case 139:
        display_main_credit();
        break;
      case 141:
        scroll_base_ptr = scroll_game_play;
        scroll_pos = -(145 << 2);
        intro_count += 1 << 5;
        break;
      case 275:
        drawxpm_ani(&ntitle_bm,
        SCREEN_HSIZE_X - (ntitle_bm.width >> 1),5,0,1);
        break;
      case 276:
        if(intro_count == (276 << 5))
          load_scores();
      case 277:
      case 278:
      case 279:
        display_scores();
        break;
      case 280:
        drawxpm_ani(&ntitle_bm,
        SCREEN_HSIZE_X - (ntitle_bm.width >> 1),5,0,1);
        break;
      case 281:
        drawxpm_ani(&ntitle_bm,
        SCREEN_HSIZE_X - (ntitle_bm.width >> 1),5,0,1);
        scroll_base_ptr = scroll_tips;
        scroll_pos = -(145 << 2);
        intro_count += 1 << 5;
        break;
      case 407:
      case 408:
      case 409:
      case 410:
        display_main_credit();
        break;
      case 412:
        scroll_base_ptr = scroll_misc;
        scroll_pos = -(145 << 2);
        intro_count += 1 << 5;
        break;
      case 561:
        drawxpm_ani(&ntitle_bm,
        SCREEN_HSIZE_X - (ntitle_bm.width >> 1),5,0,1);
        break;
      case 562:
        if(intro_count == (562 << 5))
          load_scores();
      case 563:
      case 564:
      case 565:
        display_scores();
        break;
      case 566:
        drawxpm_ani(&ntitle_bm,
        SCREEN_HSIZE_X - (ntitle_bm.width >> 1),5,0,1);
        break;
      case 567:
        drawxpm_ani(&ntitle_bm,
        SCREEN_HSIZE_X - (ntitle_bm.width >> 1),5,0,1);
        scroll_base_ptr = scroll_disclaimer;
        scroll_pos = -(145 << 2);
        intro_count += 1 << 5;
        break;
      case 639:
        drawxpm_ani(&ntitle_bm,
        SCREEN_HSIZE_X - (ntitle_bm.width >> 1),5,0,1);
        break;
      case 640:
        if(intro_count == (640 << 5))
          load_scores();
      case 641:
      case 642:
      case 643:
        display_scores();
        break;
      case 645:
        intro_count = 8 << 5;
        break;
      default:
        if((i >= 19) && (i < 75))
          draw_scroll();
        else if((i >= 81) && (i < 135))
          draw_scroll();
        else if((i >= 142) && (i < 275))
          draw_scroll();
        else if((i >= 282) && (i < 406))
          draw_scroll();
        else if((i >= 413) && (i < 561))
          draw_scroll();
        else if((i >= 568) && (i < 640))
          draw_scroll();
        break;
    }
    XCopyArea(display,render_screen,window,gc_bgblt,0,0,
      window_width,window_height,0,0);
    while(XPending(display))
    {
      XNextEvent(display,&event);
      if(event.xany.type == KeyPress)
      {
        register int ch;

        ch = XLookupKeysym((XKeyEvent *)&event,0);
        switch(ch)
        {
          case 'q':
            intro_f = 1;
            break;
          case ' ':
            intro_f = 2;
            break;
        }
      }
      else
        if(event.xany.type == ButtonPress)
          intro_f = 2;
    }
    usleep(UPDATE_DELAY);
  }
  bg_deinit();
  return intro_f - 1;
}

/***************************************************************************
*
***************************************************************************/
void get_ready(void)
{
  register int a;
  XEvent event;
  char str[STR_LABEL_LEN];

  for(a = 0;a < 3;a++)
  {
    XFillRectangle(display,render_screen,gc_bgblt,0,0,window_width,window_height);
    drawxpm_ani(&get_ready2_bm,SCREEN_HSIZE_X - (get_ready2_bm.width >> 1),
      10,0,1);
    XSetForeground(display,gc_dline,white_pixel[6]);
    sprintf(str,"Starship %s",ship->name);
    XDrawString(display,render_screen,gc_dline,
    SCREEN_HSIZE_X - ((strlen(str) >> 1) * 7),55,str,strlen(str));
    drawxpm_ani(&ship->map_bm,SCREEN_HSIZE_X - (ship->map_bm.width >> 1),
      70,0,1);
    XCopyArea(display,render_screen,window,gc_bgblt,0,0,window_width,window_height,0,0);
    while(XPending(display))
      XNextEvent(display,&event);
    sleep(1);
  }
}

/***************************************************************************
*
***************************************************************************/
void bg_calc(int sig)
{
  signal(sig,&bg_calc);
  if(ship != NULL)
    ship->level_bg_calc();
  sscore += score;
  sscore >>= 1;
}

void game(void)
{
  XEvent event;

  init();
  init_colours();
  if(load_flr_xpms())
  {
    if(load_sprite_xpms())
    {
      XMapWindow(display,window);
      XSync(display,0);
      printf("Entering Nighthawk..\n");
      for(;;)
        if(draw_intro())
        {
          register int dead = 0;
          sscore = score = 0;

          ship_ptr = ship_start;
          while(!dead)
          {
            ship = new(tship);
            if(ship != NULL)
            {
              if(ship->load(ship_table[ship_ptr]))
              {
                get_ready();
                bg_init(&bg_calc);
                ship->level_run();
                if(ship->droids[0]->state == 2)
                {
                  dead = 1;
                  update_score(0);
                }
                ship->unload();
                bg_deinit();
              }
              delete(ship);
            }
            else
              break;
            ship_ptr++;
            if((ship_table[ship_ptr] == NULL) && !ship->droids[0]->state)
            {
              tbm finale_bm;

              ship_ptr--;
              update_score(1);
              if(loadxpm_xpm("standard/finale.xpm",&finale_bm))
              {
                register int a;

                for(a = 0;a < 8;a++)
                {
                  drawxpm_ani(&finale_bm,0,0,0,1);
                  XCopyArea(display,render_screen,window,gc_bgblt,0,0,
                    window_width,window_height,0,0);
                  while(XPending(display))
                    XNextEvent(display,&event);
                  sleep(1);
                }
                free_bm(&finale_bm);
              }
              break;
            }
          }
        }
        else
          break;
      free_sprite_xpms();
    }
    free_flr_xpms();
  }
}

/***************************************************************************
*
***************************************************************************/
void display_scroll(char *data)
{
  register int x,lines;

  lines = (int)strlen(data) / 36;
  for(x = 0;x < lines;x++)
  {
    register int a;

    printf("                    ");
    for(a = 0;a < 36;a++)
    {
      putchar(*data);
      data++;
    }
    putchar('\n');
  }
  printf("\n\n\n");
}

/***************************************************************************
*
***************************************************************************/
int main(int argc,char *argv[])
{
  register int x;

  printf(WELCOME);
  for(x = 1;x < argc;x++)
  {
    if(argv[x][0] == '-')
    {
      switch(argv[x][1])
      {
        case 'h':
          printf(
            "Usage: %s <options>\n"
            " -h        Display this help info\n"
            " -s        Display game scrolls\n"
            " -c x      Start at a given ship (0-9)\n"
            "\n",argv[0]);
          return 1;
        case 's':
          printf("\n\n\n");
          display_scroll(scroll_story);
          display_scroll(scroll_keys);
          display_scroll(scroll_game_play);
          display_scroll(scroll_tips);
          display_scroll(scroll_misc);
          display_scroll(scroll_disclaimer);
          return 1;
        case 'c':
          if(argc == 3)
          {
            ship_start = atoi(argv[x + 1]);
            if(ship_start < 0) ship_start = 0;
            if(ship_start > 9) ship_start = 9;
            printf("Starting at level '%s' (Nb/ High scores will note "
                   "that you have cheated)\n",ship_table[ship_start]);
          }
          else
            printf("Error: -c, missing parameter\n");
          break;
      }
    }
  }
  printf("Data Directory: " INSTALL_LIB_DIR "/\n"
         "No sound.\n");
  if(x_init())
  {
    game();
    Xreaper();
  }
  printf("See ya\n");
  return 1;
}
