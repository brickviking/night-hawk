/* Included with list, split off on advice from bronaugh
 * Now all I have to do is include all the needed includes
 */ 
#include <stdio.h>
#include <stdlib.h>
#ifndef _MINIX  /* because MINIX hasn't got a basename() */
#include <libgen.h> /* ...but everything else I know, has a basename() */
#endif 
#include <ctype.h>
#include <curses.h>
#include <menu.h>
#include <dirent.h>
#include <sys/stat.h>
#include "list.h"

void Usage(void) {
	/* ECG: 9-Oct-2001
	 * This has been pointed out as not needed by bronaugh - we should
	 * make it non-curses-reliant - done.
	 * Hmmm, just wondering if we could integrate this into Bye somehow?
	 * Say, call it from Bye? After all, we DO exit the program at this stage.
	 * Any other notes I will add to head of list.c
	 */
  char *tempstring;
  if(debug) {
    if(NULL == (tempstring = (char *) malloc(sizeof(tempstring) * 128)))   /* We shouldn't need more than this */
      Bye(1, __LINE__); /* .. and if we can't get it, then we bail out... */
    sprintf(tempstring, "Debugging version %1d.%1d.%1d %s %s build\n", LISTVERSION, LISTMAJOR, LISTMINOR,  __DATE__, __TIME__ );
    printf(tempstring);
  }
  printf("List, by Eric Gillespie, released under the GNU Public License version 2\n");
  printf("Usage: list [-?|-h] [-v] [-d] [-x|t] [-7|8] [-s start] [-n count]\n\t");
/*  printf("[-b tabsize] "); */
  printf("[-l] [-e] [-m] [-f] file_name [-r] rows [-c] columns [-q] \n");
  printf("\t ?/h this help\t v: version\t d: Debug mode (very slow)\n");
  printf("\t q: Quickmode (no textmode - just hex)\n");
  printf("\t (*)start: bytes offset before printing file\n");
  printf("\t (*)count: number of bytes to read\n");
/*  printf("\t tabsize: size a Tab char (09) takes up\n"); */
  printf("\t x: Hexadecimal dump mode");
  printf("\t t: Text dump mode (like less)\n");
  printf("\t 7: 7-bit ASCII (Unix-like)");
  printf("\t 8: 8-bit ASCII (MS-DOS-like)\n");
  printf("\t l: line numbers (in text mode)");
  printf("\t e: show line endings\n");
  printf("\t m: Mono version");
  printf("\t f: specify filename to open\n");
  printf("\t r: rows to use, c: columns to use\n");
  printf("\t (*) means I haven't finished coding this bit yet\n");
/*  printf("Hit any key to exit\n"); / * Added by Dion's bugfinding, taken out
 *  in the change from curses to tty */
  /* printf("\t a: Assembler dump mode");  AT&T or Intel? */
}

