/* vim:ts=2:ww=0: 
 * */
/*  code starts at line 293, int main() starts at 348
**  HEXDUMP.C - Dump a file.
**  Changed to list.c - to emulate (hopefully) Vernon Buerg's famous program
**  of the same name
**
**  Initial copyright
**    This Program Written By Paul Edwards w/ modifications by Bob Stout
**
**  Srchfile.c added (regards to Bob Stout - thanks...) with modifications
**  Contributors:
**     Myself of course (Eric Gillespie <list@flying-brick.caverock.net.nz>
**     Some debugging and other points pointed out by Dion Bonner <bonner@xtra.co.nz>
**  Version   - stored in hexdump.h
**  0.2 - 0.5 Further mods by Eric Gillespie 16 Jun 97 and Aug - Nov 1997
**  0.6 - 0.6.2 Working on realloc...working...working  - Jan 1998
**  0.6.3  Finally squashed the realloc bug!
**         Also removed CountCr as it was unnecessary, seeing as I had
**         ScanForCr() already
**  0.6.4  Have gone to a struct to pass between functions - major revamp
**         of everything to match up with struct pointer passing
**  Feb 1998
**  0.6.5  Tidied up the beginning and end of file processing and installed a
**         zero-length check.
**  0.6.6  Completely revamped the dump routines...
**  0.6.7  ... and slightly revamped the struct, adding an entry for
**         the current line being used.  Also changed the buf array
**         from char to unsigned char, and changed the format for sprintf
**  0.6.8  Dealt with the Ruler, and with the bottom status line
**         Also shifted globals into new hexdump include file
**         *Finally* sorted boundary problems
**         Adding switch to select new file - isn't working yet
**         File is selected, but memory gets written over every time
**         I fseek the file.  I will also need to add a Comp() - see
**         dequeue.c:Q_Sort() for more details
**  March 1998
**         If I don't specify -f on commandline, program gets rather lost and
**         doesn't display anything, and exits when I hit any key.
**  0.6.9  Making a maintenance release with updated feature of 7/8 bit
**         printing of characters.  Have fixed ruler problem on text mode.
**         Fixed stdin reading (sort of...)
**  0.6.10 Have fixed keyboard problem when piped from stdin...
**         I just don't ask for a getch()
**  0.6.11 Have added a line-end toggle, though it shows all chars as well...
**         Have noticed that program is slow at allocating line-ends on huge
**         files (larger than say, 300k) - would this be sped up by loading
**         file into huge buffer and counting CRs there?
**  ********************************************************
**        Imported to Linux - gee, this will be an adventure
**  0.7.0  Will have to change all the screen-based routines
**         to use curses.  Have also had to undergo a name change
**         because hexdump already exists on this platform 8-)
**         hey, hexdump actually works here...!! Wonder if list will too...
**  Jan 1999
**  0.7.1  Have split off the debug switches into a separate function
**         have also (slightly) sped up the hex display of screen
**         by not updating each line, just doing a screen at a time.
**  June 1999
**  0.7.2  Have added the % command (jump to percentage of file)
**         and modified the help screen to reflect the correct number of
**         bytes and lines (but need to modify to remove dependency on LINES
**         and COLS (which don't change))
** November 1999
**  0.7.3  Have (finally) got a scandir function working - shall attempt to
**         amalgamate it in to filebrowse.c
**         Am also integrating dequeue back into the source if it can be used
**         effectively (i.e. if I can figure out how to use it without SIGSEVs)
**         I have just been made aware of a weakness in the ncurses implementation:
**         the window doesn't automatically re-size (though I think there is a function
**         that does just this - I just have to find out what it is called.)
**         Also, I have a problem with the ALT_GR charset in an xterm - it can't
**         be seen!!  I will obviously have to work out how to "have my IBM charset"
**         and view it too.  Hmmm... that's a point - I could deliberately make
**         a program to repeatedly show LINES and COLS, and see whether it changes
**         when I resize the xterm window.  There is an environment function I can
**         use to test for DISPLAY - if that exists, I can switch over to using
**         just plain characters (char mapping is done by the font manager anyway).
**
**  0.7.4  Worked out (sorta) the new filename prompting... except the strcpy routine is
**         puking at the copying of the string into the fi->FName var.  Haven't
**         worked out why yet...
**
** 0.7.5   Well, I sorted that out by malloc'ing PATH_MAX bytes to fi->FName each time
**         and now it doesn't fall over.  Also added a -v switch, and realised another
**         reason why I wanted this filebrowser - I didn't want to see any more
**         <FF><C0>-style things turning up.  Mind you, I haven't sorted out full
**         8-bit printing in text mode - that's my next project. Then I want to handle
**         input from stdin, like less does.
**         One fault I have just found is that the debug function doesn't
**         work as well as I'd like.  Am working on this one now.
**
** 0.7.6   Decided to add a DisplayStats function, to show number of lines, bytes etc
**         Also added colour for the first time! Converted Help() and DisplayStats()
**         to open in a window rather than erase stdscr.
** 0.7.7   Have finally cracked the 8-bit display in text mode!  It was wrapped up
**         in the test of prtln[i] against valid values, so I changed it to a
**         large switch/case statement.  It seems that I still haven't
**         worked out the line length vs. COLS problem.
** 0.7.8   Well, it seems like I've cracked the COLS problem - the next implementation
**         will be bringing in some sort of search function...yeah, likely - not!
**
** March 2000
** 0.7.9   I've finally found a version of lclint that works!  It has picked up so many
**         nits and bits that I'm surprised about.  I went back to signed long ints, but 
**         it looks like I might have to go back to unsigned for all the screen trouble
**         it's giving me.  I will also have to rewrite the print-to-string routine for
**         hex mode.
**         So far I've gone from signed char buf[17] to unsigned chars, which has cured
**         the screen droppings.  So I'll retain the signed longints to keep lint happy.
**
** 0.7.10  I think I've come up with a simpler way of creating my output line in hex mode...
**         The concept is to build a line using 4-byte chunks to the width of the screen.
**         and then filling lines.  I found out what was causing the Help() fault in an xterm
**         - not enough lines in my xterm!  I needed the size of xterm to be 26 lines or
**         better, so I've reduced the help screen down to 24 lines, and changed the
**         midpoint.  I have also started changing the window references in DisplayStats()
**         Of course that still doesn't help the ALT_CHARSET problem under X - I don't
**         have an answer for this.  So far I get around it by checking whether DISPLAY exists
**         and if so, I use alternative chars to outline the windows with.
** 0.7.11  Worked out the width problem for window, and side effect is that screen resizing
**         seems to be handled fine - I thought I was going to have to handle that myself!
**     I still haven't separated output into 4-byte blocks yet, just a stream of hex
**         bytes followed by the chars. Now I just need to add code for the ruler.
**         Only thing is that text mode still works at the screen size the program started
**         with...guess I'll have to go add the code for that into dump(text) as well as 
**         dump(hex)
** July 2000 (approximately)
** 0.7.12  YES!!!!! The arrows problem is FINALLY SOLVED!!!!!!!!!!! I was using the wrong
**         function to read my keys in WhatNext() - I should have been using the ncurses
**         routine getch() _instead of_ the stdio routine getchar().
**         Next object is to go sort out the code that reads the keys.
**         Whoops - realised that keys behave differently for each termtype, i.e. for Linux,
**         console, the keys work properly, but for some other termtypes, (especially under X)
**         they don't work the same way.  Should I try to understand how ncurses picks up its
**         keystrokes to feed to the upperlevel process?  Is there some way of modifying low-level
**         keyparsing routines?
**
**         I think I have just come up with a filename length problem (256 bytes...) when I feed
**         (G)et with a name longer than 254 chars...?  Also, how would I do the equivalent of
**         basename? This would help out with DisplayStats().
**
**         Oooooo - what happens when list tries to open a Directory???????  Just saw this from
**         looking at code for less.  Am hoping to integrate some useful code from less to modularise
**         the functions somewhat.  Also want to set up a queue of files on the commandline to list.
**         Have also tidied up how the ruler looks in decimal or hex, in text and hexdump modes.
**
** 0.7.13  Cleaned up some code, added extra cases to Bye(), added a search routine (both forwards
**         and backwards, however, the search-to-position isn't working well, and neither is the
**         display of the screen directly subsequent to a search hit.  Heh he - I just thought of
**         a way around the lookataDir() problem - give a listing of the directory on the screen!
**         Now isn't that smart?  Although seriously, this should come from filebrowse if object
**         turns out to be a dir.
**
**         Have just figured out that the ffsearch/rfsearch are "best-fit" algorithms - though the
**         best-fit doesn't...for example if I search for ffsearch from beginning of list.c, I'll
**         get a hit on ffset instead...
**         20 minutes later...
**         Figured out what was wrong - I was setting size in ffsearch to sizeof(fi->SearchString)
**         which, of course, will give you the size of the pointer (which this is...) - what I
**         wanted was the length of the string SearchString pointed _to_, therefore, *(fi->SearchString)
**         Now what we need, is  to position (in text mode anyway) to the nearest line containing search
**         pattern.  And, it's an exact pattern match. Hex mode doesn't matter - it will handle exact
**         placement, however, it looks a little funny to see
**                 ffsearch(blah);
**         instead of seeing
**                 if((-1L) ==(fi->SPosn = ffsearch(blah);
**         I'm still seeing screen droppings...
**
**         I've had an idea, but it would mean changing the key that 7/8 or * toggles were done on,
**         it would be to use a numerical prefix to do the following command n times...
** August 2000
** 0.7.14  I'm sure this is worth another version update - I seem to have fully implemented the functions
**         Search forward, Search Backward, and Repeat Search.  I even think I have cleared off the screen
**         droppings.  The only thing is, I haven't fully bullet-proofed the StringPrompt routine yet
**         (for empty strings etc...)  Have also changed the key used for help from ? to h, and the key
**         for Repeat from ^L to N.  Have also changed what gets passed to Help from a File * to a struct pointer
** 0.7.15  Tidied some more code up, due to Dion's bugfinding, and finally got debug_function to display
**         yellow text on red background.  Also added a taBsize parameter, and sorted out most of the
**         screendroppings left behind as a result - meaning I get to change the version number yet again.
**     <B> The only thing left is that the rest of the line left behind (if wider than COLS) doesn't get
**         shifted down onto the next line of display.  Hmmm.... Have decided to take out tab-processing for now.
**         Have found another buglet I have seen for a while.  When starting in text mode, the hexmode vars
**         aren't initialised.  Will cure this forthwith.  I may also have to #include another file so I
**         don't get an implicit declaration of basename()
**         Ported (well, just copied, really...) this to BSD - compiled without a hitch except for having to use
**         a slightly different library name curses instead of ncurses.  It's all the same thing anyway in the
**         version of OpenBSD I have (2.7) - hope Dion has luck with copying.  I also had to put in a horrible
**         #ifdef LINUX because BSD console treats ALT_CHARSET wierd, so it's better if I leave it as white on
**         black for the moment.  I'm even surprised the Makefile works under BSD - they seem to use a different
**         but similar syntax, however BSD swallowed my Makefile without comment. Actually, I've found that the
**         variable LINUX isn't defined under Linux...? Huh?
**         Fixed that - found that __linux__ would probably be #defined somewhere in the Linux system...I really
**         need to remove the system dependency just for displaying 8-bit and DisplayStat() and Help()
** 0.7.16  Started work on a BugWrite function to take input from the user and either write to a specified file
**         or mail to a specified user.  Creates a file in the tmp dir to work with...
**         Also added stat data into the struct.  Will also need to remove all references to COLS and LINES, and
**         replace with the two vars I defined in the struct FileData.
**         I've also cleaned up screen updating in Help() and DisplayStats() by removing wclear() out of each.
**     <B> It seems the screen isn't updating properly any more...so have put a wclear back in for the moment
**         until I sort out how to clear off each line without too much penalty in speed
** 0.7.17  Added an edit function, calls system() to run external program, go see EditFunction()
**     <B> So far, bug is that LnAtTopOfScrn isn't being set properly at _all!_
**     <B> I've altered the debug_function code, but it is no longer printing the line it's supposed
**         to print. Tried doing a neat frame around the window, but it's not having any of it...
**         Also added a timestamp option to each entry I write to the debug file.
**         Am striking problems with the last line of file not getting written to screen in text mode.
** 0.7.18  Got fixed...  Also fixed offset problem when starting editor at specified line - luckily most
**         Linux editors support this.  Also put in a return to same line feature once returned from edit.
**         Only hassle is trying to edit a filename that begins with + - the editor thinks it's a command.
**         Now, if we coud only fix this LineCount problem...and it still won't compile under FreeBSD
**         The following vars aren't found: __errno_location, __xstat, __cbyte_h (??) and __strtol_internal
**         It's possible that these are compiler generated.
**         I also haven't sorted out what happens when no file is specified on the commandline.
**         (Later) Sorted that.  Now my only problem is working out how to simplify the colour stuff
**         so that list will compile under minix!  Yes!  If I can do this, it might give me ideas as
**         to how to strip/simplify mainstream code.  Minix has color, but I don't know how to access it yet.
**         <Later> Sorted out most of the minix problems, now I just have to strip start_color and init_pair
**         and have minix work without colours.
**     <B> Fixed a subtle fault I didn't find until now - if I start a search "/", and then hit enter, the
**         program bombed - am now checking the length of string returned by SearchPrompt() - also found
**         independently by Dion the BugHunter.
** October 2000
** 0.7.19  (End of September)  Made a large number of changes to code to allow the use of list under
**     monochrome conditions such as minix offers.  Have altered HelpScreen to HelpWin to be consistent 
**         with StatWin in ShowStat(), and changed the name of the Help() routine to ShowHelp() to be more
**         consistent.  There may be a bit more "tidying up" of namespace over the next few releases.
** 0.7.20  I have added line numbers to the text view - it was easier than I thought... but lines are not
**         being filled to the right hand side when line numbers aren't being shown - so really, the sooner
**         I get the lineparsing routine fixed from choosing fixed length (screenwide) long, the better.
**     <B> I've just found that the ruler line in text mode doesn't shift across when in line number mode.
** 0.7.21  So I fixed it!  Have also changed the name of DisplayStats() to ShowStats (consistency...) and
**         added a CmdMode blank function for vim-like functions, such as (N)ext/(P)rev file...and also
**         copied argc and argv to local copies inside the fi structure for use later by CmdMode.
**         Well, have finally found out that __OpenBSD__ _is_ #defined in said OS, and so would __FreeBSD__
**         be in that OS.  Dion mentioned that when he attempts to compile under FreeBSD, stdin is not #defined
**         at the link stage, so I have to work out why, though I suspect that now I've sorted out that.
**     <B> Found another bug relating to display in text mode - on an 80 char screen, 71 chars (not 70) get 
**     <B> displayed. Also, colours aren't changing under OpenBSD - I can't figure out why...
** December 7th, 2000
**         Not a lot has happened since last code addition - still considering code complexity; for example:
**         Screen lines vs. File lines - I guess I display all screenlines I can, then increment filelines with
**         as many as fitted on the screen?  Then how do I treat lines that are longer than one screen in length?
**     <N> Changed a XRunning variable into EightBitDisplay, and changed the sense of test.
** February 12, 2001
** 0.7.22  Finally found one reason why things are taking so long to start up.  I was doing ScanForCR() and 
**         AllocateLines() twice. Found it when I added a 'q'uick switch to get around it - hey, I'm supposed 
**         to be rewriting that whole section anyway!  As a result, I have ended up with a MAJOR increase in 
**         startup speed for hexmode when selecting quick mode.
**         Have also added code to handle displaying in a different screensize than is current - though I have
**         to watch what I get here...
** March 2, 2001
** 0.7.23  Just added code in for choosing size of screen for dump...
**     <B> Found a buglet - on a huge file, display of offset in decimal gets overwritten by file data - corrected
**     <B>'nother buglet - go to the end of the file, it's not the end - short by Scrn_wide bytes.
**         Could put more info in the bug file...like dump stats once per session, and dump fileposn every time
**         I "w"rote to the bugfile.
**     <B> Displaying the last line of a file in hex that takes up less than a screen, loses it's last half a hex byte, 
**         and doesn't display it's chars, except when I display the end of the file.  This could be the bit of the 
**         file that's responsible for munging the last line.
**         We seem to have sorted that one out...
**         Went on a bit of a tikitour the past few days trying to clear off excess lines...back to original 0.7.23 status
**     <B> Another bug found by Dion - Searching backwards on empty string crashes the program.
**
** 0.7.24  Hmm, we seem to have missed a version here somewhere
** 0.7.25  Okay, some stuff suggested by bronaugh's comments of 8-Oct-2001
**         Okay, what happens when the user has a 128M file of \n?  How large does CrArray get then? (512M)
**     <B> WhatNext() is leaking memory
** 0.7.26  Almost fixed blank screen for Usage(), but haven't got "Wrong parameter - not known" done yet
** 0.7.27<B>I still have to fix the flick into curses and back out for Usage().
**         I have split off Usage() and Help() into a separate file, added BugWriteFName to fi, 
**         and given the user the chance to select the filename he wishes to use.  
**         I haven't YET set up a default of homedir/list.debug
**     <I> I could load the previous contents of the BugWrite file into the edit window.
**     <B> We STILL don't have the lines display correct in PrintLine if we have less lines in file than a screenfull.
**     <?> Could we utilise a unwrap-like function? This displays the leftmost hunk of line, and we scroll right or left
**         for any remainder.
** 0.7.28  Moved an awful lot of code out of list.c into other files - file.c, input.c and screen.c
**         Also added a ccmalloc target, to use with ccmalloc.  So far, all I can find is that
**         an awful lot of strcpy calls get made. Apart from that, nothing appears out of the ordinary,
**         but ncurses eats memory and doesn't appear to release it until programs end. Apparently, WhatNext()
**         also gobbles memory. Bronaugh sent me a file to utilise as a replacement to WhatNext, using an array
**         of function pointers. It also refuses to compile under DOS (not that I expected anything else).
**     <B> I don't need to switch to curses mode until my file can be loaded, however, the f param
**         calls NewFile(), which prompts user we're about to scan for lines, then calls ScanForCR()
**         So - now we need to find out how to tell the user without using NewFile(); i.e. separate the two
**         functions from each other.  Perhaps call it as initial start through Dump?  We ought to be in
**         curses by then, assuming file can be opened.
** 0.7.29  Hm!  Well, THAT worked well! Now I just work on shifting the rest of the ncurses code past init
**     <B> Uh oh - now Bye isn't ...quite... correct.  If ncurses isn't even started, then Bye barfs
**         because it closes down curses before printing anything - guess I'm going to have to stick THAT
**         in a separate function...sheesh!  - Okay, done.  Now how do I clean up when I call ecalloc?
**
**  WARNING: program in semi-stable (read - BETA) state at the moment...it's usable, but a few bugs remain.
**
**  Released under the GPL version 2, Copyright (c) 2002 by Eric Gillespie
*/
#ifndef __linux__
#ifndef __OpenBSD__  /* Then is it NetBSD? */
#ifndef __NetBSD__ /* Nope, FreeBSD? */
#ifndef __FreeBSD__  /* No, try DOS */ 
#ifndef __DJGPP__ /* Let's try solaris*/
#ifndef __sun__ /* Erm - not Solaris? Must be minix... */
#include <lib.h>  
/* Sheesh - want to #define _MINIX but I'm striking trouble,
   this file is the only one to #define _MINIX
*/
#else /* lets include the RIGHT library for dirname() */ 
#include <libgen.h>
#endif	/* __sun__ */
#endif  /* !DJGPP */
#endif  /* !BSD - this doesn't take DOS into account */
#endif /* !FreeBSD */
#endif /* !OpenBSD */
#endif /* !__linux__ */
#ifndef _MINIX  /* because MINIX hasn't got a basename() */
#include <libgen.h>  /* Just for basename() */
#endif
#include <stdlib.h>
#include <errno.h> /* For ... you guessed it! */
#include <string.h> /* ... though I thought basename was in here... */
#include <fcntl.h>
#include <curses.h>
#include <ctype.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>  /*  Do we need this here, or in filebrowse? */
/* User includes */
#include "list.h"
#include "dequeue.h"

