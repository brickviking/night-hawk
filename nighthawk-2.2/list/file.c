/* File to include to List project
 */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <dirent.h>
#include <sys/stat.h>
#include <curses.h>
#include "list.h"


int NewFile(struct FileData *fi, char *NewName) {
/* It seems I should abandon local variables and just work with the globals *
 * I already have fi->FName, so I could just work from that...though if I've
 * already nuked fi, what do I start with?  An assignment of tempname to fi->FName?
 * Assign some space for the new filename...space should already have been freed
 * from any previous uses */
  /* Now the def'n for PATH_MAX has disappeared, we have to #define it somewhere... */
  /* OOOPS!!!! What if FName overruns 256 bytes? Or PATH_MAX bytes? */
  if(NULL == (fi->FName = (char *) malloc(sizeof(NewName) * PATH_MAX))) Bye(1, __LINE__); /* Previously freed, or not alloc'ed until here */
  if(NewName=="") {
    Bye(7, __LINE__);
  }
  strcpy(fi->FName, NewName);
  if(stat(fi->FName, fi->FileInfoPtr)) {  /* Could this be replaced with is_dir() or augmented? */
    /* Stick the stat() data into the new structure, and return the value to the program.
     * Anything except a return of zero is classed as an error.  I made this a little
     * more polite and explained *why* we can't look at file, using a switch{} statement -
     * all shifted into Bye() */
    Bye(3, __LINE__);
  }
  /* So, we sucessfully stat()'ed it, now, can we open it for reading? */
  if( !( fi->FPtr = fopen(fi->FName, "rb"))) {  /* i.e. if fopen returns NULL - though I should never get here...*/
    /* Again, this could be replaced by debug_function() */
    Bye(7, __LINE__); /* Should be different from Bye(3..) because I'm using fopen, not ...? */
  }
/* ... otherwise, file open succeeded, and we have the handle in fi->FPtr
 * There's a weakness here - what if I want to open multiple files? This burns and
 * crashes at the first file it can't open, and (presumably) dumps the rest of its args
 * Perhaps we should continue with the next param (if it's a filename...) and only fall out
 * if the remaining files cannot be opened.
 */
    if(fi->FileInfo.st_size == 0) {  /* get the filesize... */
      errno = ENOSYS;  /* Means - haven't implemented function yet... */
      Bye(8, __LINE__); /* the file appears to be of zero length, exit politely */
    }
  fi->FEnd = fi->FileInfo.st_size;  /* sets the filesize */
  fi->Count = fi->FEnd; /* ... so that we don't overrun the end */
  /* Previous line doesn't take into account users wishes for part of file */
  fseek(fi->FPtr, 0, SEEK_SET); /* Set up the file pointer at the beginning of the file */
/*  free(fi->CrArray); / * Should clear this before I reallocate it */
  /* Hey, what if it hasn't been allocated yet? Hmmm, 'nother bug fixed... */
  fi->FPosn = fi->Start; /* Do I want to start from the same point? If new file, then Start should change... */
  fi->FLineCtr = 0; /* Not sure about this... */
  return(0);
} /* End of NewFile() */

/***********************************************************************
 * Routine to either: find fi->Scrn_x chars, stuff pointer in next position of
 * array, or if we find a CR before that, stuff THAT value into next
 * position of array
 * Problems: could need to realloc several times, depending on the number
 * of lines longer than Scrn_x chars
 ***********************************************************************/
void ScanForCr(struct FileData *fi) {
  /* We'll start with an array of CrInFile size and expand it if necessary */
  long int i, j, linelength, llflag = 0, c;
  for(i = 0, j = 1; i < fi->FEnd;)  {
   for (linelength = 0, c = 0; linelength < fi->Scrn_x - 10; linelength++) /* finish should actually be 4096 or something...
   hmmm, how about making it say, one screen - 1 line in size, so that we don't lose lines when we go down one line? */
      {
        switch (c=fgetc(fi->FPtr))
          {
          case '\n':
            j++;
            llflag++;
            break;
          default:
            break;
          }/* end of case statements */
        i++;
        if(llflag) {
          break;
        }
      }  /* end of switch and of for linelength loop */
    /*  if j hasn't been added to, stuff value in anyway, then realloc array  */
    if(!llflag) {
      j++;
    } /* end of llflag check */
    else llflag--;
  }
  fi->FLines=j;
  fseek(fi->FPtr, 0, SEEK_SET); /* back to beginning of file for further operations */
  return;
}