void ShowHelp(struct FileData *fi) {
/* Notes: So far I have had to make sure that this screen is only 80 ch wide... *
 * and now I'm going to see if I can stick it in the middle of the screen vertically *
 * I also have to worry about ALT_CHARSET, because that doesn't display well in an   *
 * xterm for some unknown reason - possibly to do with font mapping               *
 * I have also had the idea of displaying a second help screen - seeing as the
 * first is keys, the second can be for search + description.
 */
	/* This help screen layout has been suggested by bronaugh
	 *
                               +-----+------+------------------------+-----+------+------------------------+ 
                               | Key | Mode |         Action         | Key | Mode |         Action         |
                               +-----+------+------------------------+-----+------+------------------------+ 
                               | Z/z | All  | Toggle debug flag      | H/h | All  | Show help screen       |
                               | T/t | Hex  | Switches to text mode  | X/x | Text | Switch to hex mode     |
                               | /   | All  | Search forward         | \   | All  | Search backwards       |
                               | n   | All  | Repeats search         | e   | All  | Edit with ext. editor  |
                               | S   | All  | Statistics on file     | w   | All  | Write out bug report   |
                               | O/o | Hex  | Address in Hex or Dec. | Q/q | All  | Quit (Hasta La Vista!) |
                               | 7   | Text | Toggle printable chars | 8   | Text | Toggle all chars       |
                               | *   | Text | Show line ends         | l   | Text | Toggle line numbers    |
                               +-----+------+------------------------+-----+------+------------------------+ 
 
                               +-----+------+------------------------+-----+------+------------------------+ 
                               | Key | Mode |         Action         | Key | Mode |         Action         |
                               +-----+------+------------------------+-----+------+------------------------+ 
                               | B   | Text | Move back one page     | B   | Hex  | Move back 2072 bytes   |
                               | Up  | Text | Move back one line     | Up  | Hex  | Move back 37 bytes     |
                               | Dn  | Text | Move forward one line  | Up  | Hex  | Move forward 37 bytes  |
                               | Sp  | Text | Move forward one page  | Sp  | Hex  | Move forward 2072 bytes|
                               | H   | All  | Move to start of file  | E   | Hex  | Move to end of file    |
                               | G   | Text | Jump to line in file   | G   | Hex  | Jump to byte in file   |
                               | %   | All  | Move to n% offset file |     |      |                        |
                               +-----+------+------------------------+-----+------+------------------------+ 
	 * */
/* char tmpstr[COLS+1]; */
  char *tempstring=(char *) malloc(sizeof(tempstring) * 1024);  /* To give us some room */
  int c, middle, x, y, EightBitDisplay;
  WINDOW *HelpWin;
  if(fi->Scrn_x < 80 || fi->Scrn_y < 25)  { /* This shouldn't be a show stopper */
    debug_function("Cannot display help - screen too small", 0, fi->Scrn_y, __LINE__);
    refresh();
    return;
  }
/* LINES is supplied by ncurses, but this would change on a SIGWINCH */
  middle = (((fi->Scrn_y > 24) ? fi->Scrn_y : 24) / 2) ;
/* Falls over if LINES is < 24, so we provide some protection */
  x = 0; y = 0;
  HelpWin = newwin(24, 80, (middle - 12), 0);
/*  wclear(HelpWin); */
  if(!fi->Monochrome)  {
    wbkgd(HelpWin, ' ' | COLOR_PAIR(COLOR_BLUE));
  }
  else {
    wbkgd(HelpWin, A_BOLD);
  }
#ifdef __linux__
 /* I really really shouldn't have to do this... but BSD being what it is... */
  if(getenv("DISPLAY")==NULL)
    EightBitDisplay=1;
  else
#endif
    EightBitDisplay=0; /* This is to sort out those cases in which I don't
                 * have Linux on board - it's a safety measure */
  if(EightBitDisplay)
    if(!fi->Monochrome)  {
      wattron(HelpWin, A_ALTCHARSET | COLOR_PAIR(COLOR_YELLOW) | A_BOLD);
    }
    else  {  /* We're in mono */
      wattron(HelpWin, A_ALTCHARSET |  A_BOLD);
    }
  else { /* Can't use ALT_CHARSET */
    if(!fi->Monochrome) {
      wattron(HelpWin, COLOR_PAIR(COLOR_YELLOW) | A_BOLD);
    }
    else {  /* We're in mono */
      wattron(HelpWin,  A_BOLD);
    }
  }
/* All these wmove/waddstr's can be replaced by a single call! *
 * mvwaddstr(WIN, y, x, string) */
/*  wmove(HelpWin, 0,18); */
  /*   standout(); */
  x = 1; /* Starts "list" in the middle, increment y each line */
  if(EightBitDisplay) {
    mvwaddstr(HelpWin, y++, x, "                 °°   °°                °°                                  ");  /* Ln 0 */
    mvwaddstr(HelpWin, y++, x, "                 °°                     °°                                  ");  /* Ln 1 */
    mvwaddstr(HelpWin, y++, x, "                 ±±   ±±   Ü±±±±±±Ü  ±±±±±±±±         By                    ");  /* Ln 2 */
    mvwaddstr(HelpWin, y++, x, "                 ±±   ±±   ±      ß     ±±       Eric Gillespie             ");  /* Ln 3 */
    mvwaddstr(HelpWin, y++, x, "                 ²²   ²²    ²²²²²²      ²²                                  ");  /* Ln 4 */
    mvwaddstr(HelpWin, y++, x, "                 ²²   ²²   Ü      ²     ²²                                  ");  /* Ln 5 */
    mvwaddstr(HelpWin, y++, x, "                 ÛÛ   ÛÛ   ßÛÛÛÛÛÛß     ÛÛ                                  ");  /* Ln 6 */
    mvwaddstr(HelpWin, y++, x, "                 ÛÛ                                                         ");  /* Ln 7 */
    mvwaddstr(HelpWin, y, x,   "                 ÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛ                               ");  /* Ln 8 */
  }
  else {
    mvwaddstr(HelpWin, y++, x, "   aawaawaa             _aasasa.                       .wwwaww,            ");
    mvwaddstr(HelpWin, y++, x, "   mWWWQWW#    By       jQWWWWW:                       :QWWWQW(            ");
    mvwaddstr(HelpWin, y++, x, "   mQQQQQQ#             )TY?Y?Y`                       :QQQQQQ(            ");
    mvwaddstr(HelpWin, y++, x, "   mQQQQQQ#             _______       ._aaaaas,,.    wawQQQQQQwaac         ");
    mvwaddstr(HelpWin, y++, x, "   mQQQQQQ#   Eric      jQQQQQQ;    <yQQQWWWWWQQQw,  QWWQQQQQQQQQk         ");
    mvwaddstr(HelpWin, y++, x, "   mQQQQQQ#             jQQQQQQ;   jWQQQQW\?\?YQWQWWQ, ^~)QQQQQQr~^`         ");
    mvwaddstr(HelpWin, y++, x, "   mQQQQQQ# Gillespie   jQQQQQQ;   mWQQQQQws,==----    :QQQQQQ(            ");
    mvwaddstr(HelpWin, y++, x, "   mQQQQQQ#             jQQQQQQ;   ]QQQQQQWWQQQmwa,    :QQQQQQ(            ");
    mvwaddstr(HelpWin, y++, x, "   mQQQQQQ#             jQQQQQQ;    -?YUWWQQQQQQQQQw   :QQQQQQ(            ");
    mvwaddstr(HelpWin, y++, x, "   mQQQQQQ#             jQQQQQQ;  __i___s ~\"?QWQQQQQ   :QQQQQQ[   ._,      ");
    mvwaddstr(HelpWin, y++, x, "   mQQQQQQQaaaawaaaaw,  jQQQQQQ;  -QQQQQQga_amQQQQQD   :QQQQQQmawmQQQg     ");
    mvwaddstr(HelpWin, y++, x, "   mQQQQQQWQQQQWWQQQW(  jQQQQQQ;   \"9QQQQQWQQQQQQ@!`   -$QQQQQQWWQQQQ\?     ");
    mvwaddstr(HelpWin, y++, x, "   \?\?\?\?\?\?\?\?\?\?\?\?\?\?\?\?\?\?'  )!!!!!!`     -~\"\?\?T\?T\?!\"`        ~\"!\?\?\?\?\?\?\?\"`     ");
                                                                   
  }
/*
  else  {
    mvwaddstr(HelpWin, y++, x, "                 ..   ..                ..                                  ");         / * Ln 0 * /
    mvwaddstr(HelpWin, y++, x, "                 ::                     ::                                  ");        / * Ln 1 * /
    mvwaddstr(HelpWin, y++, x, "                 %%   ;;    ;;;;;;   ;;;;;;;;         By                    "); / * Ln 2 * /
    mvwaddstr(HelpWin, y++, x, "                 %%   %%   ;      '     %%       Eric Gillespie             "); / * Ln 3 * /
    mvwaddstr(HelpWin, y++, x, "                 &&   %%    %%%%%%      %%                                  "); / * Ln 4 * /
    mvwaddstr(HelpWin, y++, x, "                 &&   &&   ,      :     &&                                  "); / * Ln 5 * /
    mvwaddstr(HelpWin, y++, x, "                 ##   ##    ######      ##                                  "); / * Ln 6 * /
    mvwaddstr(HelpWin, y++, x, "                 ##                                                         "); / * Ln 7 * /
    mvwaddstr(HelpWin, y, x,   "                 ############################                               "); / * Ln 8 * /
/ * Gawd, this needs some work! * /
  } End of commented out code */
  /*   standend(); */
  if(!fi->Monochrome)  {
    wattroff( HelpWin,  COLOR_PAIR(COLOR_YELLOW)); /* reset attribs to cyan on blue */
    if(EightBitDisplay) {
        wattron( HelpWin, A_ALTCHARSET | COLOR_PAIR(COLOR_CYAN));
      }
    else  {
      wattron( HelpWin, COLOR_PAIR(COLOR_CYAN));
    }
  }
  else  {
    wattroff( HelpWin, A_BOLD);
  }
  if(debug) {
    y = 13; x = 17;
    sprintf(tempstring, "Debug version %1d.%1d.%1d %s %s build", LISTVERSION, LISTMAJOR, LISTMINOR, __DATE__, __TIME__); /* Ln 10 */
    mvwaddstr(HelpWin, x, y, tempstring);
  }
  x = 5; y = 13;
/* We have a problem: what about screens that are NOT 80x25?  */
/* Here's one solution...keyed off LINES (a constant of ncurses) */
  mvwaddstr(HelpWin, y++, x, "Keys to use within list: (Q to exit help, any other key to continue)"); /* Ln 12 */
  x = 1;   /* reset x to left hand side of the screen plus one to balance */
  if(EightBitDisplay) {
    mvwaddstr(HelpWin, y++, x, "ÚÄÄÄÄÄÂÄÄÄÄÄÄÂÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÂÄÄÄÄÄÄÄÂÄÄÄÄÄÄÂÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿"); /* Ln 13 */
    mvwaddstr(HelpWin, y++, x, "³ Key ³ Mode ³         Action         ³  Key  ³ Mode ³       Action          ³"); /* Ln 14 */
    mvwaddstr(HelpWin, y++, x, "ÃÄÄÄÄÄÂÄÄÄÄÄÄÂÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÂÄÄÄÄÄÄÄÂÄÄÄÄÄÄÂÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´"); /* Ln 15 */
    mvwaddstr(HelpWin, y, x,   "³  B  ³ Text ³ Move back one page     ³   B   ³ Hex  ³ Move back");
    sprintf(tempstring, "%-5d", -(fi->Scrn_y-1-(ruler)) * fi->ScrnWide);
    x += 64;
    mvwaddstr(HelpWin, y, x, tempstring);
    x += 6;
    mvwaddstr(HelpWin, y++, x,                    " bytes ³");  /* Ln 16 */
    x = 1; /* Reset back to the left of the screen plus one for balancing */
/*    mvwaddstr(HelpWin, x++, y, "³ Up (j) ³  16 bytes   ³    One line backward       ³(E)nd ³    End of file    ³"); / * Ln 17 */
    mvwaddstr(HelpWin, y, x,   "³   Up   ³");
    x += 10;
    sprintf(tempstring, "  %-4d", fi->ScrnWide);
    mvwaddstr(HelpWin, y, x, tempstring);
    x = 17; /* 63 chars from the right hand side */
    mvwaddstr(HelpWin, y++, x, " bytes ³    One line backward       ³(E)nd ³   End of file   ³"); /* Ln 17 */
    x = 1;
    mvwaddstr(HelpWin, y, x, "³  Down  ³");
    x += 10;
    sprintf(tempstring, "  %-4d", fi->ScrnWide);
    mvwaddstr(HelpWin, y, x, tempstring);
    x = 17; /* 63 chars from the right hand side */
    mvwaddstr(HelpWin, y++, x, " bytes ³    One line forward        ³(G)o  ³ Goto byte/line  ³"); /* Ln 18 */
    x = 1;
    mvwaddstr(HelpWin, y, x,   "³ Space  ³ +");
    sprintf(tempstring, "%-5d", (fi->Scrn_y - 1 - (ruler)) * fi->ScrnWide);
    waddstr(HelpWin, tempstring);
    x += 17;
    mvwaddstr(HelpWin, y, x,                    "bytes ³ 1 page (");
    x += 35;
    sprintf(tempstring, "%-4d", (int)fi->Scrn_y-1-ruler);
    waddstr(HelpWin, tempstring);
    waddstr(HelpWin,                                               " lines) forward");
    mvwaddstr(HelpWin, y++, x,                                                     "³  %   ³ Goto % of file  ³"); /* Ln 19 */
    x = 1;  /* Reset back to the left of the screen plus one for balancing */
    mvwaddstr(HelpWin, y, x,   "ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ"); /* Ln 25 */
  } /* now, that deals with a non-X screen... what about a screen under X? */
  else  { /* Here goes ... */
    mvwaddstr(HelpWin, y++, x, ".________._____________.____________________________.______._________________.");
    mvwaddstr(HelpWin, y++, x, "| Key    |  Hex mode   |       Text mode            | Keys |  Location       |"); /* Ln 14 */
    mvwaddstr(HelpWin, y++, x, "+--------+-------------+----------------------------+------+-----------------+"); /* Ln 15 */
    mvwaddstr(HelpWin, y, x,   "|(B)ackUp|");
    sprintf(tempstring, "%-5d", -(fi->Scrn_y-1-(ruler)) * fi->ScrnWide);
    x += 11;
    mvwaddstr(HelpWin, y, x, tempstring);
    x += 5;
    mvwaddstr(HelpWin, y, x,                " bytes | 1 page (");
    x += 17;
    sprintf(tempstring, "%-4d", (int)fi->Scrn_y-1-ruler);
    mvwaddstr(HelpWin, y, x, tempstring);
    x += 3;
                                     mvwaddstr(HelpWin, y, x, "lines) backward");
    x += 16;
    mvwaddstr(HelpWin, y++, x,                                                     "|(H)ome|  Start  of file |");
    x = 1; /* Reset back to the left of the screen plus one for balancing */
/*    mvwaddstr(HelpWin, x++, y, "| Up (j) |  16 bytes   |    One line backward       |(E)nd |    End of file    |"); */
/*    mvwaddstr(HelpWin, x++, y, "|Down(k) |  16 bytes   |    One line forward        |(G)o  |  Goto byte/line   |"); */
    mvwaddstr(HelpWin, y, x,   "|   Up   |");
    x += 10;
    sprintf(tempstring, "%-4d", fi->ScrnWide);
    mvwaddstr(HelpWin, y, x, tempstring);
    x = 17; /* 63 chars from the right hand side */
    mvwaddstr(HelpWin, y++, x, " bytes |     One line backward      |(E)nd |   End of file   |"); /* Ln 17 */
    x = 1;
    mvwaddstr(HelpWin, y, x, "|  Down  |");
    x += 10;
    sprintf(tempstring, "  %-4d", fi->ScrnWide);
    mvwaddstr(HelpWin, y, x, tempstring);
    x = 17; /* 63 chars from the right hand side */
    mvwaddstr(HelpWin, y++, x, " bytes |     One line forward       |(G)o  | Goto byte/line  |"); /* Ln 18 */
    x = 1;
    mvwaddstr(HelpWin, y, x,   "| Space  | +");
    x += 12;
    sprintf(tempstring, "%-5d", (fi->Scrn_y-1-(ruler)) * fi->ScrnWide);
    mvwaddstr(HelpWin, y, x, tempstring);
    x += 5;
    mvwaddstr(HelpWin, y, x,                   "bytes | 1 page (");
    x += 16;
    sprintf(tempstring, "%-4d", (int)fi->Scrn_y-1-ruler);
    mvwaddstr(HelpWin, y, x, tempstring);
    x += 3;
    mvwaddstr(HelpWin, y, x,                                       " lines) forward");
    x += 16;
    mvwaddstr(HelpWin, y++, x,                                                     "|  %   | Goto % of file  |");
    x = 1;  /* Reset back to the left of the screen */
    mvwaddstr(HelpWin, y, x,   "`============================================================================'");
  }
  /* We preferably don't want the help screen itself larger than 24 lines */
  /*  move(24,39);
  addstr("Q to quit help screen"); */
  wrefresh(HelpWin);
  /*  if(FPtr!=stdin) */  /* Hmmm, will the next line work in STDIN mode? */
   if( QUIT_CHAR != ( c = getch())) { /* use this char to decide whether we display the next help screen... */
/*    wclear(HelpWin); */
    if(!fi->Monochrome) {
      if(EightBitDisplay) {
        wattron(HelpWin, A_ALTCHARSET | COLOR_PAIR(COLOR_CYAN) | A_BOLD);
      }
      else {
        wattron(HelpWin, COLOR_PAIR(COLOR_CYAN) | A_BOLD);
      }
    } else {
      if(EightBitDisplay) {
        wattron(HelpWin, A_ALTCHARSET | A_BOLD);
      }
      else {
        wattron(HelpWin,  A_BOLD);
      }
    }  
       
    y = 0, x = 1;
    if(EightBitDisplay) {
      mvwaddstr(HelpWin, y++, x, "ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿"); /* Ln 1 */
      mvwaddstr(HelpWin, y++, x, "³                               Other keys                                   ³"); /* Ln 2 */
      mvwaddstr(HelpWin, y++, x, "ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´"); /* Ln 3 */
      mvwaddstr(HelpWin, y++, x, "³   Z/z: toggle debug flag     H/h: this help screen   S: statistics on file ³"); /* Ln 4 */
      mvwaddstr(HelpWin, y++, x, "³   O/o: toggles offset address in hex mode between decimal and hexadecimal  ³"); /* Ln 5 */
      mvwaddstr(HelpWin, y++, x, "³   7: toggle printable chars    8: toggle all chars    *: show line ends    ³"); /* Ln 6 */
      mvwaddstr(HelpWin, y++, x, "³   l: toggle line numbers (only in text mode)                               ³"); /* Added: Ln 7 */
      mvwaddstr(HelpWin, y++, x, "³   t/x: toggles text/hex mode  Q/q: Quit (Hasta La Vista!)                  ³"); /* Ln 8 */
      mvwaddstr(HelpWin, y++, x, "³   /  to search forward                     \\ to search backwards           ³"); /* Ln 9 */
      mvwaddstr(HelpWin, y++, x, "³   n  repeats a previous search in the same direction                       ³"); /* Ln 10 */
      mvwaddstr(HelpWin, y++, x, "³           Other stuff will be put in here soon, as I make up the code      ³"); /* Ln 11 */
      mvwaddstr(HelpWin, y++, x, "³                                                                            ³"); /* Ln 12 */
      mvwaddstr(HelpWin, y++, x, "³   e  fires up an editor on the file     w  write bug report                ³"); /* Ln 13 */
      for(y = 13; y < 22; y++) {
        mvwaddstr(HelpWin, y, x, "³                                                                            ³");
      }  /* Gives us the next few lines the lazy way... */
      mvwaddstr(HelpWin, y++, x, "³                     Hit any key to continue                                ³"); /* Ln 24 */
      y = 23;
      mvwaddstr(HelpWin, y, x,   "ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ"); /* Ln 25 */
      wrefresh(HelpWin);
      c = getch();
    }
    else  {
      mvwaddstr(HelpWin, y++, x, ".----------------------------------------------------------------------------."); /* Ln 1 */
      mvwaddstr(HelpWin, y++, x, "|                            Other keys                                      |"); /* Ln 2 */
      mvwaddstr(HelpWin, y++, x, ":----------------------------------------------------------------------------:"); /* Ln 3 */
      mvwaddstr(HelpWin, y++, x, "|   Z/z: toggle debug flag    H/h: this help screen   S: statistics on file  |"); /* Ln 4 */
      mvwaddstr(HelpWin, y++, x, "|   O/o: toggles address in hex mode between decimal and hexadecimal         |"); /* Ln 5 */
      mvwaddstr(HelpWin, y++, x, "|   7: toggle printable chars    8: toggle all chars  *: show line ends      |"); /* Ln 6 */
      mvwaddstr(HelpWin, y++, x, "|   l: toggle line numbers (only in text mode)                               |"); /* Added: Ln 7 */
      mvwaddstr(HelpWin, y++, x, "|   t/x: toggles text/hex mode  Q/q: Quit (Hasta La Vista!)                  |"); /* Ln 8 */
      mvwaddstr(HelpWin, y++, x, "|   /  to search forward   \\ to search backwards      n  repeats search      |"); /* Ln 9  */
      mvwaddstr(HelpWin, y++, x, "| e  fires up an editor on the file   w  write bug report to /tmp/list.debug |"); /* Ln 10 */
      mvwaddstr(HelpWin, y++, x, ":----------------------------------------------------------------------------:"); /* Ln 11 */
      mvwaddstr(HelpWin, y++, x, "|           Other stuff will be put in here soon, as I make up the code      |"); /* Ln 12 */
      mvwaddstr(HelpWin, y++, x, "|                                                                            |"); /* Ln 13 */
      for(y = 12; y < 20; y++) {
        mvwaddstr(HelpWin, y, x, "|                                                                            |"); /* Ln 14-21 */
      }  /* Gives us the next few lines the lazy way... */
      mvwaddstr(HelpWin, y++, x, ":----------------------------------------------------------------------------:"); /* Ln 22 */
      mvwaddstr(HelpWin, y++, x, "|                     Hit any key to continue                                |"); /* Ln 23 */
      mvwaddstr(HelpWin, y, x,   "`----------------------------------------------------------------------------'"); /* Ln 24 */
      wrefresh(HelpWin);
      c = getch();
    }
  } /* ... otherwise, finish up with the ShowHelp() routine... */
/*  clear();
  refresh(); */
   delwin(HelpWin);  /* That takes care of removing the window - no steenkin' memory leaks here */
}