/* User Definitions */
/* Have stuck this here, seeing as list.h is shared between here and filebrowse,
 * and it is a little inappropriate to have two declarations of the same function
 */
#ifdef _MINIX
/* Have had to define this here, 'cos there isn't such a function in minix */
void getmaxyx(WINDOW *win, int Cols, int Rows)  {
 Cols = (win)->_maxx;
 Rows = (win)->_maxy;
}
int has_colors(void){
  return 0;  /* returns 0 if no color, 1 otherwise... */
  }

void start_color(void) {
}

/* minix pukes on this next definition.. the compiler packs a SIGSEV */
void init_color(int First, int Second, int Third) { /* There! Will this cure the problem? */
}
#endif

/* Ah! This is here, 'cos I can't define it in a shared header file */
static size_t fsetup(FILE *, size_t);

int main(int argc, char **argv)
{
  char *bufp;
  char *tempstring;  /* Don't forget to malloc later on... */
  int c, y=0, x=0;
  /* I have to fix this - I HAVE to initialise this struct before referencing it... */
/*   struct stat FileInfo; / * stat from <sys/stat.h> - 2 lines - bits to delete... */
/*   struct stat *ListFileInfo; / * ... and create an associated pointer... */

  struct FileData ListFile =
  { 0, /* FPosn */
    0, /* FEnd */
    0, /* FLines */
    0, /* *CrArray */
    0, /* LnAtTopOfScrn */
    0, /* FLineCtr */
    0, /* LineNumbers - added for debugging */
    0, /* Start */
    0, /* Count */
    0, /* Scrn_y */
    0, /* Scrn_x */
    0, /* ScrnWide */
    0, /* *Fptr */
    0, /* *FName */
    't', /* DumpMode - set to text by default */
    '0', /* DumpFlag */ /* It's official - this wants to be a char */
    0, /* LineNumbers - on or off */
    0, /* Monochrome - pretty obvious, isn't it? */
    8, /* TabSize - set it to the PC default for starters */
    0, /* AddrFlag */
    0, /* EofSet */
    0, /* FgColour */
    0, /* BgColour */
    '\0', /* SearchDirection */
    0, /* SearchString */
    0, /* Search Position (SPosn) */
   { /* Start of struct stat */
     0, /* st_dev */
#ifdef __linux__
     0, /* __pad1 */
#endif
     0, /* st_ino */
     0, /* st_mode */
     0, /* st_nlink */
     0, /* st_uid */
     0, /* st_gid */
     0, /* st_rdev */
#ifdef __linux__
     0, /* __pad2  */
     0, /* st_size */
     0, /* st_blksize */
     0, /* st_blocks */
     0,  /* st_atime */
     0,  /* unused1 */
     0,  /* st_mtime */
     0,  /* unused2 */
     0,  /* st_ctime */
     0, /* unused3 */
     0, /* unused4 */
     0  /* unused5 */
#else
#if defined __OpenBSD__ || defined(__FreeBSD__)  /*__OpenBSD__ entries... */
     { 0, /* st_atimespec */
       0 },
     { 0,
       0 }, /* st_mtimespec */
     { 0,
       0 }, /* st_ctimespec */
     0, /* st_size */
     0, /* st_blocks */
     0, /* st_blksize */
     0, /* st_flags */
     0, /* st_gen */
     0, /* st_lspare - not used */
     { 0, 0 }  /* st_qspare - not used */
#else
#ifdef _MINIX
     0, /* st_size */
     0, /* st_atime */
     0,  /* st_mtime */
     0  /* st_ctime */
#endif /* _MINIX */
#endif /* __FreeBSD__ */
#endif /* __linux__ */
    }, /* struct FileInfo */
    0, /* FileInfoPtr */
    0, /* QuickDisplay */
    {0, 0, 0, 0, 0, 0}, /* add the queue DirQueue */
    0,  /* BugWriteFName */
    0,  /* MyArgc */
    0,   /* MyArgv - perhaps we should only make this valid filenames? */
    0    /* MyCurrArgC */
    };  /* ... the end of the struct init */
  /* Initialises struct to empty - yep, everything! - lclint complains*/
  struct FileData *fi; /* Define an associated pointer... doesn't this have to be alloc'ed? */
  fi=&ListFile; /* ... and point it to our just-filled ListFile struct */
  fi->FileInfoPtr = &fi->FileInfo;
  tempstring = (char *) malloc(sizeof tempstring * 1024); /* Does this limit need re-evaluating? */
  if(tempstring == NULL) Bye(1, __LINE__); /* died due to lack of memory */
  /* ... otherwise, continue as planned... */
  (void) Q_Init(&fi->DirQueuePtr); /* Initialises the queue created - lclint suggests return a void */

  /* getopt parser */
  debug=0;
  opterr = 0;

  if(argc < 2) {
    errno = ENOENT;
    Bye(12, __LINE__); /* 'Nother Dion bug squashed */
  }
  /* Now copy the argc and argv to our localspace */
  fi->MyArgc = argc;
  fi->MyArgv = argv; /* Will this work?  I don't know... */
  /* Okay - should we shove all THIS into a separate function too? */
  while ((c=getopt(fi->MyArgc, fi->MyArgv, "vf:s:r:c:n:b:eqltx78dmh?"))!= -1) {
/* v - version  f: filename s: start_byte r: rows to display  c: columns to display n: num_bytes b: taB size
 * e - show line Endings q - QuickHexmode l - show line numbers in text mode  t - text  x - hexadecimal
 * 7/8 - 7/8-bit display d - debug m - monochrome usage h/? - Usage 
 *
 */
    switch(c) {
      case 'v':    /* Spits out version information */
           printf("list: Version %1d.%1d.%1d, compiled on %s at %s, by %s\n", LISTVERSION, LISTMAJOR, LISTMINOR, __DATE__, __TIME__, "Eric Gillespie");  /* Why don't we run this through Bye()? Because there's no ERROR, that's why!!! */
           exit(0);
           break;  /* never gets to this line... 8-) this should be tidier */
      case 'f': /* Load following filename */
           /* This gets duplicated down the bottom if the -f switch wasn't used... hmmm */
           NewFile(fi, optarg);
           break;
      case 's': /* Start position */
           fi->Start=strtol(optarg, &bufp, 0);
           break;
      case 'r': /* Set number of rows (lines) different from present screen */
           y=strtol(optarg, &bufp, 0);
           if(y < fi->Scrn_y)
              fi->Scrn_y = y;
           break;
      case 'c': /*  Set number of columns different from present screen  */
           x=strtol(optarg, &bufp, 0);
           if(x < fi->Scrn_x)
               fi->Scrn_x = x;
           break;
      case 'n': /* Count off (N)um bytes */
           fi->Count=strtol(optarg, &bufp, 0);
           break;
      case 'b': /* ta(B) size */
           fi->TabSize = strtol(optarg, &bufp, 0);
           break;
      case 'e': /* Show line ends */
           fi->DumpFlag = '*';
           break;
      case 'l': /* Show line numbers in text mode */
           fi->LineNumbers = 1;
           break;
      case 't': /* Start in Text mode (default) */
           if(!fi->Quick)
             fi->DumpMode = 't';
           else  {
                   fi->DumpMode = 'x';
                   printf("You selected Quickmode - displaying hex only\n");
           }
           break;
/*    case 'A':   80x86 asm mode (Intel or AT&T?) : see how biew has done it...
      case 'a':
           fi->DumpMode='a';
           break;             end of comment */
      case 'x': /* Start in Hexdump mode */
           fi->DumpMode='x';
           break;
      case '7': /* Use 7-bit (Unixy) dump mode */
      /* This displays only characters in the range 0x20 - 0x7E, all others show as '.' */
           fi->DumpFlag = '7';
           break;
      case '8': /* Use 8-bit (DOS) mode - display all characters except 0x0 - 0x1F, 0x7F
               This mode works well in situations where 0x80 - 0xFF map exactly to IBM's
               idea of what they should be, alas in the Unix world, this is very rarely
               the case.  In fact, a lot of the time, they're not even like that in the
               DOS world if codepages are installed, or when Windows has a different charset */
           fi->DumpFlag = '8';
           break;
      case 'd': /* Turn on the debugging flag - beware - this singlesteps most main procedures */
           debug++;
           break;
      case 'm':   /* this line added to deal with debugging for minix when in Linux */
           fi->Monochrome++; /* Oooops - this value is asked for before we set it - naughty... */
           break;
      case 'q': /* This is only in while I get around the textwrap problems... */
           fi->Quick++;
           fi->DumpMode='x'; /* Force this into hexmode dump */
           break;
      case 'h': /* Well, I guess this gives a helpscreen and exits */
      case '?':
 /* Added by Dion the Bugfinder - after all, we want to see the helpscreen before it gets whisked away. */
/*           c = getch();  kinda made irrelevant by making the Usage into
 *           Printf */
           Usage();
/* After exiting from Usage(), could I go to the file selection screen?
 * Maybe not, if my options are too seriously screwed up ... */
           return (EXIT_SUCCESS); /* (Not FAILURE) After all, it did what we wanted... */
           break;
      default: /* Well, we don't know what this is... */
           refresh();
           endwin();
           printf("list: Unknown switch\n");
           Usage();
           return (EXIT_FAILURE);
    }  /* end of switch statement */
  } /* end of while loop */
  /* Process any non-flag arguments following the flags using optind
     as index to next argument.
     getopt() sets the global variable optind set to the next
     argument to process.  After the loop above it is pointing to the
     first command line argument following processed flags and their args. */
/* Perhaps I should change this to if(!fi->FPtr) { NewFile(fi, fi->MyArgv[optind]; } */
/* Old line: */
  for( ; optind < fi->MyArgc; optind++ ) {
    if(!fi->FPtr) {
/* This line below is incorrect - it should be Newfile on the first filename only...
 * and besides which, what happens when I go beyond argc?
 */
      NewFile(fi, fi->MyArgv[optind]); /* This is where all the file opening work is done */
    /* Another point - this will end up opening the *last* file on the commandline -
     * I should shove these into a linked list dequeue-style, perhaps add a new function - AddFile()?
     * Each list would have filename, previous filename, and next filename in its entry
     * Of course, I guess I could call Dump() on each...
     * Hey, this might fit in better with DirRead() and filebrowse.c ...*/
     }
  }
  if(fi->FName==0) {  /* If nothing else specified on commandline .. */

    /* fi->FPtr=stdin;   /x* We might have to scrap this, I think... */
    /* One way of dealing with the stdin would be (borrowed from an MS-DOS idea)
           * is to copy the stdin to a temporary file, or to allocate a large array */
    NewFile(fi, "stdin"); /* I should probably copy pipe data to a temp buffer - either memory or tempfile */
          fflush(fi->FPtr); /* Just to satisfy the purists... */
          /* MS-DOSism   setmode(fileno(stdin), O_BINARY);  ... try the above instead.. */
          /*    addstr("list: No filename specified on commandline\n"); */
					CloseNCurses();
    Bye(12, __LINE__); /* We dump out by telling the user we can't open file at the moment... */
  }
  /* ncurses code can FINALLY start here! */
  initscr();
  if(!fi->Monochrome) { /* 1s If we're NOT in mono ... */
    if( has_colors()) {  /* 2s - test we have color available. Hey, what happens if i want to run in monochrome? */
      start_color();
      init_pair(COLOR_BLACK, COLOR_BLACK, COLOR_BLACK);
      init_pair(COLOR_CYAN, COLOR_CYAN, COLOR_BLUE);
      init_pair(COLOR_RED, COLOR_RED, COLOR_BLACK);
      init_pair(COLOR_GREEN, COLOR_YELLOW, COLOR_RED);  /* Set this to unusual pairing */
      init_pair(COLOR_WHITE, COLOR_WHITE, COLOR_BLACK);
      init_pair(COLOR_MAGENTA, COLOR_MAGENTA, COLOR_BLACK);
      init_pair(COLOR_BLUE, COLOR_BLUE, COLOR_BLUE);
      init_pair(COLOR_YELLOW, COLOR_YELLOW, COLOR_BLUE);
    } /* 2e */
  } /* 1e */
/*    Bye(6, __LINE__);  / * This line will be replaced soon - just give me time to write the monochrome equivalents */
  noecho();
  cbreak();
  keypad(stdscr, TRUE);
  nonl();
/*  bkgd(A_ALTCHARSET); */
 /* This line tell us the real screensize -in fact I've just found a duplication with a line inside Dump() */
  getmaxyx(stdscr, fi->Scrn_y, fi->Scrn_x);
  
  if(debug) { /* We will have a future conflict here when we finally include stdin */
    sprintf(tempstring,"FName: %s          DumpMode: %c\n\rStart: %ld        Count: %ld\n\r",
            fi->FName, fi->DumpMode, fi->Start, fi->Count);
    debug_function(tempstring, 0, fi->Scrn_y, __LINE__);
  }
  /* end of getopt section - this had better work...
   * Incidentally, I've noticed OpenBSD 2.7 doesn't
	 * I don't know about any other forms of BSD
   * handle getopt parameters the same way as Linux.
  */
  /* First, a check on the fi->FName, to see if it has budged from 0 */
  if(!(fi->FPtr==stdin)) { /* if the FPtr is NOT == stdin, then ... */
    if( !( fi->FPtr = fopen( fi->FName, "rb"))) { /* ...find out if the file can be opened for reading, if not ... */
			CloseNCurses();
      Bye(7, __LINE__);
    } /* End of failure to open */
    if(fi->Scrn_x < 40) { /* Darn - this witty comment gets lost in the Bye() call */
/*      addstr("list: Um, I don't have enough columns to display the file in hex format,");
      addstr("      So am defaulting to text mode!  Hope you don't mind!");  */
			CloseNCurses();
      Bye(9,__LINE__);
    }
    if(fi->Scrn_y < 24) { /* Darn - this witty comment gets lost in the Bye() call */
/*            addstr("list: Err, there aren't enough lines here to do justice to this");
            addstr("      file - I need to get out of here! (Reason:Claustrophobia)"); */
			CloseNCurses();
      Bye(10,__LINE__);
    }
/*    stat(fi->FName, ListFileInfo); / * Second time this file is stat'ed - is this necessary? */
    /* Here are a few sanity checks... */
    if(fi->FileInfo.st_size == 0) {  /* get the filesize... */
      addstr("list: Hmmm, the file appears to be of zero length!!");
      refresh();
      endwin();
      exit(EXIT_FAILURE);
    } /* End of "File is zero bytes in size" */
    else if(fi->Count < 1) {
      addstr("list: You're trying to ask for too few bytes...!!");
      refresh();
      endwin();
      exit(EXIT_FAILURE);
    }
    else if(fi->Start > fi->FileInfo.st_size) {
      addstr("list: You're asking for bytes starting past the end of the file...");
      refresh();
      endwin();
      exit(EXIT_FAILURE);
    }
    fi->FEnd=fi->FileInfo.st_size;
  }
  else { /* Huh? input _IS_ stdin? */
    addstr("list: Seeking to end...");
    refresh();
    cbreak();
    noecho();
    /*      nodelay(); */
    /*    if((c=getchar())=='Q') Bye(2, __LINE__); */
/*    while(c!=EOF) c=getchar();  wait for key ... */
    fseek(fi->FPtr, 0, SEEK_END);
    if(!(fi->FEnd=ftell(fi->FPtr))) {
      addstr("list: Ooops - selected file is empty\n");
      refresh();
      endwin();
      exit(EXIT_FAILURE);
    }  /* end of if file empty */
  } /* end of else file==STDIN */
  /* Ummm, seems these lines are not needed...they already get seen to in NewFile() */
  /******************************************************/
  /*  ScanForCr(fi);    / * this should hopefully give me the correct
                     * number of lines in my data * /
  / * looks like I'm getting a mismatch - revamp the routine?  * /
  if(!(fi->CrArray=calloc(fi->FLines, sizeof(u_long)))) Bye(1, __LINE__);
  fi->CrArray=AllocateLines(fi); / * This allocates the line ends */
  /*******************************************************/
  if (fi->Count==0)
    fi->Count = fi->FEnd;
  if(fi->DumpFlag!='7' && fi->DumpFlag != '8')
    fi->DumpFlag = '7';  /* sets allchars/printchars switch - make it 7 just for safety on binary files */
  /* Go ahead and show the file on the screen */
  Dump(fi);
  /* Now shut down and tidy everything up */
  fclose(fi->FPtr);
  /*    fflush(stdout); *  is this needed? */
  /*    setmode(fileno(stdin), O_TEXT); * Ooops, another DOS-ism crept in here */
  free(fi->CrArray);
  clear();
  refresh();
  endwin();
  return (EXIT_SUCCESS);
}  /* End of main() */

