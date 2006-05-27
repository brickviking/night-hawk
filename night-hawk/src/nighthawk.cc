/* -*- mode: C++; tab-width:2 -*-
****************************************************************************
****************************************************************************
*
* NightHawk - By Jason Nunn - Oct 1996  (email viking for details)
* Eric Gillespie and others - Jan 2003  (viking667 at users dot sourceforge dot net)
* Copyright GPL version 2, 2003.
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
#include <pwd.h>
#include <sys/types.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <time.h>
#include <sys/time.h>
#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/xpm.h>
#include <X11/keysym.h>
}
#include "nighthawk_defs.h"
#include "misc.h"
#include "tship_demo.h"

tship        *ship = NULL;

struct       itimerval oldtimer,timer;
tscore_table score_table[MAX_SCORE_TABLE];
int          ship_ptr,ship_start = 0;

// nighthawk is assumed to have a max of 10 ships . no more unless you
// make space
char         *ship_table[MAX_SHIPS + 1] =
  {NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL};

int intro_module_count,intro_sequence_no,intro_back_bm_no;

static char* scores_file = NULL;

/***************************************************************************
 *
 ***************************************************************************/
void kill_pipe(int)
{
  // This is somewhat inaccurate, as NightHawk shows this message if it can't
  // start the engine, as well as when the engine dies.
  printf("Nb/ Sound engine just died.\n");
  sound_engine_fp = NULL;
}

void init(void)
{
  register int x;

  signal(SIGPIPE,kill_pipe);
  umask(~0666);
  for(x = 0;x < MAX_SCORE_TABLE;x++)
		{
			strcpy(score_table[x].name,"-");
			strcpy(score_table[x].highest_ship,"-");
			strcpy(score_table[x].time,"-");
			score_table[x].score = 0;
		}
}

int get_ctrl_file(void)
{
  FILE *fp;

  printf("Loading Ship List..\n");
  fp = fopen(INSTALL_DIR "/data/ctrl.d","r");
  if(fp != NULL)
		{
			register int x = 0;

			for(;;)
				{
					char str[STR_LABEL_LEN + 1],*s;

					str[0] = 0;
					if(fgets(str,STR_LABEL_LEN,fp) == NULL)
						break;
					str[strlen(str) - 1] = 0;
					ship_table[x] = (char *)malloc(strlen(str) + 1);
					if(ship_table[x] != NULL)
						{
							strcpy(ship_table[x],str);
							x++;
						}
				}
			fclose(fp);
		}
  else
		{
			perror("get_ctrl_file()");
			return 0;
		}
  return 1;
}

/***************************************************************************
 *
 ***************************************************************************/
FILE *lopen(char *filename,char *mode)
{
  FILE *fp;
  int retval;

  if((fp = fopen(filename,mode)) != NULL)
/* ECG: 'nother bug nailed by the FreeBSD porters - only thing NOW puzzling me is - where does "flock() come from? */					
/*		flock(fileno(fp),LOCK_EX);  */
    /* TODO: I have to sanity-check this for errors: namely EAGAIN */
  retval=lockf(fileno(fp), F_LOCK, 0);
  switch(retval) {
    case 0: // she works, let's get out of here
      break;
    default: // anything else is an error, let's fall out, saying we can't lock the scorefile
      break;
  }
  return fp;
}

//added: W.Scherer 4/99
char *
which_scores_file (void)
{
  if (scores_file == NULL)
		{
			scores_file = SCORES_FILE;
			if (scores_file[0] != '/')
				{
					char *home = getenv ("HOME");
					if (home)
						{
							scores_file = (char *) malloc (strlen (home) + strlen (SCORES_FILE) + 2);
							if (scores_file)
								{
									sprintf (scores_file, "%s/%s", home, SCORES_FILE);
								}
							else
								{
									scores_file = SCORES_FILE;
								}
						}
				}
		}
  return scores_file;
}

