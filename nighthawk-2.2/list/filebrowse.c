#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
/* #include <files.h> */ /* Where did this come from, anyway???  Must
 * have had DOS on my mind too much ... heh heh heh... */
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include "list.h"

/* The idea of this is a preliminary filebrowse facility, pretty much
 * like Vernon Buerg does it... only using ALL my own code...
 * Anyway, this should be a  description of what I need to do, 
 * step by step (well, relatively)
 * Version 0.01 - 23-5-1999
 *    First ideas for what to do - 
 *    A - read specified directory,
 *    B - generate a filelist,
 *    C - print a scrollable version,
 *    D - show a selection bar,
 *    E - interpret keyboard input (up, down, etc)
 *    F - select and show file
 *
 * A) Need following procedures:
 *   :opendir() - stream whose elements are directory entries
 *     - Got that!  provided in std library
 *   :readdir() - retrieves each entry as a struct dirent object
 *   :  d_name is member of the above struct
 * B) Do I sort this list, or generate in order of occurrence?
 * There is a function for sorting the directory listing
 * anyway, : stash list in doubly-linked list, format as:
 *    struct dirent_list {
 *        int filename_len;
 *        char *dirname[filename_len];
 *        queue *prev_entry;
 *        queue *next_entry;
 *        };
 *    Some of this should be used from (read stolen) the dequeue code
 * C) Once I have generated the list, I need to display it in an ncurses way
 * D) and find out what I want to do with it ... another switch/case loop?
 * 
 * ====================
 * Other Ideas for List
 * ++++++++++++++++++++
 * These include:  variable number of columns, external file commands,
 * colour selection, ftp, mouse control... the list goes on.  Currently,
 * LIST does not do networked drives.
 */

/* User routines... */

struct FileData *FileCommand(struct FileData *fi)  {
   fi->FPtr=GetItem((DIR *)"./");
   /* What can we provide here?
   0 for read file, other ints for other commands
   -1 for exit of program
   */
   return 0;
}

DIR *GetDirectory(DIR *TmpDir, char *DirectoryName)  {
/*   fi->FPtr= something I don't yet know - ; */
    TmpDir = opendir(DirectoryName);
    if(TmpDir != NULL) Bye(5, __LINE__);
/* Couldn't I get this to return the dir ptr instead of 0? */
    return 0;
}

int ListDirectory(DIR *TempDir)  {
    struct dirent **This_Directory;
    int n;
    n = scandir("./", &This_Directory, 0, alphasort);
    if(n > 0)  {
	 int cnt;
	 for(cnt = 0; cnt < n; ++cnt) 
		 printf("%s\n", This_Directory[cnt]->d_name);
	 }
    else
	 perror("Couldn't open the directory!");
    return 0;
}

/*
static int one (struct dirent *unused) {
	     return 1;
     }
*/

FILE *GetItem(DIR *ThisDir) {
  /* Nothing here currently ... */
  struct dirent *ThisDirEntryPtr;
  struct dirent ThisDirEntry;
  FILE *ThisFile;
  ThisDirEntryPtr = &ThisDirEntry;
  ThisDirEntryPtr = readdir(ThisDir);
  if(ThisDirEntryPtr == NULL)
    return NULL;
  else
    ThisFile = fopen(ThisDirEntryPtr->d_name, "r");
    if(ThisFile == NULL) return NULL;
    else return ThisFile;
}