void CartWheel(void)  {
  int i;
  char tst[2];
  char *tstr=&tst[0];
  char whirl[] = { '\\', '|', '/', '-' };
  for(i = 0; i <= 3; i++) {
    move(22,39);
    sprintf(tstr,"%c", whirl[i]);
    addstr(tstr);
    refresh();
    /*    delay(100); */
  }
}

/*
 * Is the specified file a directory?
 * (This code is lifted straight out of less 3.58 and de-K&R'ed)
 */
#ifndef S_ISDIR
#define S_ISDIR(m)      (((m) & S_IFMT) == S_IFDIR)
#endif
#ifndef S_ISREG
#define S_ISREG(m)      (((m) & S_IFMT) == S_IFREG)
#endif

int is_dir(char *filename) { /* Hey, why do I need this routine, anyway? */
  int isdir = 0;

  filename = unquote_file(filename);  /* this routine I'll have to duplicate too... */
  {
    int r;
    struct stat statbuf;

    r = stat(filename, &statbuf);
    isdir = (r >= 0 && S_ISDIR(statbuf.st_mode));
  }
  free(filename);
  return (isdir);
}

/*
 *  * Remove quotes around a filename.
 *   */
char * unquote_file(char *str) {
  char *name;
  char *p;
  char openquote = '"';
  char closequote = openquote;

  if (*str != openquote)
  return (save(str));
  name = (char *) ecalloc(strlen(str), sizeof(char));
  strcpy(name, str+1);
  p = name + strlen(name) - 1;
  if (*p == closequote)
  *p = '\0';
  return (name);
}