long *AllocateLines(struct FileData *fi) {
  /* We'll start with an array of CrInFile size and expand it if necessary
   * Another idea (28-7-2000) would be to implement a while loop - peeling off
   * COLS chars at a time until the line falls below COLS in length.  Stick
   * each end val into the fi->CrArray
   * For example:
   * MainEngine {
   * Len = ReadALinesLength(fi->FPtr, fi->FPos);
   * do  {
   *   fi->CrArray[LineCtr++] = fi->FPos;
   *   fi->FPos += COLS;
   *   Len -= COLS;
   * }
   * until (Len <= COLS);
   * } / * End of MainEngine * /
   *
   * ReadALinesLength(fi)  {
   *   pseudocode is:
   *   Line begins at n, finishes at CR, return CR position or EOF if End Of File hit first
   */
  int linelength, llflag=0, c;
  long int i, j;
  fi->CrArray[0]=0;
  move(fi->Scrn_y - 1, 0);
  clear();
  refresh();
  addstr("list: Counting lines ... please wait");
  for(i = 0, j = 1; i < fi->FEnd;)  {
/*    for (linelength = 0, c=0; linelength < 80 ; linelength++ ) */
    for (linelength = 0, c = 0; linelength < fi->Scrn_x - 10; linelength++)  /* Took 10 bytes off to add in line numbers */
       {
  c = fgetc(fi->FPtr);
        switch (c)
          {
          case '\n':
            *(fi->CrArray+j)=i+1;
            j++;
            llflag++;
/*            if(debug) {
              char *tempstring;
        tempstring = (char *) malloc(sizeof(tempstring) * 80); / * Surely this should be long enough to list numbers up to several million? * /
              sprintf(tempstring, "CR:%ld %ld\t", i-1, j);
              debug_function(tempstring, 0, 0, __LINE__);
        free(tempstring);
            } */
            break;
/* /          case '\t':  / * Whoops, this doesn't work... wonder why not? */
        /* Ah, I think I've found why not!  linelength isn't checked for >160 conditions after updating *Sheesh*
         * The tabs are also not counted to their nearest mod equivalent * /
        if(linelength >= (COLS - fi->TabSize)) {
    linelength = COLS;
    break;
              }
              else
    linelength += 4; / * The case should be sorted out from here */
          default:
            break;
          }/* end of case statements */
        i++;
        if(llflag) {
          break;
        }
      }  /* end of switch and of for linelength loop */
    /*  if j hasn't been added to, stuff value in anyway, then realloc array  */
    if(j > (fi->FLines)) { /* now this line should NEVER happen */
      /* if(debug)*/  addstr("\nReallocating...\n");
      if(( fi->CrArray = realloc(fi->CrArray, j)) == NULL) {
        addstr("\nWhoops - error trying to realloc!!\n");
        Bye(1, __LINE__);  /* Jump to end */
      } /* failed to realloc */
    } /* end of check for CrArray size */
    if(!llflag) { /* if 80 chars got to, and CR not found, set */
      *(fi->CrArray+j++)=i;
      if(debug) {
        char tmpstr[81];
/*      char tmpstr[COLS+1]; */
        char *tempstring=&tmpstr[0];
        sprintf(tempstring,"L:%ld %ld\t", i-1, j);
        addstr(tempstring);
      }
    } /* end of llflag check */
    else llflag--;
  } /* end of first for loop */
  fi->FLines=j;
  fseek(fi->FPtr, 0, SEEK_SET);
  return fi->CrArray;  /* can I do this???? */
}