int ShowStats(struct FileData *fi) {
  char *termtype, *tempstring;
  WINDOW *StatWin;
  int c, y=0, x=0, EightBitDisplay;
  if(NULL == (tempstring = (char *) malloc(sizeof(tempstring) * 1024))) {
    CloseNCurses();
    Bye(1, __LINE__);
  }
/*  clear(); */
  if(fi->Scrn_x < 56 || fi->Scrn_y < 18 ) {
    debug_function("Cannot display stats - screen too small", 0, fi->Scrn_y, __LINE__);
    refresh();
    return 0;
  }
  StatWin = newwin(18, 56, 5, 5);
  termtype=(char *) malloc(sizeof(termtype) * (strlen(getenv("TERM"))+1));
  /* What did I define this  previous line for? */
#ifdef __linux__  /* For some reason, this compiles under BSD and Linux, but LINUX isn't defined here...
 * anywayI should be using the value of xterm instead of LINUX here...
 */
  if(getenv("DISPLAY")==NULL)
    EightBitDisplay=1;
  else
#endif
    EightBitDisplay=0;
  if(EightBitDisplay) {
    if(!fi->Monochrome)  { /* If in colour mode... */
      wbkgd(StatWin, ' ' | A_ALTCHARSET | COLOR_PAIR(COLOR_BLUE));
    }  else  { /* Nope, mono */
      wbkgd(StatWin, ' ' | A_ALTCHARSET | A_BOLD);
    }
    if(!fi->Monochrome)  {
      wattron(StatWin, A_ALTCHARSET | COLOR_PAIR(COLOR_CYAN));
    }
  }
  else {
    if(!fi->Monochrome)  {
      wattron(StatWin, COLOR_PAIR(COLOR_CYAN));
    }
  }
/*  wmove(StatWin, y++, x); */
  if(EightBitDisplay)  {
    mvwaddstr(StatWin, y++, x, "ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿"); /* Ln 0 */
/*     wmove(StatWin, y++, x); */
    mvwaddstr(StatWin, y, x,   "³                   ");
    x += 20;
    if(!fi->Monochrome)  {
      wattron(StatWin, COLOR_PAIR(COLOR_YELLOW) | A_BOLD);
    } else  {
      wattron(StatWin, A_BOLD);
    }
    mvwaddstr(StatWin, y, x,                       "File Statistics");
    x += 15;
    if(!fi->Monochrome)  {
      wattroff(StatWin, COLOR_PAIR(COLOR_YELLOW) | A_BOLD);
      wattron(StatWin,  COLOR_PAIR(COLOR_CYAN));
    } else  {
      wattroff(StatWin, A_BOLD);
    }
    mvwaddstr(StatWin, y++, x,                                    "                    ³"); /* Ln 1 */
    x = 0;
    mvwaddstr(StatWin, y++, x, "ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´"); /* Ln 2 */
    mvwaddstr(StatWin, y, x,   "³  FPosn:    ");
    x += 13;
    sprintf(tempstring,                       "%6ld /* Current file position */        ³", fi->FPosn); /* Ln 3 */
    mvwaddstr(StatWin, y++, x, tempstring);
    x = 0;
    sprintf(tempstring, "³  FEnd:     %6ld /* Total bytes */                  ³", fi->FEnd); /* Ln 4 */
    mvwaddstr(StatWin, y++, x, tempstring);
    sprintf(tempstring, "³  FLines:   %6ld /* Num CR-terminated lines */      ³", fi->FLines); /* Ln 5 */
    mvwaddstr(StatWin, y++, x, tempstring);
    sprintf(tempstring, "³  FLineCtr: %6ld /* Current line within file */     ³", fi->FLineCtr); /* Ln 6 */
    mvwaddstr(StatWin, y++, x, tempstring);
    sprintf(tempstring, "³  Start:    %6ld   Count:    %6ld                 ³", fi->Start, fi->Count); /* Ln 7 */
    mvwaddstr(StatWin, y++, x, tempstring);
    sprintf(tempstring, "³  Scrn_y: %3d  Scrn_x: %3d ScrnWide: %3d              ³", fi->Scrn_y, fi->Scrn_x, fi->ScrnWide); /* Ln 8 */
    mvwaddstr(StatWin, y++, x, tempstring);
    sprintf(tempstring, "³  *FName: %s ",   /* there's a bit of trickery here - let the preprocessor do the work - the next lines ARE correct */
#ifndef _MINIX
    basename
#endif
    (fi->FName)); /* Should we squeeze in the dirname? */
    mvwaddstr(StatWin, y, x, tempstring);
    x += 27;
    mvwaddstr(StatWin, y++, x, " /* The file's basename */  ³"); /* Ln 9 */
    x = 0;
    sprintf(tempstring, "³  DumpMode: %s /* Either hex or text */             ³", (fi->DumpMode=='t') ? "text" : "hex "); /* Ln 10 */
    mvwaddstr(StatWin, y++, x, tempstring);
    sprintf(tempstring, "³  DumpFlag: %c /* Either '7' or '8' or '*' */          ³", fi->DumpFlag); /* Ln 11 */
    mvwaddstr(StatWin, y++, x, tempstring);
    sprintf(tempstring, "³  addrflag: %7s  /* Hex or decimal addresses */   ³", ((fi->addrflag) ? "Decimal" : "Hex    "));   /* Ln 12 */
    mvwaddstr(StatWin, y++, x, tempstring);
    sprintf(tempstring, "³  EofSet: %s   /* At End of File? */                 ³", ((fi->EofSet)? "yes" : "no "));  /* Ln 13 */
    mvwaddstr(StatWin, y++, x, tempstring);
    sprintf(tempstring, "³  SPosn: %ld", fi->SPosn);
    mvwaddstr(StatWin, y, x, tempstring);
    x += 16;
    mvwaddstr(StatWin, y++, x,                 " /* String Position */                 ³"); /* Ln 14 */
    x = 0;
    mvwaddstr(StatWin, y++, x, "ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´"); /* Ln 15 */
/*    sprintf(tempstring,  "³                "); */
    mvwaddstr(StatWin, y, x,   "³                ");  /* Ln 16 */
    x += 17;
    wattron(StatWin, A_BOLD);
    mvwaddstr(StatWin, y, x,                    "Hit any key to continue"); /* Ln 16 */
    x += 23;
    wattroff(StatWin, A_BOLD);
    mvwaddstr(StatWin, y++, x,                                         "               ³");  /* Ln 16 */
    x = 0;
    mvwaddstr(StatWin, y++, x, "ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ"); /* Ln 17 */
  }
  else  {
    mvwaddstr(StatWin, y++, x, ".______________________________________________________.");
    mvwaddstr(StatWin, y, x,"|                   ");
    if(!fi->Monochrome)  {
      wattron(StatWin, COLOR_PAIR(COLOR_YELLOW) | A_BOLD);
    } else {
      wattron(StatWin, A_BOLD);
    }
    x += 20;
    mvwaddstr(StatWin, y, x, "File Statistics");
    if(!fi->Monochrome)  {
    wattroff(StatWin, COLOR_PAIR(COLOR_YELLOW) | A_BOLD);
    wattron(StatWin,  COLOR_PAIR(COLOR_CYAN));
    } else {
      wattroff(StatWin, A_BOLD);
    }
    x += 15;
    mvwaddstr(StatWin, y++, x, "                    |");
    x = 0;
    mvwaddstr(StatWin, y++, x, "|------------------------------------------------------+");
    mvwaddstr(StatWin, y, x, "|  FPosn:    ");
    sprintf(tempstring, "%6ld /* Current file position */        |", fi->FPosn);
    x += 13;
    mvwaddstr(StatWin, y++, x, tempstring);
    x = 0;
    sprintf(tempstring, "|  FEnd:     %6ld /* Total bytes */                  |", fi->FEnd);
    mvwaddstr(StatWin, y++, x, tempstring);
    sprintf(tempstring, "|  FLines:   %6ld /* Num CR-terminated lines */      |", fi->FLines);
    mvwaddstr(StatWin, y++, x, tempstring);
    sprintf(tempstring, "|  FLineCtr: %6ld /* Current line within file */     |", fi->FLineCtr);
    mvwaddstr(StatWin, y++, x, tempstring);
    sprintf(tempstring, "|  Start:    %6ld   Count:    %6ld                 |", fi->Start, fi->Count);
    mvwaddstr(StatWin, y++, x, tempstring);
    sprintf(tempstring, "|  Scrn_y: %3d  Scrn_x: %3d ScrnWide: %3d              |", fi->Scrn_y, fi->Scrn_x, fi->ScrnWide);
    mvwaddstr(StatWin, y++, x, tempstring);
    sprintf(tempstring, "|  *FName: %s ", /* Yes, these next lines ARE correct - let the preprocessor do the work */
#ifndef _MINIX
    basename
#endif
        (fi->FName));
    mvwaddstr(StatWin, y, x, tempstring);
    x += 31;
    sprintf(tempstring, "  /* The file's name */ |");
    mvwaddstr(StatWin, y++, x, tempstring);
    x = 0;
    sprintf(tempstring, "|  DumpMode: %s /* Either 'hex' or 'ascii' */       |", (fi->DumpMode=='t') ? "ascii" : "hex ");
    mvwaddstr(StatWin, y++, x, tempstring);
    sprintf(tempstring, "|  DumpFlag: %c /* Either '7' or '8' or '*' */          |", fi->DumpFlag);
    mvwaddstr(StatWin, y++, x, tempstring);
    sprintf(tempstring, "|  addrflag: %s  /* Hex or decimal addresses */    |", (fi->addrflag) ? "Decimal" : "Hex   ");
    mvwaddstr(StatWin, y++, x, tempstring);
    sprintf(tempstring, "|  EofSet: %s   /* At End of File? */                 |", (fi->EofSet) ? "yes" : "no ");
    mvwaddstr(StatWin, y++, x, tempstring);
    mvwaddstr(StatWin, y++, x, "|======================================================|");
    mvwaddstr(StatWin, y, x, "|                ");
    x += 17;
    wmove(StatWin, y, x);
    wattron(StatWin, A_BOLD);
    mvwaddstr(StatWin, y, x, "Hit any key to continue");
    x += 23;
    wattroff(StatWin, A_BOLD);
    mvwaddstr(StatWin, y++, x, "               |");
    x = 0;
    mvwaddstr(StatWin, y++, x,"`------------------------------------------------------'");
  }
  wrefresh(StatWin);
   if( QUIT_CHAR != ( c = getch())) { /* use this char to decide whether we display the next stats screen... */
   
  }
  /* Here's the next screen...shows argc and argv... */
  if(!fi->Monochrome)  {
    wattroff(StatWin, A_ALTCHARSET | COLOR_PAIR(COLOR_YELLOW));
  }
  delwin(StatWin);
  return 0;
}