/*
 * Copy a string to a "safe" place
 * (that is, to a buffer allocated by calloc).
 */
char *save(char *s) {
   register char *p;

   p = (char *) ecalloc(strlen(s)+1, sizeof(char));
   strcpy(p, s);
   return (p);
}

/*
 * Allocate memory.
 * Like calloc(), but never returns an error (NULL).
 */
void *ecalloc(int count, unsigned int size) {
  register void *p;

  p = (void *) calloc(count, size);
  if (p != NULL)
    return (p);
/*  error("Cannot allocate memory", NULL_PARG);
  quit(QUIT_ERROR);   */
  Bye(1, __LINE__);
  /*NOTREACHED*/
  exit(1);
}


/*
 *  Allocate a big buffer, use it to buffer a specified stream
 *  - but why does it have to be declared static?
 */

static size_t fsetup(FILE *fp, size_t minbuf) {
  register size_t bufsize;
  register char *buffer;

/* Allocate the largest buffer we can */

  for (bufsize = 0x4000; bufsize >= minbuf; bufsize >>= 1) {
  if (NULL != (buffer = (char *) malloc(bufsize)))
    break;
  }
  if (NULL == buffer)
    return 0;

  /* Use the buffer to buffer the file */

  if (0L == setvbuf(fp, buffer, _IOFBF, bufsize))
    return bufsize;
  else  return 0;
}