int EditFunction(struct FileData *fi) {
  int RetVal;
  char *CmdParam = (char *) malloc(sizeof(CmdParam) * 1024); /* Lets get some space here */
  char *CmdName = (char *) malloc(sizeof(CmdName) * 1024); /* Lets get some space here */
/*  return 1; / * For the moment (18-8-2000 10:43) return "Cannot do" until I write the rest of the code */
  echo();
  nl();
  endwin();
  /* This is a quick test for write-ability - if we have permission to append, then
   * (presumably) we have permission to edit the file.
   */
  if(NULL == (fopen(fi->FName, "a+"))) { /* Ooops, can't open the file for appending, so ... */
    RetVal = -1;
    fopen(fi->FName, "rb"); /* Re-open file readonly */
  }
  else {
    char *MyReturn;
    if(NULL == (MyReturn = (char *) malloc(sizeof(MyReturn) * 160)))
      Bye(1, __LINE__); /* Usual memory lack message */
/*    int OffSet; */
    if(fclose(fi->FPtr)) 
      Bye(9, __LINE__); /* We *HAVE* to close this first */
    /* Build the command string - we need to get $EDITOR if it exists */
    /* getenv returns a char * so testing it for NULL and assigning it could work... */
    if(!getenv("EDITOR")) { /* If EDITOR doesn't exist in environment... */
      /* Options are: popen(), exec(), fork(), and system().  Problem is exec doesn't return (I don't think) 
       * and fork() lets the parent program continue - not needed here... popen is for pipes...
       */
       sprintf(CmdName, "%s -f", "vim"); /* This only works if vim exists on the system...and -f is for foreground */
      /* Otherwise, we have to go hunt for an editor - emacs anyone? */
    }
    else { /* Otherwise copy editorname from EDITOR to cmdline */
      strcpy(CmdName, getenv("EDITOR"));
      /* this getenv is done twice - how can we make this more efficient? */
    }
    sprintf(CmdParam, "%s +%ld %s", CmdName, fi->FLineCtr, fi->FName);
    /* Again, this only works if editor takes a line offset
     * Add CmdName to Commandline, and an offset within file ...then add the filename
     * Luckily most Linux editors allow the +nn to jump straight to a specific
     * line number
     */
/*    OffSet = strlen(CmdParam) + 1;
    sprintf(CmdParam + OffSet, " %s", fi->FName); / * Add the filename... something's a little funny here...*/
    if(debug)
      debug_function(CmdParam, 0, fi->Scrn_y - 1, __LINE__);
    RetVal = system(CmdParam); /* Do the editing thing and return the code the program finished with... */
    refresh(); /* like they say... stops the mangling of the output string */
    nonl();
    noecho();
    if(0 < RetVal) {
      sprintf(MyReturn, "Edit_Function returned %d", RetVal); /* This is some more debugging code */
    /* There's a bug here - the return val gets mangled on display - this might have been fixed... 27-8-2K */
      /* We CERTAINLY want to know if the edit prog fell over */
      debug_function(MyReturn, 0, fi->Scrn_y - 1, __LINE__);
    }
    free(MyReturn);
  }
  refresh();  /* This is to restore the screen */
  nonl();
  noecho();
  free(CmdParam);
  free(CmdName);
  return RetVal;
}