int Dump(struct FileData *fi) { /* 1 */
  int a;
  long i, j, k, l;
  char *tempstring;
  unsigned char buf[1024];
  /* For the hexdump processing buffer - have a decent amount of room
   * now we aren't dealing strictly with 16 byte blocks any more */
  char promptbuf[256]; /* Buffer for prompt of bytes/lines - strictly this should be enlarged too for stupidity */
  char *promptbufp;
  int linecount=0; /* Obvious name for a counter */
  char *prtln; /* output line, bumped up from 82, changed from signed */
  tempstring = (char *) malloc(sizeof tempstring * 1024);
  if(tempstring == NULL) {
	  CloseNCurses();
	  Bye(1, __LINE__);
  }
  /* Define nice big fat amount, since we nearly hit the 256 limit with a
   * 160 char wide screen (imagine what would happen on a 21" screen @ 1600x1200) */
  promptbufp=&promptbuf[0];

  /*   fi->DumpFlag = '8'; /X* Hey, we've got a questionable line here... */
  memset(buf, 0,  (sizeof(char) * 1024)); /* Clear both buffers before using... */
  if(NULL ==
     (prtln =
      (char *) calloc(((sizeof prtln) * 1024), sizeof(char))
      )
     )  /* If this calloc fails (NULL return) */
     {
	CloseNCurses();
    Bye(1, __LINE__);  /* Then bomb with "No memory available" message */
     }
  fi->FPosn=fi->Start;  /* Set pos'n to requested Start */
/*  if(not exist DISPLAY) do next line, else continue  */
#ifdef __linux__  /* I shouldn't have to use this... */
  if((getenv("DISPLAY"))==NULL)
     attron(A_ALTCHARSET);
#endif
  clear();
  refresh();
  /* I should call ScanForCR() from here instead of from within
   * NewFile() */
  sprintf(tempstring, "list: Please wait while I go find all the lines...\n");
  move(8,0); /* Move to a place the user can see us... */
  addstr(tempstring);
  refresh();  /* ... and now, display it */
  if(!fi->Quick) { /* If the Quick switch is set, say for large 120 meg files ... then forget these */
    ScanForCr(fi);
    if(!(fi->CrArray=calloc(fi->FLines, sizeof(u_long)))) {
	    CloseNCurses();
	    Bye(1, __LINE__);
    }
    fi->CrArray=AllocateLines(fi); /* This allocates the line ends */
  }
  for(;;) {  /* (2) i.e forever... or at least until I kill it ... */
    if(debug) 
      debug_function("Seeking.. ", fi->Scrn_x - 11, fi->Scrn_y - 1, __LINE__);
    fseek(fi->FPtr, fi->FPosn, SEEK_SET);
/* Ummm - not sure if I need this... after all, if I redefine Scrn_y|x then I don't WANT getmaxyx */
/* *******************************************************************************/
/*    y = fi->Scrn_y; x = fi->Scrn_x;  / * First off, set the vars.. */
/*    getmaxyx(stdscr, fi->Scrn_y, fi->Scrn_x); */
/* Then check whether they are the same or less than asked for values */
/*    if(y < fi->Scrn_y)
          fi->Scrn_y = y;
    if(x < fi->Scrn_x)
          fi->Scrn_x = x; */
    /* ... otherwise, continue with computed values... the ones we defined are the same or too large */
    /* This line tell us the real screensize - this was dup'ed earlier in main()
     * but this will handle screen resizes while running
     */
    /* I want to add a user-defined screensize here, add a couple of entries into the struct */
    fi->ScrnWide = (fi->Scrn_x - 12) / 4;  /* This seems to be a little tidier than if else statements, but is int rounded... */
    linecount = (ruler) ? 1 : 0; /* This is the line number to which lines get printed */
    /*      standend(); /x* just to reset the screen */
/*    clear(); */  /* See if taking this out speeds up any ... */
    move(linecount, 0);
/*    refresh(); */
    if(fi->DumpMode == 'x') { /* 3s */
      for(;;) {  /* 4s   Yes, another loop forever...this *should* loop through each page, but is only looping through a line */
   /* What I really want, is to loop through each line on this page, and once we run out of bytes in the file,
     * print blank lines until the end of the page
     */
   /* ... this rewrite came about as a result of dissatisfaction with the previous code */
        j = fread(buf,  /* read off fi->ScrnWide bytes */
                sizeof(char),
                ((fi->FPosn) <= (fi->FEnd - fi->ScrnWide)) ? fi->ScrnWide : (fi->FEnd - fi->FPosn),
    /* this is where we read screen-dependent widths - i.e. non-80 char screens */
                fi->FPtr);
        if(j == 0) {   /* This deals with the case of no more to read from file... */
          for(a = linecount; a < fi->Scrn_y - 1; a++) {
       /* Something's funny with the linecount var - it's Scrn_y whatever I do... */
            move(a,0);
            clrtoeol();
          }
          move(fi->Scrn_y-1,0);
	  /* Now, how do I set up the EndOfFile flag? Ah, there it is...see if this works... */
          fi->EofSet++;
          PrintLine(fi);
          break;
	}
        memset(prtln, ' ', sizeof(char) * 1024);
     /* first, print the offset in whatever form... */
        if(!(fi->addrflag)) 
          sprintf(prtln, "%06lX:   ", (unsigned long)fi->FPosn);
        else 
          sprintf(prtln, "%06ld:   ", fi->FPosn);
     /* ... now print the hex bytes... */
        for (i = 0; i < j; i++) {  /* 5s - this prints off the hex bytes */
          sprintf(prtln+((i*3+10)), "%02X ", buf[i]); /* 5e  - Will this fix it? */
        }
        if(j < (fi->ScrnWide)) {
           int count;
           fi->EofSet++;
           for(count=j; count < fi->ScrnWide; count++)
             sprintf(prtln+((count*3+10)), "   ");  /* This clears off the rest of the chars */
/*           for(a = (++linecount); a < fi->Scrn_y - 1; a++) {
       / * Something's funny with the linecount var - it's Scrn_y whatever I do... * /
            move(a,0);
            clrtoeol();
            refresh(); / * ECG: added 5-3-2001 * /
          } */
        }
        if (debug) {
          char tempst[80];
          sprintf(tempst, "Line:%d\ti:%ld\tj:%ld", linecount, i, j);
          debug_function(tempst, 0, fi->Scrn_y - 1, __LINE__);
        }
        sprintf(prtln+(10+(fi->ScrnWide*3)), " "); /* this adds a gap */
     /* now do it again for the chars... */
        for(i = 0; i < j; i++)
          switch(buf[i]) {
             case 0:
             case 1:
             case 2:
             case 3:
             case 4:
             case 5:
             case 6:
             case 7:
             case 8:
             case 9:       /* Tab case... */
             case 11:
             case 12:
             case 13:
             case 14:
             case 15:
             case 16:
             case 17:
             case 18:
             case 19:
             case 20:
             case 21:
             case 22:
             case 23:
             case 24:
             case 25:
             case 26:
             case 27:
             case 28:
             case 29:
             case 30:
             case 31:
             case 127:
                  sprintf(prtln + i + 11 + (fi->ScrnWide*3), "%c", '.');
                  break;
             case 10:
               if(fi->DumpFlag == '*')
                 sprintf(prtln + i + 11 + (fi->ScrnWide*3), "%c", '.');
               else
                 sprintf(prtln + i + 11 + (fi->ScrnWide*3), "%c", ' ');
                 break;
             default:
                 if(fi->DumpFlag == '7') /* but does it interfere with this? */
                   sprintf(prtln + 11 + i + (fi->ScrnWide * 3), "%c", (isprint(buf[i])) ? buf[i] : '.');
                 else
                   sprintf(prtln + 11 + i + (fi->ScrnWide * 3), "%c", ((buf[i]=='\n') ? ' ' : buf[i]));
                    /* returns . if 7-bit, else _should_ return itself */
                 break;
          }
	if(j < fi->ScrnWide) {
          for(i = j; i < fi->ScrnWide; i++)
            sprintf(prtln + 11 + i + (fi->ScrnWide * 3), "%c", ' ');
/*	  PrintLine(fi); ECG: Added 6-3-2001 ->*/
/*          refresh(); */
          fi->EofSet++;
/*            sprintf( prtln+(9 + i + (fi->ScrnWide*3)), "%c",  ((buf[i]=='\n') ? '.' : buf[i]));  / * again, this is dependent on screensize... */
  /* Code needs revamping from here onwards to cope with the serious code rewrite in the past few lines *x/                                                                    /x* | */
          /* These two cases blank out the non valid hex characters on the line *      |
           * that is, the characters that are read after the end of the file */     /* | */
/* We need this next line to tell us the screen size */
        }
        fi->FPosn+=j; /* is this needed now? */
        sprintf(prtln + 11 + (fi->ScrnWide * 4), "%c", '\n'); /* Well, adding a \n seems a bit pointless *
    * considering it's ncurses we're dealing with... */
        addstr(prtln);  /* What were these two lines for??? tempstring replaced by prtln to print out... */
/*  sprintf(tempstring, "%s\n", prtln);
  addstr(tempstring); */
/*        refresh(); * remove this refresh and enable one below...
 *         it seems to make things move faster
 */
        linecount++;
        memset(buf, 0, sizeof(buf));
        /* cleans out the buffer for the next  bytes */
        if((linecount%(fi->Scrn_y-1))==0) { /* is the page full yet? */
          move(fi->Scrn_y-1,0); /**/
          PrintLine(fi);
          if (fi->FPosn >= (fi->FEnd-1)) {
/* Ah!  This is where I should blank the remaining lines...? */
            fi->FPosn=fi->FEnd-1;
            move(fi->Scrn_y-1,68);
/*              move(fi->Screen_y-1, 68); */
            attron(A_BOLD);
            addstr("End of file");  /* This needs to turn up whenever we get to end of file, not just at end of screen */
            attroff(A_BOLD);
            refresh();
          } /* end of FPosn:FEnd check */
          refresh();  /* added this here to reduce the time of refresh  */
          break; /* now which one does *this* break out of??? */
	} /* End of full page check */
      move(linecount, 0);  /* this was added to see if it cures the problem of 0410: 0410: etc ... */
      }
    } /* Matches beginning of loop for hexdump */

 /* ***********************    text version   ****************************** */

     else if(fi->DumpMode=='t') {
       /* What I want to do here is to count CRs or complete Scrn_y-wide lines in a screen *
       * kind of like:
       for(i=fi->FPosn; i < (fi->FPosn + ((fi->Scrn_x) * (ruler) ?  (fi->Scrn_y-2) : (fi->Scrn_y-1 ))); i++)  {
         if(!fread(buf, sizeof(char), fi->Scrn_x, fi->FPtr)) Bye(1);
          */
/*      clear();
      refresh(); */
      k = 0;
      fi->FPosn = (fi->FPosn > fi->FEnd) ? fi->FEnd : (fi->FPosn < 0) ? 0 : fi->FPosn;
      /* gives me either legal values within file, or file beginning or end. */
      for(fi->FLineCtr = 0; fi->FLineCtr < fi->FLines; fi->FLineCtr++ ) {
        /* sets initial fi->FLineCtr for line p'sn retrieval */
        if(*(fi->CrArray+fi->FLineCtr) >= fi->FPosn)
          break;
      }  /* Now I've got the right line, step through the chars ... */
      for(;;)
        {
/*    char TabVal;  * Stick this back in once we work out how to do Tabs elegantly - yeah, right... */
          memset(prtln,0,(sizeof(prtln) * 1024)); /* clear out the build string ... */
          memset(tempstring, 0, (sizeof(tempstring) * 1024 )); /* ... and clear out the printing string... */

          /* beginning of loop 4eva - don't know what other limit to use */
          if(debug) debug_function("Seeking...", fi->Scrn_x - 11, fi->Scrn_y - 1, __LINE__);
/*          {
            int oldx=linecount;
            move(LINES-1,69);
            addstr("Seeking...");
            move(oldx,0);
            /x*     c=getchar();
                   while(c != EOF) c=getchar(); *x/
            refresh();
          } */
          fseek(fi->FPtr, fi->FPosn, SEEK_SET); /* Seeks to where the file position is set */
          if(debug) 
            debug_function("Getting...", fi->Scrn_x - 11, fi->Scrn_y - 1, __LINE__);
    /* it seems fgets expects an int as second param, and I'm using longint here - getline()?
     * Might need to change this from an fgets to getch (until tab or CR) - then display line.
      * Of course, all this means a revamp of the AllocateLines array to also include Tab characters
     * ... I could always dump this entirely, and use fread a block, then spit chunks of that block
     * to the screen as I need them, formatting as I go...the only thing I would need to remember
     * would be when I came to block boundaries.  I would just load in another block then.
     * ******* BUT ******* ... I haven't got there yet.
     */
     /* if(fread(BigBuffer, Size (char), Amount, fi->FPtr))
           DoTheStuff();
              else {   what?
    }
      */
          l = (*(fi->CrArray + (fi->FLineCtr + 1 )) - *(fi->CrArray + fi->FLineCtr));
    /* This is the number of bytes to read */
          if(l == fread(  /* If the fread operation is equal to the number of bytes I want to read ...*/
                   prtln, /* the place to put the string ... */
             sizeof(char), /* size of each item to read */
       l, /* this being the third arg */
                   fi->FPtr  /* ... and where it comes from... */
             ))
    /*            for(int k=0; k < (*(fi->CrArray+(fi->FLineCtr+1))-*(fi->CrArray+fi->FLineCtr))+1; k++)  */
            {  /* ... then do... */
            fi->FLineCtr++;  /* I should stuff the initial TopOfScreen value somewhere to use later... */
      if(debug) {
        char*TmpString = (char *) malloc(sizeof(TmpString) * 1024);
        sprintf(TmpString, "Got %ld bytes - step into the for loop", l);
         debug_function(TmpString, fi->Scrn_x - 40, fi->Scrn_y - 1, __LINE__);
        free(TmpString);
        }
            move(linecount,0);  /* Set to the beginning of the screenline */
/*            refresh(); */
/************************** Added program code 5 Oct 2000 - ECG *******************************/
            if(fi->LineNumbers) { /* This could maybe be toggled hex/decimal ... possibly ... */
              sprintf(tempstring, "%08ld: ", fi->FLineCtr);
            }
/*********************** End of added program code 5 Oct 2000 - ECG *******************************/
            for( i = 0;
/*                 i <= (*(fi->CrArray+fi->FLineCtr - 1) - *(fi->CrArray+(fi->FLineCtr))); */
                   i < l;
                 i++ ) { /* [3] loop through each char */
/*              if(prtln[i] == 0x0a || prtln[i] == 0x0d)
                if(fi->DumpFlag!='*')
                  prtln[i]=' ';
/ * This stops line-ends showing * /
                else prtln[i]='.';
              if( prtln[i] <= 0x1F )
                prtln[i]='.';
              if(  prtln[i] == 0x7F )
                prtln[i]='.'; */
/* The previous lines have been replaced with this following cleaner switch statement
   The only thing remains is to see which is faster, the switch or the if statement?
 */
              if(k > fi->Scrn_x) {   /* seeing as tabs are now taken into account, we now have to recheck linelength */
    fi->FPosn += i;
    break; /* ... out of the for loop, and read off the rest of the line to the next screen line */
              }
              switch(prtln[i]) {
                case 0:
                case 1:
                case 2:
                case 3:
                case 4:
                case 5:
                case 6:
                case 7:
                case 8:
    case 9:  /* This case will disappear soon, to be replaced by the commented out case 9 below */
                case 11:
                case 12:
                case 13:
                case 14:
                case 15:
                case 16:
                case 17:
                case 18:
                case 19:
                case 20:
                case 21:
                case 22:
                case 23:
                case 24:
                case 25:
                case 26:
                case 27:
                case 28:
                case 29:
                case 30:
                case 31:
                case 127:
                  prtln[i] = '.';
      memcpy(tempstring + (k++) + ((fi->LineNumbers) ? 10 : 0), prtln + i, 1);
                     break;
                case 10:
         if(fi->DumpFlag == '*') {
           prtln[i] = '.';
           memcpy(tempstring + (k++) + ((fi->LineNumbers) ? 10 : 0), prtln + i, 1);
         }
                     else {
           prtln[i] = ' ';
           memcpy(tempstring + (k++) + ((fi->LineNumbers) ? 10 : 0), prtln + i, 1);
                     }
                     break;
/*              case 9: */ /* Hell, this is a K&R Chapter 1 exercise!!! */
         /* We badly need a strcat here to add into the middle of a string...
          * This would take the part of read string up to [i], copy that somewhere...
          * then expand tab another n spaces, then copy rest of string onto end of string.
          */
/*         for (TabVal = 0; TabVal < fi->TabSize; TabVal++)
            memcpy(tempstring + k + TabVal, " ", 1);
                     k += TabVal;
         break; / * Now, will this work? I have string overflow problems here... */
                default:
                  if(fi->DumpFlag == '7') /* but does it interfere with this? */
        /* memcpy(tempstring + j, prtln[i], 1); */
                    prtln[i] = (isprint(prtln[i])) ? prtln[i] : '.';
        memcpy(tempstring + (k++) + ((fi->LineNumbers) ? 10 : 0), prtln + i, 1);
                    /* returns . if 7-bit, else _should_ return itself */
                  break;
                } /* End of switch statement */
/*              if(debug) {
                sprintf(tempstring, "%ld      ", i);
                debug_function(tempstring, fi->Scrn_x - 11, linecount, __LINE__);
              } */

            } /* end of for loop [3] through each char */
            if(debug) {
        char *TmpString = (char *) malloc(sizeof(TmpString) * 160);
              sprintf(TmpString, "Printing.. %lu %u", (u_long)strlen(prtln), linecount);
              debug_function(TmpString, fi->Scrn_x - 20, fi->Scrn_y - 1, __LINE__);
        free(TmpString);
            }

/*            sprintf(tempstring, "%s", prtln); */
            clrtoeol();
            addstr(tempstring); /* This is where the string is printed to screen */
      k = 0;
/*            refresh(); / * This refresh must be needed? */
      if(debug)
        debug_function("Next one...", fi->Scrn_x - 11, fi->Scrn_y - 1, __LINE__);
            fi->LnAtTopOfScrn = fi->FLineCtr - linecount; /* at least, I _think_ this is right... naah, not the right value, so what now?...*/
      fi->LineCount = linecount;
            linecount++;
            fi->FPosn=*(fi->CrArray+fi->FLineCtr);
          } /* Matches with if(fread) */
          else {  /* No, fread didn't read all the bytes... */
      /* Another bug ready to be squashed *splat* this one's eating the last line of file */
      clrtoeol();
      if(debug) debug_function("End of file hit?", fi->Scrn_x - 16, fi->Scrn_y - 1, linecount);
            linecount=fi->Scrn_y-1; /* End of file or Error */
            fi->EofSet++;
          } /* End of fread function() */

/* Now, since the main work is done, now follows a few tests */

          if(fi->FPosn>=(fi->FEnd-1)) {
          /* If fileposition is >= end of file,  (should this be at this point, or one screen back? )*/
      for(a = linecount; a < fi->Scrn_y - 1; a++) { /* ... clear off the rest of the lines  */
         move(a, 0);
         clrtoeol();
         }
            fi->LnAtTopOfScrn = fi->FLineCtr - linecount; /* at least, I _think_ this is right... naah, not the right value, so what now?...*/
      fi->LineCount = linecount;
            fi->FPosn=(*(fi->CrArray+fi->FLineCtr-1));
            linecount=fi->Scrn_y-1;
          }
          /* Original spot block taken from */
          if (linecount>=fi->Scrn_y-1) {
      for(a = (linecount + 1); a <= fi->Scrn_y - 1; a++) { /* Stick this here, what does it do?  */
         move(a, 0);
         clrtoeol();
         }
            fi->LnAtTopOfScrn = fi->FLineCtr - linecount; /* at least, I _think_ this is right... naah, not the right value, so what now?...*/
      fi->LineCount = linecount;
            move(fi->Scrn_y-1,0);
            PrintLine(fi);
            refresh();
            break;
          }
          if(fi->EofSet) {
            move(fi->Scrn_y-1,68);
            attron(A_BOLD);
            addstr("End of file");
            attroff(A_BOLD);
            fi->EofSet--;
          }
          refresh();  /* is this where I should place it? */
        } /* End of loop 4eva */
    }  /* End of fi->DumpMode=='t' test */
    if((fi->DumpMode=WhatNext(fi))==0) /* Goes and asks what to do next */
      break;
  } /* end of loop forever through file loop */
  return(0);
} /* End of Dump() */