/*
**  Search a file for a pattern match (forward)
**
**  Arguments: FILE pointer
**             pattern to search for
**             size of pattern
**             find Nth occurrence
**
**  Returns: -1L if pattern not found
**           -2L in case of error
*/

long ffsearch(struct FileData *fi, int N) {  /* 1st br */
  /* Hmmm, seems we don't need size, we can surely get that from strlen(SearchString) */
  long pos = -2L, tempos = 0L;
  char *sbuf, *p;
  size_t skip, size;
  int ch = 0;
  size = strlen(fi->SearchString);  /* We want the length of the string... 8-) */

/* Allocate a search buffer */

  if (NULL == (sbuf = (char *)malloc(size - 1)))
    goto FDONE;

  /* Buffer the file and position us within it */

  if (0 == fsetup(fi->FPtr, size))
    goto FDONE;
  pos = -1L;
  fseek(fi->FPtr, fi->FPosn, SEEK_SET);

  /* Set up for smart searching */

  if (1 < strlen(fi->SearchString) && NULL != (p = strchr(fi->SearchString + 1, *fi->SearchString)))
    skip = p - (char *)fi->SearchString;
  else  skip = strlen(fi->SearchString);

  /* Look for the pattern */

  while (EOF != ch) {  /* 2nd in */
    if (EOF == (ch = fgetc(fi->FPtr)))
      break;
    if ((int)*fi->SearchString == ch) {  /* 3rd in */
       tempos = ftell(fi->FPtr);
       if (size - 1 > fread(sbuf, sizeof(char), size - 1, fi->FPtr))
         goto FDONE;
       if (0L == memcmp(sbuf, &fi->SearchString[1], size - 1)) {  /* 4th in */
         if (0L == --N) {  /* 5th in */
           pos = tempos - 1L;
     goto  FDONE;
         }  /* 5th out */
       }  /* 4th out */
       fi->SPosn = ftell(fi->FPtr);  /* uh?  this doesn't seem to work... */

     } /* 3rd out */
  } /* 2nd out */
/* Clean up and leave */

FDONE:
  free(sbuf);
  return pos;
} /* end of ffsearch */