int Search(struct FileData *fi)  { /* 1s */
  switch (fi->SearchDirection) { /* 2s */
    case 'f':
       fi->SPosn = ffsearch(fi, 1); /* First, get the strings position if found... */
       break;
    case 'b':
       fi->SPosn = rfsearch(fi, 1);
       break;
  } /* 2e */
  if((-2L) == fi->SPosn)
      Bye(1, __LINE__);  /* This dumps if no memory to assign strings in ffsearch() ... */
  if((-1L) == fi->SPosn) { /* ... Hasn't died yet, so check if string was _not_ found ... 2s */
      debug_function("String not found - hit any key to return", 0, fi->Scrn_y, __LINE__);
  } /* 2e...*/
  else  {   /* String was found - so ... 2s */
      if(fi->DumpMode == 'x')
        fi->FPosn = fi->SPosn; /* ... set the file position to where the search was found... */
      else  { /* We have to seek to the nearest line... 3s */
        for(fi->FLineCtr = 0; fi->FLineCtr < fi->FLines; fi->FLineCtr++ ) { /* 4s */
        /* sets initial fi->FLineCtr for line p'sn retrieval */
          if(*(fi->CrArray+fi->FLineCtr) >= fi->SPosn) { /* 5s */
      /* Step through the lines until we find one that is greater than SearchPosition */
      fi->FPosn = *(fi->CrArray + fi->FLineCtr - 1);  /* ... set the required file position ... */
       /* Not sure why it needs one line taken off, but I guess the line itself needs to be included */
            break;
          } /* 5e */
        }  /* Now I've got the right line, step through the chars ... 4e */
      } /* End of test for DumpMode  ... 3e */
    } /* 2e */
  return 0;
} /* 1e */

int Bye(int ByeReason, int LineCalled)  {
  char *ErrMessage = (char *) malloc(sizeof(ErrMessage) * 127);
  if(ErrMessage == NULL) {
    printf("list: Wow! We are REALLY out of memory here!\n");
    exit(EXIT_FAILURE);
    }
  /* Stick all the curses clean-up here at the top, so it's not shared... waste of good code otherwise */
  switch(ByeReason) {
    case 1:
         sprintf(ErrMessage, "list: Ooops - out of memory at line %d", LineCalled);
         break;
    case 2:
         sprintf(ErrMessage, "list: exiting debug loop and leaving Program...\n");
         break;
    case 3:
         switch (errno)  {
           case ENOENT:
               sprintf(ErrMessage, "list: File not found when called from line %d!\n", LineCalled);
               break;
           case EACCES:
               sprintf(ErrMessage, "list: You do not have permission to look at this file\n");
               break;
#ifndef _MINIX
           case ELOOP:
               sprintf(ErrMessage, "list: Got caught trying to follow too many symbolic links\n");
               break;
#endif
           default:
               sprintf(ErrMessage, "list: something unknown is wrong here when trying to open a file\n");
         } /* End of switch for case 3... */
         break;
  case 4:
   sprintf(ErrMessage, "list: String handling error at %d ", LineCalled);
   break;
  case 5:
   sprintf(ErrMessage, "list: you asked for a directory instead of a file - can't oblige yet.");
   break;
  case 6:
   sprintf(ErrMessage, "list: Ooops, no colour available here");
   return (0);
   /* This shouldn't stop the program from working...just do things in b&w */
   break;
  case 7:
   /* This is different from the case 3 version (ENOENT), because we were using fopen,
    * and we know the file exists, but we can't open it for reading - shouldn't this be an EACCES problem? */
   sprintf(ErrMessage, "list: Cannot open file for reading - permissions?");
   break;
  case 8:
   sprintf(ErrMessage, "list: Huh? You want me to open an empty or non-regular file? I ain't THAT bright!");
   break;
  case 9:
   sprintf(ErrMessage, "list: Sorry - your terminal doesn't have enough columns!");
   break;
  case 10:
   sprintf(ErrMessage, "list: Sorry - your terminal doesn't have enough lines!");
   break;
  case 11:
   sprintf(ErrMessage, " "); /* No need for a error message, but I have to shut up perror with something */
   Usage(); /* This was added in here to reorganise */
   break;
  case 12: /* Use this until we get back the code for opening files specified on the commandline. */
   sprintf(ErrMessage, "list: You didn't give me a filename to open. I'm out of here");
   Usage();
   break;
  default:
   sprintf(ErrMessage, "list: Something else is wrong at line %d...", LineCalled);
   break;
    }
  perror(ErrMessage);
  exit(ByeReason);  /* Might as well exit using the reason as a return value - at least until I find a better reason not to */
}

void CloseNCurses(void) {
  clear();
  refresh();
  nl();
  nocbreak();
	endwin();
	return;
}