FILE *load_scores_sub(void)
{
  FILE *fp;

  fp = lopen(which_scores_file(),"r+b");
  if(fp != NULL)
		{
			rewind(fp);
			fread(score_table,sizeof(tscore_table),MAX_SCORE_TABLE,fp);
			return fp;
		}
  else
		{
			fp = lopen(which_scores_file(),"w+b");
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

  intro_module_count = 0;
  intro_sequence_no = 20;
  fp = load_scores_sub();
  if(fp != NULL)
		{
			register int x;

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
 * Well, look what we've found here - a system timer
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

/****************************
 *
 *****************************/
void pause_1_bg(void)
{
  intro_module_count++;
  if(intro_module_count > 73)
		{
			intro_module_count = 0;
			intro_sequence_no++;
			intro_back_bm_no = random() & 0x3;
		}
}

void pause_2_bg(void)
{
  intro_module_count++;
  if(intro_module_count > 35)
		{
			intro_module_count = 0;
			intro_sequence_no++;
			intro_back_bm_no = random() & 0x3;
		}
}

void pause_3_bg(void)
{
  intro_module_count++;
  if(intro_module_count > 80)
		{
			intro_module_count = 0;
			intro_sequence_no++;
			intro_back_bm_no = random() & 0x3;
		}
}

void blank_fg(void)
{
  XFillRectangle(display,render_screen,gc_bgblt,
								 0,0,window_width,window_height);
}

void credit_1_fg(void)
{
  XFillRectangle(display,render_screen,gc_bgblt,
								 0,0,window_width,window_height);
  drawxpm_ani(&credit_1_bm,
							SCREEN_HSIZE_X - (credit_1_bm.width >> 1),
							40,0,1);
}

void credit_2_fg(void)
{
  XFillRectangle(display,render_screen,gc_bgblt,
								 0,0,window_width,window_height);
  drawxpm_ani(&credit_2_bm,
							SCREEN_HSIZE_X - (credit_2_bm.width >> 1),
							40,0,1);
}

void credit_3_fg(void)
{
  XFillRectangle(display,render_screen,gc_bgblt,
								 0,0,window_width,window_height);
  drawxpm_ani(&credit_3_bm,
							SCREEN_HSIZE_X - (credit_3_bm.width >> 1),
							40,0,1);
}

void credit_4_fg(void)
{
  XFillRectangle(display,render_screen,gc_bgblt,
								 0,0,window_width,window_height);
  drawxpm_ani(&credit_4_bm,
							SCREEN_HSIZE_X - (credit_4_bm.width >> 1),
							40,0,1);
}

void credit_5_fg(void)
{
  XFillRectangle(display,render_screen,gc_bgblt,
								 0,0,window_width,window_height);
  drawxpm_ani(&credit_5_bm,
							SCREEN_HSIZE_X - (credit_5_bm.width >> 1),
							40,0,1);
}

void main_logo_fg(void)
{
  char *mess =
    "5(C) 97,98,1999 Jason Nunn (JsNO)\n"
    "5(C) 2003 Eric Gillespie (viking667)\n"
    "5http://night-hawk.sourceforge.net/\n"
    "5\n"
    "5'Rescue from Vega' (C) 1998, Vincent Voois (Vv)\n"
    "5home.worldonline.nl/~vvacme\n"
    "5'Introtune' (Demo tune) by 4-matt of Anarchy\n"
    "7(used without permission)\n";

  drawxpm_ani(&credit_6_bm,
							SCREEN_HSIZE_X - (credit_6_bm.width >> 1),
							20,0,1);
  display_message(mess,67);
}

void main_logo_fg2(void)
{
  char *mess =
    "0Space to play, 'q' to Quit,\n"
    "0'g' to lock mouse to game,\n"
    "0or 'd' for a demo.\n";

  main_logo_fg();
  display_message(mess,170);
}

void credit_6_fg(void)
{
  XFillRectangle(display,render_screen,gc_bgblt,
								 0,0,window_width,window_height);
  main_logo_fg();
}

void credit_7_fg(void)
{
  XFillRectangle(display,render_screen,gc_bgblt,
								 0,0,window_width,window_height);
  main_logo_fg2();
}

void credit_8_fg(void)
{
  drawxpm_ani(&intro_back_bm[intro_back_bm_no],0,0,0,1);
  main_logo_fg2();
}

/****************************
 *
 *****************************/
void display_scores_bg(void)
{
  intro_module_count++;
  if(intro_module_count > 80)
		{
			intro_module_count = 0;
			intro_sequence_no++;
			intro_back_bm_no = random() & 0x3;
		}
}

void display_scores_fg(void)
{
  char str[STR_LEN];
  register int x;

  drawxpm_ani(&intro_back_bm[intro_back_bm_no],0,0,0,1);
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

/****************************
 *
 *****************************/
#include "scroll.inc"
char *scroll_base_ptr;
int scroll_limit;

void draw_scroll_bg(void)
{
  intro_module_count += 3;
  if(intro_module_count > scroll_limit)
		{
			intro_module_count = 0;
			intro_sequence_no++;
			intro_back_bm_no = 0;
		}
}

void draw_scroll_init_1_fg(void)
{
  scroll_base_ptr = scroll_story;
  intro_module_count = -(190 << 2);
  intro_sequence_no++;
  scroll_limit = 1375;
}

void draw_scroll_fg(void)
{
  register int a,b,c,d;
  int co_tab[16] = {8,7,6,5,4,3,2,1,1,2,3,4,5,6,7,8};

  XFillRectangle(display,render_screen,gc_bgblt,
								 0,0,window_width,window_height);
  d = strlen(scroll_base_ptr);
  b = (intro_module_count >> 2) / 12;
  c = (intro_module_count >> 2) % 12;
  for(a = 0;a < 16;a++)
		{
			register int p;

			p = (a + b) * 36;
			if((p >= 0) && (p < d))
				{
					XSetForeground(display,gc_dline,green_pixel[co_tab[a]]);
					XDrawString(display,render_screen,gc_dline,
											55,20 + (a * 12) - c,scroll_base_ptr + p,36);
				}
		}
  XSetForeground(display,gc_dline,red_pixel[7]);
  XFillRectangle(display,render_screen,gc_bgblt,
								 55,0,220,12);
  XFillRectangle(display,render_screen,gc_bgblt,
								 55,200 - 12,220,12);
}

/****************************
 *
 *****************************/
void contacts_1_fg(void)
{
  char *mess =
    "0GPL\n"
    "4\n"
    "4Nighthawk   comes   under  the   GNU\n"
    "4GENERAL PUBLIC LICENSE AGREEMENT,and\n"
    "4comes  with  ABSOLUTELY NO WARRANTY.\n"
    "4This is free software,  and  you are\n"
    "4welcome  to  redistribute  it  under\n"
    "4certain conditions.\n"
    "4A copy of the  GPL  should have come\n"
    "4in a file called COPYING distributed\n"
    "4with this game.\n";

  drawxpm_ani(&intro_back_bm[intro_back_bm_no],0,0,0,1);
  display_message(mess,40);
}

void contacts_2_fg(void)
{
  char *mess =
    "0=Contacts=\n"
    "4\n"
    "4-Maintainers-\n"
    "1<viking667 at users dot sourceforge dot net>\n"
    "1<linuxfalcon at users dot sourceforge dot net\n"
    "4\n"
    "4-Testers-\n"
    "1<dionb at users dot sourceforge dot net>\n"
    "1<zerodogg at users dot sourceforge dot net\n"
    "1\n"
    "4-Website-\n"
    "1http://night-hawk.sourceforge.net\n"
    "4\n";
	// Put other developers in here

  drawxpm_ani(&intro_back_bm[intro_back_bm_no],0,0,0,1);
  display_message(mess,40);
}

void contacts_3_fg(void)
{
  char *mess =
    "0=Contacts=\n"
    "4\n"
    "4-Vincent Voois (Musician)-\n"
    "1<vvacme@worldonline.nl>\n"
    "1(Address may no longer be current)\n"
    "1http://home.worldonline.nl/~vvacme\n"
    "1(Address may no longer be current)\n"
    "1(we hope there will be more neat\n"
    "1music and sound effects to come soon)\n"
    "4\n"
    "4Adrian Bridgett (Debian Maintainer)\n"
    "1<bridgett at debian dot org>\n";

  drawxpm_ani(&intro_back_bm[intro_back_bm_no],0,0,0,1);
  display_message(mess,40);
}

void contacts_4_fg(void)
{
  char *mess =
    "4Thanks to the  many  people who have\n"
    "4emailed  feedback  about  Nighthawk.\n"
    "4\n"
    "1Support Free Software!\n"
    "4\n"
    "1Thanks also to Sourceforge.net\n"
    "1for hosting this fantastic project\n"
    "1and many many others, including\n"
    "1paradroid clones well worth a play\n"
    "1when you have moments of nostalgia\n"
    "1http://reedroid.sourceforge.net/\n"
    "1http://paradroid.sourceforge.net/\n"
    "4\n"
    "4Hit Space to Start.\n"
    "4Hit 'g' to lock mouse to screen.\n"
    "4Hit 'd' for demo.\n";

  drawxpm_ani(&intro_back_bm[intro_back_bm_no],0,0,0,1);
  display_message(mess,40);
}

/****************************
 *
 *****************************/
typedef struct
{
  void (*bg)(void);
  void (*fg)(void);
} tintro_tab;

tintro_tab intro_tab[] = {
  {pause_3_bg,blank_fg},
  {pause_1_bg,credit_1_fg},
  {pause_2_bg,blank_fg},
  {pause_1_bg,credit_2_fg},
  {pause_2_bg,blank_fg},
  {pause_1_bg,credit_3_fg},
  {pause_2_bg,blank_fg},
  {pause_1_bg,credit_4_fg},
  {pause_2_bg,blank_fg},
  {pause_1_bg,credit_5_fg},
  {pause_2_bg,blank_fg},
  {pause_2_bg,credit_6_fg},
  {pause_3_bg,credit_7_fg},
  {draw_scroll_bg,draw_scroll_init_1_fg},
  {draw_scroll_bg,draw_scroll_fg},
  {pause_3_bg,credit_8_fg},
  {pause_3_bg,contacts_1_fg},
  {pause_3_bg,contacts_2_fg},
  {pause_3_bg,contacts_3_fg},
  {pause_3_bg,contacts_4_fg},
  {display_scores_bg,display_scores_fg},
  {NULL,NULL}
};

void draw_intro_bg(int sig)
{
  signal(sig,&draw_intro_bg);
  if(intro_tab[intro_sequence_no].bg != NULL)
    intro_tab[intro_sequence_no].bg();
  else
    intro_sequence_no = 13;
}

void do_fade_out(void)
{
  register int x;
  XEvent event;

  for(x = 0;x < 280;x += 8)
		{
			XFillRectangle(display,window,gc_bgblt,
										 0,0,window_width,window_height);
			while(XPending(display))
				XNextEvent(display,&event);
			if(x < 255)
				sound_engine_cmd(SND_CMD_FADE_OUT,0,0,0);
			usleep(UPDATE_DELAY);
		}
}

int draw_intro(void)
{
  register int intro_f = 0;
  XEvent event;

  intro_module_count = 0;
  intro_sequence_no = 0;
  intro_back_bm_no = 0;
  load_scores();
  bg_init(&draw_intro_bg);
  sound_engine_cmd(SND_CMD_LOAD_SONG,0,0,0);
  while(!intro_f)
		{
			if(intro_tab[intro_sequence_no].fg != NULL)
				intro_tab[intro_sequence_no].fg();
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
								case KEY_GRAB:
									nighthawk_mousegrab();
									break;
								case KEY_QUIT:
									intro_f = 1;
									break;
								case KEY_SELECT:
									intro_f = 2;
									break;
								case KEY_DEMO:
									intro_f = 3;
									break;
								}
						}
					else
						if(event.xany.type == ButtonPress)
							intro_f = 2;
				}
			usleep(UPDATE_DELAY);
		}
  do_fade_out();
  sound_engine_cmd(SND_CMD_UNLOAD_SONG,0,0,0);
  bg_deinit();
  return intro_f;
}

/***************************************************************************
 *
 ***************************************************************************/
tship_demo *ship_demo = NULL;

void bg_calc_demo(int sig)
{
  signal(sig,&bg_calc_demo);
  if(ship_demo != NULL)
    ship_demo->level_bg_calc();
  if(score != sscore)
    sscore += ((score - sscore) >> 1) | 1;
}

void do_demo(void)
{
  srandom(0);
  kevent_buf_ptr = 0;
  demo_count = 0;
  ship_demo = new(tship_demo);
  if(ship_demo != NULL)
		{
			if(ship_demo->load(ship_table[5]))
				{
					bg_init(&bg_calc_demo);
					sound_engine_cmd(SND_CMD_LOAD_SONG_DEMO,0,0,0);
					ship_demo->level_run();
					ship_demo->unload();
					do_fade_out();
					sound_engine_cmd(SND_CMD_UNLOAD_SONG,0,0,0);
					bg_deinit();
				}
			delete(ship_demo);
		}
}

/***************************************************************************
 *
 ***************************************************************************/
void get_ready(void)
{
  register int a,siren_r = 0,siren_c = 0;
  XEvent event;
  char str[STR_LABEL_LEN];

  for(a = 0;a < (4 * 8);a++)
		{
			if(siren_c < 3)
				if(!siren_r)
					{
						siren_r = 6;
						siren_c++;
						sound_engine_cmd(SND_CMD_FX,FX_ALERT_SIREN,0xff,0x80);
						sound_engine_cmd(SND_CMD_FX,FX_ALERT_SIREN,0xff,0x80);
					}
				else
					siren_r--;
			XFillRectangle(display,render_screen,gc_bgblt,0,0,
										 window_width,window_height);
			drawxpm_ani(&get_ready1_bm,SCREEN_HSIZE_X - (get_ready1_bm.width >> 1),
									12,0,1);
			XSetForeground(display,gc_dline,white_pixel[6]);
			sprintf(str,"Starship %s",ship->name);
			XDrawString(display,render_screen,gc_dline,
									SCREEN_HSIZE_X - ((strlen(str) >> 1) * 7),55,str,strlen(str));
			drawxpm_ani(&ship->map_bm,SCREEN_HSIZE_X - (ship->map_bm.width >> 1),
									70,0,1);
			XCopyArea(display,render_screen,window,gc_bgblt,0,0,window_width,window_height,0,0);
			while(XPending(display))
				XNextEvent(display,&event);
			usleep(125000);
		}
}

/***************************************************************************
 *
 ***************************************************************************/
void display_final_score(void)
{
  char str[STR_LEN];
  register int sl;

  sprintf(str,"Your Final Score: %d",score);
  XSetForeground(display,gc_dline,green_pixel[2]);
  sl = strlen(str);
  XDrawString(display,render_screen,gc_dline,
							SCREEN_HSIZE_X - ((sl * 6) >> 1),180,str,sl);
}

void trans_terminated(void)
{
  register int a;
  XEvent   event;

  sound_engine_cmd(SND_CMD_RANDOM,0,0,0);
  for(a = 0;a < 30;a++)
		{
			register int x,y,ww,wh,ww_inc = 0,wh_inc = 0;

			ww = window_width >> 5;
			wh = window_height / 20;
			for(y = 0;y < 20;y++)
				{
					ww_inc = 0;
					for(x = 0;x < 32;x++)
						{
							XSetForeground(display,gc_line,white_pixel[random() & 0x7]);
							XFillRectangle(display,render_screen,gc_line,ww_inc,wh_inc,ww,wh);
							ww_inc += ww;
						}
					wh_inc += wh;
				}
			XCopyArea(display,render_screen,window,gc_bgblt,0,0,window_width,window_height,0,0);
			while(XPending(display))
				XNextEvent(display,&event);
			usleep(UPDATE_DELAY);
		}
  sound_engine_cmd(SND_CMD_SILENCE,0,0,0);
  sound_engine_cmd(SND_CMD_FX,FX_TRANS_TERM,0xff,0x80);
  for(a = 0;a < (4 * 2);a++)
		{
			XFillRectangle(display,render_screen,gc_bgblt,0,0,window_width,window_height);
			drawxpm_ani(&trans_terminated_bm,
									SCREEN_HSIZE_X - (trans_terminated_bm.width >> 1),50,0,1);
			display_final_score();
			XCopyArea(display,render_screen,window,gc_bgblt,0,0,window_width,window_height,0,0);
			while(XPending(display))
				XNextEvent(display,&event);
			usleep(500000);
		}
}

/***************************************************************************
 *
 ***************************************************************************/
void finale(void)
{
  XEvent event;
  register int a;
  char *mess =
    "1\n"
    "1   You have completed all levels!\n"
    "1\n"
    "1\n"
    "1  Thank you for playing Nighthawk.\n\0";

  update_score(1);
  for(a = 0;a < (8 * 2);a++)
		{
			XFillRectangle(display,render_screen,gc_bgblt,
										 0,0,window_width,window_height);
			display_message(mess,40);
			display_final_score();
			XCopyArea(display,render_screen,window,gc_bgblt,0,0,
								window_width,window_height,0,0);
			while(XPending(display))
				XNextEvent(display,&event);
			usleep(500000);
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
  if(score != sscore)
    sscore += ((score - sscore) >> 1) | 1;
}

void game(void)
{
  if(!get_ctrl_file())
    return;
#ifndef DEVELOPMENT_MODE
  if(load_demo())
		{
#endif
      // This condition test is incorrect, as x_init errors with -1
//BAD:if(x_init())
      if(1 == x_init())
				{
					init();
					init_colours();
					printf("Loading sprites..\n");
					if(load_flr_xpms())
						{
							if(load_sprite_xpms())
								{
									XMapWindow(display,window);
									XSync(display,0);
									printf("Entering Nighthawk..\n");
									for(;;)
										{
											register int intro_f;

											intro_f = draw_intro();
											if(intro_f == 2)  /* Let's play!!! */
												{
													register int dead = 0;

													sound_engine_cmd(SND_CMD_FX_INIT,0,0,0);
													sscore = score = 0;
													ship_ptr = ship_start;
													while(!dead)
														{
															srandom(12338);
															ship = new(tship);
															if(ship != NULL)
																{
																	if(ship->load(ship_table[ship_ptr]))
																		{
																			get_ready();
																			bg_init(&bg_calc);
																			ship->level_run();
																			ship->unload();
																			bg_deinit();
																			if(ship->droids[0]->state == 2)
																				{
																					trans_terminated();
																					dead = 1;
																					update_score(0);
																				}
																		}
																	delete(ship);
																}
															else
																break;
															ship_ptr++;
															if((ship_table[ship_ptr] == NULL) && !dead)
																{
																	ship_ptr--;
																	finale();
																	break;
																}
														}
													sound_engine_cmd(SND_CMD_UNLOAD_SONG, 0, 0, 0);
												}
											else
												if(intro_f == 3)
													do_demo();
												else
													break;
										}
									free_sprite_xpms();
								}
							free_flr_xpms();
						}
					Xreaper();
				}
#ifndef DEVELOPMENT_MODE
			free_demo_buf();
		}
#endif
}

/***************************************************************************
 *
 ***************************************************************************/
int main(int argc,char *argv[])
{
  register int x,sound = 0;
  char sound_param[STR_LEN] = FUNKPLAY_PATH;
	/* Whoops - what happens if we have a REALLY LONG PATH? */
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
										 " -h         Display this help info\n"
										 " -c<d>      Start at a given ship (0-9)\n"
										 " -m         Sound and music\n\n"
										 "'-m' options:\n"
										 "  -p<d>   No of Patterns alloc\n"
										 "  -s<d>   Sampling rate\n"
										 "  -S<d>   Stereo    (S1=Stereo,S0=Mono)\n"
										 "  -P<d>   Precision (P8=8 bit,P16=16 bit)\n"
										 "  -x<d>   No of FX channels\n"
										 "\n",argv[0]);
							return 1;
						case 'c':
							ship_start = atoi(argv[x] + 2);
							if(ship_start < 0) ship_start = 0;
							if(ship_start > 9) ship_start = 9;
							printf("Starting at level %d (Nb/ High scores will note "
										 "that you have cheated)\n",ship_start);
							break;
						case 'm':
							sound = 1;
							break;
						case 'p':
						case 's':
						case 'S':
						case 'P':
						case 'x':
							strcat(sound_param," ");
							strcat(sound_param,argv[x]);
							break;
						}
				}
		}
  printf(" Data Directory: " INSTALL_DIR "/data/\n"
         "Score Directory: %s\n", which_scores_file());
#ifdef DEVELOPMENT_MODE
  printf("YOU'RE IN DEVELOPMENT MODE!!!!!\n"
         " Which means:\n"
         "  - Floor editor class included\n"
         "  - saves keyboard events during play (for demos)\n\n"
         "THIS IS NOT A USER MODE!\n");
#endif
#ifdef REDUCED_SPRITES
  printf("Reduced sprites mode (for slow machines)\n");
#endif
  // We should try starting the X layer FIRST, 
  // if that succeeds, THEN we can start the sound engine.
  if(sound)
		{
			fflush(stdout);
			sound_engine_fp = popen(sound_param,"w");
			if(sound_engine_fp != NULL)
				{
					game();
					sound_engine_cmd(SND_CMD_QUIT,0,0,0);
					if(sound_engine_fp != NULL)
						pclose(sound_engine_fp);
				}
			else
        {
				printf("Error: Couldn't run sound engine, running without sound.\n");
      game();
        }
			/* The question here is:  do we continue without sound and 
			 * play anyway?  Or do we abort like we used to? 
			 */
		}
  else
		{
			printf("Sound not requested.\n");
			game();
		}
  printf("See ya\n");
  return 1;
}
/* 
	 vim:tw=0:ww=0:ts=2 
*/