/*
**  Search a file for a pattern match (backwards)
**
**  Arguments: FILE pointer
**             pattern to search for
**             size of pattern
**             find Nth occurrence
**
**  Returns: -1L if pattern not found
**           -2L in case of error
*/

long rfsearch(struct FileData *fi, int N) {
/* Again, do we need size?  Again, size can be got from strlen(fi->SearchString) -
 * in fact, now I come to think of it, fi->SearchString could provide *fi->Search
 */
  long pos = -2L, tempos;
  char *sbuf, *p;
  size_t skip, size;
  int ch = 0;
  size = strlen(fi->SearchString);
  /* Allocate a search buffer */

  if (NULL == (sbuf = (char *)malloc(size - 1)))
    goto RDONE;

  /* Buffer the file and position us within it */

  if (0 == fsetup(fi->FPtr, size))
    goto RDONE;
  pos = -1L;
      fseek(fi->FPtr, fi->FPosn, SEEK_SET);
      tempos = ftell(fi->FPtr) - strlen(fi->SearchString);

      /* Set up for smart searching */

      if (1 < strlen(fi->SearchString) && NULL != (p = strrchr(fi->SearchString + 1, *fi->SearchString)))
            skip = strlen(fi->SearchString) - (p - (char *)fi->SearchString);
      else  skip = strlen(fi->SearchString);

      /* Look for the pattern */

      while (0L <= tempos)
      {
            fseek(fi->FPtr, tempos, SEEK_SET);
            if (EOF == (ch = fgetc(fi->FPtr)))
                  break;
            if ((int)*fi->SearchString == ch)
            {
                  if (size - 1 <= fread(sbuf, sizeof(char), size - 1, fi->FPtr))
                  {
                        if (0L == memcmp(sbuf, &fi->SearchString[1], size - 1))
                        {
                              if (0 == --N)
                              {
                                    pos = tempos;
                                    goto RDONE;
                              }
                        }
                  }
                  tempos -= skip;
            }
            else  --tempos;
      }

      /* Clean up and leave */

RDONE:
      free(sbuf);
      return pos;
}

