#include <stdlib.h>
#include <sys/stat.h> /* Do I need this? */
#include <curses.h>
#include <dirent.h>
#include <time.h>
/* #include <forms.h> */
#include "list.h"

/* This function is a real simple function designed to take users entry of
 * bugs and stuff them into a specified file - the default will be a file
 * stored in the home directory of the user, but another idea could be to
 * email this file off to another user.
 */
int BugWrite(struct FileData *fi)  {
  FILE *NewFile;
  char *TempPath = (char *) malloc(sizeof(TempPath) * PATH_MAX);
  char *Tempstring = (char *) malloc(sizeof(Tempstring) * 1024);
 /* Of course I could always prompt user for a filename here...
   */
  if(NULL == fi->BugWriteFName) { 
    if(!getenv("TEMP"))  /* Whoops, no TEMP variable defined */
      strcpy(TempPath, "/tmp/list.debug"); /* This should be the last resort filename, not the default */
    else {
  /* Should this be a file in the users home dir instead? The only place where this
   * wouldn't work is where the home dir is not writeable. 
   */
      strcpy(TempPath, getenv("TEMP")); /* Else, we found a candidate dir to use */
      sprintf(TempPath + (strlen(TempPath) + 1), "list.debug"); /* lets hope this is the right length */
    }
  }
  else
     strcpy(TempPath, fi->BugWriteFName);
  /* First, we attempt to open the file for writing to ... */
  if(NULL == (NewFile = fopen(TempPath, "a+")))  { /* This opens the file to write to... */

    debug_function("Cannot open temporary file for writing to - hit any key to return", 0, fi->Scrn_y, __LINE__); /* 66 chars long */
    /* Should we add a var to struct for whether tmpfile was able to be written? */
    /* I know I could ask the user to specify a filename to write to... */
    return 1;  /* End of story */
  }
  else {
    TakeNotes(NewFile, fi);
  }
  if(fclose(NewFile)) {  /* ... and this closes the file when finished */
    sprintf(Tempstring, "Whoops - couldn't close bugwrite file %s", TempPath);
    debug_function(Tempstring, 0, fi->Scrn_y - 1, __LINE__ );
  }
return 0;
}

/* Select "write to file", or "email to user" */
int Choose(void) {
/*  debug_function(); */
  return 0;
}

int WriteToFile(void) {
  /* Here I intend to write the data in box to a user-selected file, permissions
   * permitting, of course
   */
  return 0;
}

int EmailToUser(struct FileData *fi)  {
#ifndef __linux__
  debug_function("Sorry, can't mail on this system", 0, fi->Scrn_y, __LINE__ );
#else
  debug_function("Report mailed to user", 0, fi->Scrn_y, __LINE__ );
#endif
  return 0;
}

int TakeNotes(FILE *FileToWrite, struct FileData *fi)  {
   WINDOW *DebugEntry, *DebugEntryFrame; /* Only need DebugEntryFrame here - will need to create a Form inside this window. */
   time_t *MyTime = (time_t *) malloc(sizeof(MyTime) * sizeof(time_t));
   int TimeWritten = 0;
   char *TimeString = (char *) malloc(sizeof(TimeString) * 32); /* Is this enough space? */
   char *StringToWrite = (char *) malloc(sizeof(StringToWrite) * 4096);
   char *NextStringToWrite = (char *) malloc(sizeof(NextStringToWrite) * 96);
   memset(StringToWrite, 0, sizeof(StringToWrite)); /* Clear out the string... */
   memset(NextStringToWrite, 0, sizeof(NextStringToWrite)); /* Clear out the string... */
/* This creates a window as large as we can if the screen is smaller than standard 80x24 */
   if(fi->Scrn_y < 20 || fi->Scrn_x < 80) {
     DebugEntryFrame = newwin(fi->Scrn_y, fi->Scrn_x, 0, 0 ); /* Box for entry window */
     DebugEntry = newwin(fi->Scrn_y - 2, fi->Scrn_x - 2, 1, 1);
     }
   else {
/* ...else we create a 80x24 screen */
     DebugEntryFrame = newwin( 24, 80, 0, 0 ); /* Box for entry window */
     DebugEntry = newwin( 22, 78, 1, 1 ); /* Entry window */
     }
   box(DebugEntryFrame, 0, 0);
   mvwaddstr(DebugEntryFrame, 0, 18, "Debug screen - full stop (.) to finish");
   wrefresh(DebugEntryFrame);
   echo();
/* loop this until blank string "\n" - though if I decide to not write an entry,
 * I don't want a timestamp written to file either.  I also don't want a separate
 * timestamp for every sentence I add to the file.
 */
/* Form the timestamp string... */
  time(MyTime); /* First, get the time in seconds */
  strcpy(TimeString, ctime(MyTime)); /* Then change to an ASCII format, and copy to a string for printing */
  sprintf(NextStringToWrite, "%s offset (approx) %ld", fi->FName, fi->FPosn);
  while( 0 == wgetstr(DebugEntry, StringToWrite)) { /* real simplistic... I should use NULL here, not 0 */
    if(debug) debug_function("Checking string", 0, fi->Scrn_y, fi->LineCount);
    if(StringToWrite[0] == '.') break;  /* breaks out of the while loop - or should do...
    * seems making this a compare against \n doesn't work
    * This ^^ should be a strcmp, not an if(val[0] =='.')
    */
    if(debug) debug_function("Didn't compare to .", 16, fi->Scrn_y, fi->LineCount);
    if(!TimeWritten) { /* Only do this once per bugwrite session - wonder if this could be declared static? */
      fputs(TimeString, FileToWrite);
      /* Right here, we want to print Filename and Fptr stat */
      fputs(NextStringToWrite, FileToWrite);
      fputc('\n', FileToWrite); /* Just to tidy up the printup */
      TimeWritten++;
    }
    fputs(StringToWrite, FileToWrite);
    fputc('\n', FileToWrite);
    memset(StringToWrite, 0, sizeof(StringToWrite)); /* Clear out the string... */
    }
  noecho();
  /* Now, be good little programmers and clean up after ourselves...
   * we don't need no steenkin' memory leaks here  */
  delwin(DebugEntry);
  delwin(DebugEntryFrame);
  free(TimeString);
  free(StringToWrite);
  return 0;
}

/*
int TakeNotes2(FILE *FileToWrite, struct FileData *fi)  {
  FIELD *NotePad = new_field(24,78,0,0,128,1);
  return 0;
}
*/
