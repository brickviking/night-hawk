#include <stdlib.h>
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#include <curses.h>
#include "list.h"

char *SearchPrompt(struct FileData *fi) {
  char *Search = (char *) calloc(sizeof (Search) * 1024, sizeof(Search));
  char *tempstring = (char *) calloc(sizeof (tempstring) * 1024, sizeof(tempstring));
  char *Direction = (char *) calloc(sizeof (Direction) * 9, sizeof(Direction)); /* forward or backward prompt string */
  fi->SPosn = (long) NULL; /* set the current position */
  /* search string forwards - ifnot found (-1), prompt user */
  move(fi->Scrn_y - 1,0);
  clrtoeol();
  echo();
  attron(A_REVERSE);
  switch (fi->SearchDirection) {
    case 'f':
   strcpy(Direction, "forward");
   break;
    case 'b':
   strcpy(Direction, "backward");
   break;
  }
  sprintf(tempstring, "Search %s for this string: ", Direction);
  addstr(tempstring);
  attroff(A_REVERSE);
  refresh();
  free(tempstring);
  getstr(Search);  /* Input a new search string */
  noecho();
  return Search;  /* gives us back the value we want to search for */
  /* Only thing is...how do we free() Search? */
} /* End of SearchPrompt() */

char WhatNext(struct FileData *fi) { /* 1 */
/* Various switches go through here...
  Space - next page
  7 - 7-bit   8 - 8-bit  * - toggle line-endings
  Q - obvious
  O - decimal/hex toggle on offset address in hex view - wonder if this should
      be extended to the data display too?
  G - goto a byte / line  % - goto x percent of file
  R - toggle ruler line (removed dependency upon 80 char wide screens...)
  / - search forward for string
  \ - search backward for string (? is already taken ...and doesn't work under X anyway)
     Thanks to Dion Bonner <bonner@xtra.co.nz> for finding this one out!
  ** We should be reporting the right key but we're not (always) getting it **
 */
  int c;
  long int OldOffset; /* Only used in Edit */
  /* This line might be responsible for leaking memory */
  char *OldFName = (char *) malloc(sizeof(OldFName) * PATH_MAX);
  char buf2[PATH_MAX]; /* Make this somewhat safer for overflow */
  char *buf2p;
/* Why don't I just allocate a section of memory? */
  buf2p=&buf2[0];

  switch(c=getch()) { /* 2 */
    /* We need processing for esc chars here... */
  case ' ':          /* next page or (LINES*16) bytes */
  case KEY_NPAGE:
/*    clear();  / * Added this to see what happens... */
    break;
  case '7':
    BackPage(fi, fi->Scrn_y-1);
    fi->DumpFlag='7';
    break;
  case '8':
    BackPage(fi, fi->Scrn_y-1);
    fi->DumpFlag='8';
    break;
  case '*':
    BackPage(fi, fi->Scrn_y-1);
    fi->DumpFlag='*';
    break;
  case 'Q':          /* Quit - (Hasta La Vista!) */
  case 'q':
    return 0;
  case 'o':
  case 'O':
/*    if(!fi->addrflag)
      fi->addrflag=1;
    else
      fi->addrflag=0; * This code replaced by the single line below...simple, isn't it? */
    fi->addrflag = fi->addrflag ? 0 : 1; /* If it isn't zeroed - zero it, else set it */
    BackPage(fi, fi->Scrn_y-1);
    break;
    /*    if(getenv(TERM)=="linux") { */
  case KEY_END:
    if(fi->DumpMode == 'x') {
      fi->FPosn= (ruler) ? fi->FEnd - ((fi->Scrn_y-2) * fi->ScrnWide) : fi->FEnd - (fi->Scrn_y * fi->ScrnWide);
    }
    else if (fi->DumpMode == 't') {
      fi->FLineCtr=(ruler) ? fi->FLines - fi->Scrn_y - 2 :  fi->FLines - fi->Scrn_y - 1;
      fi->FPosn=(*(fi->CrArray+fi->FLineCtr));
    }
    fseek(fi->FPtr, fi->FPosn, SEEK_SET);
    break;
  case ':':
/*    CmdMode(fi); */ /* Comment this out until the related routine works */
    break;
  case 'g':  /* Go to a specific position/line... */
  case 'G':  /* I can basically steal this code for % (go to % of file) */
    move(fi->Scrn_y-1,0);
    clrtoeol();
    attron(A_BOLD);
    if(fi->DumpMode=='x')  {
      addstr("Byte offset: ");
    }
    else
      {
        addstr("Line offset: ");
      }
    attroff(A_BOLD);
    refresh();
    echo();
    getstr(buf2);  /* Input an offset (numerical) */
    noecho();
    if(fi->DumpMode=='x') {
      errno = 0;
      fi->FPosn=strtol(buf2, &buf2p, 0); /* Set fileposition */
      if(errno) {
	      CloseNCurses();
	      Bye(4, __LINE__);
      }
      /* Have to check for FPosn within 0 <= FPosn <= FEnd  */
      if(fi->FPosn > fi->FEnd) {
        fi->FPosn = (ruler) ? (fi->FEnd - (fi->Scrn_y-1)*fi->ScrnWide) : (fi->FEnd - (fi->Scrn_y*fi->ScrnWide));
      }
      else
        if(fi->FPosn < 0)
          fi->FPosn=0;
    }
    else if(fi->DumpMode=='t') {
      fi->FLineCtr=strtol(buf2, &buf2p, 0); /* Set fi->FPosnition */
      if (fi->FLineCtr >= fi->FLines)
        fi->FLineCtr = (ruler) ? (fi->FLines - fi->Scrn_y - 2) : (fi->FLines - fi->Scrn_y - 1);
      else if(fi->FLineCtr < 0)
        fi->FLineCtr=0;
      fi->FPosn=*(fi->CrArray+fi->FLineCtr);
    }
    fseek(fi->FPtr, fi->FPosn, SEEK_SET);
    clear();
    refresh();
    break;
  case '%':
    move(fi->Scrn_y-1,0);
    clrtoeol();
    attron(A_BOLD);
    addstr("Percentage offset (0-100%): ");
    attroff(A_BOLD);
    refresh();
    echo();
    getstr(buf2);  /* Input an offset (numerical) */
    noecho();
    /* I basically need to go to bytes_of_file * percent_input...now how large is fi? */
    fi->FPosn=(((strtol(buf2, &buf2p, 0))*fi->FEnd)/100);
    /* Set the fileposition that we want */
      /* Have to check for FPosn within 0 <= FPosn <= FEnd ... or do we? */
    if(fi->FPosn > fi->FEnd) {
        fi->FPosn = (ruler) ? (fi->FEnd - (fi->Scrn_y-1)*16) : (fi->FEnd - (fi->Scrn_y*16));
      }
      else
        if(fi->FPosn < 0)
          fi->FPosn=0;
    fseek(fi->FPtr, fi->FPosn, SEEK_SET);
    clear();
    refresh();
    break;

  case 'R':         /* Toggle the ruler  */
  case 'r':
    BackPage(fi, fi->Scrn_y-1);
    if(!ruler) ruler++;
    else ruler--;
    break;
  case 'T':  /* Go back one page, change to text dump - like less */
  case 't':
    BackPage(fi, fi->Scrn_y-1);
    if(!fi->Quick)
      fi->DumpMode='t';
    else
      /* QuickMessage(); */
      {
      debug_function("Can't change display - in Quick mode", 0, fi->Scrn_y - 1, __LINE__);
      }
    break;
  case 'X':  /* go back one page then display hex dump */
  case 'x':
    BackPage(fi, fi->Scrn_y-1);
    fi->DumpMode='x';
    break;
  case KEY_DOWN:          /* one line forward */
    BackPage(fi, fi->Scrn_y-2);
    break;
  case KEY_UP:  /* One line backward */
    BackPage(fi, fi->Scrn_y);
    break;
  case KEY_PPAGE:  /* One page backwards */
  case 'b':
  case 'B':
    BackPage(fi, fi->Scrn_y*2-2);
    break;
  case KEY_HOME:   /* Beginning of file... */
    fi->Start=0;
    rewind(fi->FPtr);
    fi->FPosn = 0;
    fi->FLineCtr=0;
    clear();
    refresh();
    break;
  case 'L':
  case 'l': /* This one's a toggle, folks */
    BackPage(fi, fi->Scrn_y-1);
    if(fi->LineNumbers)
      fi->LineNumbers= 0;
    else
      fi->LineNumbers++;
    break;
  case 'h':
  case 'H':
    BackPage(fi, fi->Scrn_y - 1);
    if(fi->Scrn_x < 80 )  {
      debug_function("Can't display - not wide enough", 0, fi->Scrn_y - 1, __LINE__);
      break;
    }
    ShowHelp(fi);
    /*           if(!(fi->FPtr==stdin)) {
                 } */
    break;
  case 'S':
  case 's':
    BackPage(fi, fi->Scrn_y - 1);
    if(fi->Scrn_x < 80 )  {
      debug_function("Can't display - not wide enough", 0, fi->Scrn_y - 1, __LINE__);
      break;
    }
    ShowStats(fi);
    break;
  case 'F':
  case 'f':
    move(fi->Scrn_y - 1,0);
    clrtoeol();
    echo();
    attron(A_REVERSE);
    addstr("New file name: ");
    attroff(A_REVERSE);
    refresh();
    getstr(buf2);  /* Input a new filename */
    noecho();
    fclose(fi->FPtr);  /* I'd better close it before opening another one */
    free(fi->FName); /* frees the memory associated with the name... */
    free(fi->CrArray); /* ... same as above ... */
/*    fi->FName=buf2p; */
/*    fi=NewFile(fi, buf2p); / * This *should* open the new fi->FName */
/* Whoops - looks like there are some troubles with this method */
/*    fseek(fi->FPtr, fi->FPosn, SEEK_SET); * This line shouldn't happen, after an fclose */
    NewFile(fi, buf2p);  /* Does the job of opening the new file and setting up fi */
    clear();
    break;
  case '/':
    BackPage(fi, fi->Scrn_y - 1);
    fi->SearchDirection = 'f'; /* Set the direction */
    fi->SearchString = SearchPrompt(fi); /* Grab the string from user */
  /* Here is where the input needs to be parsed, where a mini-language needs to be defined
   * for what we are prepared to accept.  For example, you could use \ to escape the next
   * character and putting it into the searchpattern literally.  Here would also be the
   * interpretation of, say, octal or hex sequences.
   * say, a routine called SearchParse()?
   */
   /* Ooops - found another bug here - if string is empty, then program bombs.. */
    if(1 > strlen(fi->SearchString)) break;  /* no point in continuing... */
    Search(fi);  /*  and go do the search */
/*****************************************
 * Do I have to malloc before assigning? *
 *****************************************/
    clear();
    /* Clear off the screen to save us the hassle of seeing odd strings in weird places
     * but, does it really reduce screendroppings? */
    /* There's more to do here...stick string in the middle of the screen perhaps? */
    break;
  case '\\':
    BackPage(fi, fi->Scrn_y - 1);
    fi->SearchDirection = 'b';
    fi->SearchString = SearchPrompt(fi); /* Grab the string from user */
    Search(fi); /* do the search... */
    /* There's more to do here...stick string in the middle of the screen or highlight it... */
    break;
  case 0x0c:  /* Ctrl-L - repeats the search (hopefully...), but will this work for all termtypes? */
  case 'n': /* Hm, I just hope this doesn't depend on the difference between hex and text */
       BackPage(fi, fi->Scrn_y - 2);
       Search(fi);  /* Ahhh, yes. This seems a lot simpler than what I had...we already have string, so repeat search */
       break;
  case 'z':
  case 'Z':
       debug = debug ? 0 : 1;
       break;
  case 'W':
  case 'w':
       BackPage(fi, fi->Scrn_y - 1);
       move(fi->Scrn_y - 1,0);
       clrtoeol();
       echo();
       attron(A_REVERSE);
       addstr("Bugwrite file name (<Enter> for list.debug in home dir): ");
       attroff(A_REVERSE);
       refresh();
       getstr(buf2);  /* Input a new filename */
       /* I don't know what happens if I enter nothing - I WANT it to use a
	* default value of $HOME/list.debug */
       noecho();
       fi->BugWriteFName = buf2; /* Throw it into the struct */
       BugWrite(fi);
       break;
  case 'E':
  case 'e':
       /* This next line's return should be checked... */
       BackPage(fi, fi->Scrn_y - 1);
       if(NULL == (strcpy(OldFName, fi->FName))) /* Make sure fi doesn't get clobbered until necessary */ {
         CloseNCurses();
         Bye(4, __LINE__); /* Bail out with an "String handling error" message */
       }
       fclose(fi->FPtr);  /* I'd better close it before editing - two processes can't have access at the same time */
       if(-1 == EditFunction(fi)) { /* If this function returned error... */
         debug_function("Cannot edit file - hit any key to return (q to quit)", 0, fi->Scrn_y, __LINE__);
   if(NULL == (fi->FPtr = fopen(fi->FName, "rb"))) /* Reopens, bails out if can't open */  {
     CloseNCurses();
     Bye(7, __LINE__); /* Want the "Can't open file" message */
   }
         free(OldFName); /* Free this, it's not needed any more */
   }
   else {
   free(fi->FName); /* frees the memory associated with the name... */
   free(fi->CrArray); /* ... same as above ... */
   fi->FName = (char *) malloc(sizeof(fi->FName) * PATH_MAX); /* Reallocate, 'cos we trashed it two lines up... */
   strcpy(fi->FName, OldFName);
   OldOffset = fi->FPosn;
   NewFile(fi, fi->FName); /* ... and reopen the file - this will also deal with CrArray */
   fi->FPosn = OldOffset;
   fseek(fi->FPtr, fi->FPosn, SEEK_SET); /* ... and this handles seeking back to where we were */
   free(OldFName); /* and free this var */
       }
       break;
  default:
    BackPage(fi,  (fi->Scrn_y - 1));
    break;
  }
  return fi->DumpMode;
} /* End of WhatNext() */