char *SearchParse(const char *Search)  {
   char *NewSearch;
   NewSearch = (char *) malloc(sizeof(NewSearch) * (strlen(Search) + 1));
   /* This is where things need to be looked for, such as \ processing */
   return NewSearch;
}

int CmdMode(struct FileData *fi)  { /* 1s */
/* In here, I'd like to define a vim-like interface for adding commands like (N)ext file/(P)rev file... */
/* First, steal a little code, massage a little code... */
/* Various switches go through here...
  N - next file
  P - Prev file
  * - else, back to program
  Q - obvious
 */
  int c;
  char *WhatDoIPutInHere = (char *) malloc(sizeof(WhatDoIPutInHere) * PATH_MAX);
  char *OldFName = (char *) malloc(sizeof(OldFName) * PATH_MAX);
  strcpy(OldFName, fi->FName);  /* This is supposed to provide a fallback filename */
/* Assign WhatDoIPutInHere to a member of the array of filenames we were fed
 * at startup */

  switch(c=getch()) { /* 2 */
    /* We need processing for esc chars here... */
  case 'n':          /* next file */
  case 'N':
  /* Incidentally, what is argc at this stage? */
    NewFile(fi, WhatDoIPutInHere);
    break;
  case 'p':          /* next file */
  case 'P':
  /* Incidentally, what is argc at this stage? */
    NewFile(fi, WhatDoIPutInHere);
    break;
/*  case 'Q':          / * Quit - (Hasta La Vista!) * /
  case 'q':
    return 0;  / * Hmmm, why should we quit from here? */
  default:
    return 0;
    break;
  } /* 2e */
 return 0;
}

/*
void AsciiTable(void) {
char    squaretop=" 218 ÚÄÄÄÄÄÂÄÄÄ¿ 191 ";
char     whatever=" 179 ³ 196 194 ³ 179 ";
char   squarejoin=" 195 Ã 197 Å   ´ 180";
char   squareside=" 179 ³ 193     ³ ";
char squarebottom=" 192 ÀÄÄÁÄÄÄÄÄÄÙ 217";
}
*/