void PrintLine(struct FileData *fi)  {
  int i; /* , RulerIsSet */ 
  char *tempstring;
  if(NULL == (tempstring = (char *) malloc(sizeof(tempstring) * 1024))) {
    CloseNCurses();
    Bye(1, __LINE__); /* Whoops - ran out of memory - we're out of here!*/
  }
  attron(A_REVERSE); /* Replace attron(A_BOLD) */
/* ******************* beginning of ruler print code ******************* */
/* *** Could we make this more efficient? *** */
  if(ruler) {
    int thisIval;
    move(0,0);
    clrtoeol();
    if(fi->DumpMode=='t') {
    /* First, we deal with the case of (if linenumbers are shown) */
      if(fi->LineNumbers) {
        sprintf(tempstring, "Line No:    ");
      }
      /* Now the rest of the ruler line... */
      for(i = ((fi->LineNumbers) ? 11 : 1); i <= fi->Scrn_x -
      ((fi->LineNumbers) ? 0 : 10)  /* Remove this when we find out how to do lines properly in textmode */
      ; i++)  {
        switch (fi->LineNumbers) {
           case 1:
              thisIval = i - 10;
              break;
           default:
              thisIval = i;
              break;
        }
   if(fi->addrflag)  {
           switch (thisIval % 10)  {
             case 0:
      if(thisIval < 100)
                    sprintf(tempstring + i - 1, "%d", (thisIval /10));
      else
                    sprintf(tempstring + i - 2, "%2d", (thisIval / 10)); /* This cures the larger than 100 chars wide case */
         break;
             case 1:
             case 2:
             case 3:
             case 4:
             case 6:
             case 7:
             case 8:
             case 9:
               sprintf(tempstring + i - 1, "%c", '.');
         break;
       case 5:
               sprintf(tempstring + i - 1, "%c", '*');
         break;
           } /* end of switch */
         }  /* fi->addrflag (decimal) */
         else  /* !fi->addrflag (hex) */
           switch (thisIval % 16)  {
             case 0:
               if(thisIval < 256)
                 sprintf(tempstring + i - 1, "%X", thisIval /16);
               else
/* This cures the larger than 255 chars wide case - though who has a line this long? */
                 sprintf(tempstring + i - 2, "%2X", thisIval /16);
         break;
             case 1:
             case 2:
             case 3:
             case 4:
             case 5:
             case 6:
             case 7:
             case 9:
             case 10:
             case 11:
             case 12:
             case 13:
             case 14:
             case 15:
               sprintf(tempstring + i - 1, "%c", '.');
         break;
       case 8:
               sprintf(tempstring + i - 1, "%c", '*');
         break;
           }
      }
      addstr(tempstring);
      memset(tempstring, 0, sizeof(char) * 1024);  /* Clear out the string... */
    }
    else
      if(fi->DumpMode=='x') {
  sprintf(tempstring, "%s", "Offset    ");
  for(i = 1; i <= fi->ScrnWide; i++) {
     if(fi->addrflag)  /* This switch displays either hex or decimal address */
       sprintf(tempstring + 10 + ((i-1) * 3), "%02d ", i);
     else
             sprintf(tempstring + 10 + ((i - 1) * 3), "%02X ", i);
        }
        sprintf(tempstring + 10 + (fi->ScrnWide * 3), "%c", ' ');
  for(i = 1; i <= fi->ScrnWide; i++)  {
          if(fi->addrflag)  /* Same here as above test */
      sprintf(tempstring + 10 + (fi->ScrnWide * 3) + i , "%01d ",  (i % 10)); /* 'nother bug squashed */
          else
            sprintf(tempstring + 10 + (fi->ScrnWide * 3) + i , "%01X ",  (i % 16));
        }
  addstr(tempstring);
        memset(tempstring, 0, sizeof(char) * 1024);
  refresh();
/* To think that all the above code replaces the single line below 8-) */
/*        addstr("Offset  00 01 02 03 04 05 06 07   08 09 0A 0B 0C 0D 0E 0F   0123456789ABCDEF"); */
  }
    else if(fi->DumpMode == 'd')  {
  sprintf(tempstring, "%s", "Offset  ");
  for(i = 0; i < fi->ScrnWide; i++) {
     if(fi->addrflag)  /* This switch displays either hex or decimal address */
       sprintf(tempstring + 10 + (i * 4), "%03d ", i);
     else
             sprintf(tempstring + 10 + (i * 4), "%03X ", i);
        }
        sprintf(tempstring + 10 + (fi->ScrnWide * 4), "%c", ' ');
  for(i = 0; i < fi->ScrnWide; i++)  {
          if(fi->addrflag)  /* Same here as above test */
      sprintf(tempstring + 11 + (fi->ScrnWide * 4) + i , "%01d ",  i % 10);
          else
            sprintf(tempstring + 11 + (fi->ScrnWide * 4) + i , "%01X ",  i % 16);
        }
  addstr(tempstring);
        memset(tempstring, 0, sizeof(char) * 1024);
  refresh();
    }
    move(fi->Scrn_y-1,0); /* Move to the bottom of the screen, ready for the footer */
  }
/* ********************** End of ruler test ***************************** */
/* Now the turn of the footer */
  if(debug) {
    attroff(A_REVERSE);
    sprintf(tempstring, "Vsn: %1d.%1d.%1d ", LISTVERSION, LISTMAJOR, LISTMINOR);
    debug_function(tempstring, 0, fi->Scrn_y - 1, __LINE__ );
    attron(A_REVERSE);
  }
  if(fi->FPtr!=stdin) {   /* remove the stdin stuff - whoops, this doesn't need to be
  printed if we're in text mode...*/
    sprintf(tempstring, "%s   byte: %ld %4ld%%  ", fi->FName, /* First the name, */
            (fi->FPosn <= fi->FEnd) ? fi->FPosn : fi->FEnd, /* ... then the pos or end, */
            (((fi->FPosn <= fi->FEnd) ? fi->FPosn : fi->FEnd)*100/fi->FEnd)); /* and the same in percent */
  }
  else
    sprintf(tempstring,"File: STDIN  Fpos: %ld  ", /* in short, byte pos'n */
            (fi->FPosn <= fi->FEnd) ? fi->FPosn : fi->FEnd);
  /* Line here added - though how do we know if it's at BottomOfScrn? */
  clrtoeol();
  addstr(tempstring);
  memset(tempstring, 0, sizeof(char) * 1024);
  refresh();
  if(fi->DumpMode=='t') {
/* There's a bug here... it doesn't come up with the correct value for the top of the screen line */
    sprintf(tempstring,"line: %ld - %ld LnCt: %ld", fi->LnAtTopOfScrn, fi->FLineCtr, fi->LineCount);
    /* Bug**squashed!, well not yet */
    addstr(tempstring);
    memset(tempstring, 0, sizeof(char) * 1024);
    refresh();
  }
  /* else if(fi->DumpMode=='x') ; / * empty - as no line is printed in hex mode, (???)
                                      so this line isn't even needed  */
  addstr("  h=help");
  free(tempstring);
  if(fi->EofSet) {
    move(fi->Scrn_y-1,68);
    attron(A_BOLD);
    addstr("End of file");  /* This needs to turn up whenever we get to end of file, not just at end of screen */
    attroff(A_BOLD);
    fi->EofSet--;
  }
  refresh();
/*  attroff(A_BOLD); */
  attroff(A_REVERSE);
} /* End of PrintLine */

#ifndef _MINIX
__inline__ void BackPage(struct FileData *fi, long int LinesToBackUp)  { /* 1st in */
#else
void BackPage(struct FileData *fi, long int LinesToBackUp)  { /* 1st in */
#endif
  if(fi->DumpMode=='t') { /* 2nd in */
    fi->FLineCtr-= (ruler) ?
      (fi->FLineCtr >=(LinesToBackUp-1)) ?
      (LinesToBackUp-1) :
      fi->FLineCtr :
      (fi->FLineCtr >=LinesToBackUp) ?
      LinesToBackUp :
      fi->FLineCtr;
    fi->FPosn=(*(fi->CrArray+fi->FLineCtr));
  } /* 2nd out */
  else if(fi->DumpMode=='x') {
    fi->FPosn-= (ruler) ?   /* first check is for ruler on or off */
      ( fi->FPosn >= (( LinesToBackUp - 1) * fi->ScrnWide) ) ?  /* if so, is file position less than one page away from end? */
      ( ( LinesToBackUp - 1 ) * fi->ScrnWide ) : /* if so, subtract from fi->FPosn */
      (fi->FPosn) : /* ... else, just return itself - end of return _with_ ruler set */
      ( fi->FPosn >= ( LinesToBackUp * fi->ScrnWide ) ) ? /* otherwise, no ruler, so test for fi->FPosn less than one page away from end */
      ( LinesToBackUp * fi->ScrnWide ) :  /* if so, subtract this many bytes away from fi->FPosn */
      fi->FPosn;  /* else nothing */
  }
  fseek(fi->FPtr, fi->FPosn, SEEK_SET);
}


void debug_function(char *debugstr, int Screen_x, int Screen_y, int SourceLine)  {
  int c, y, x;
  getyx(stdscr, y, x); /* Get and stash where I am... */
  move(Screen_y, 0); /* Move to bottom of screen... */
  clrtoeol();
  move(Screen_y, Screen_x);
  attron(A_BOLD);
  addstr(debugstr);
  refresh();
  move(y, x); /* Now stick me back where I was... */
  attroff(A_BOLD);

  if( QUIT_CHAR == (c=getch())) {
	  CloseNCurses();
	  Bye(2, SourceLine);
  }
        refresh();
}

/* New stuff for menus * /
int MakeMenu(struct ListFile *fi)  {
	MENU *TopMenu;
	char MenuString[] = {"File","Edit","Search","Help" };
	char FileString[] = { "Open", "Quit"};
	char EditString[] = { "Edit file", "Write notes"};
	char SearchString[] = { "Search Forward", "Search Backwards", "Repeat search" };
	char HelpString[] = { "Keys", "File Info", "About" };
	ITEM *TopMenuItems[SIZEOF(MenuString)];
	ITEM **TopMenuPtr = TopMenuItems;
	/ * Beginning of Pseudocode until I work out what I DO need 
	 * TopMenu = MakeMenu(MenuString, size_x, size_y); * This function should draw the main menu on screen 
	* /	
/ * Make sure I finish this function - some way! * /
	return 0;
}
 end of Menu code */

