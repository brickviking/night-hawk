/* Now called list.h instead of hexdump.h */
/* Global defines */
#define LISTVERSION 0
#define LISTMINOR 29
#define LISTMAJOR 7
/* Had to include this one, since it's disappeared from 2.2 linux kernel or something... */
#ifndef PATH_MAX
#define PATH_MAX  4095
#endif
/* #include <sys/stat.h> / * 'cos it says so!!! Humpf! */
#include "dequeue.h"

/* User #defines */
/* This next is to account for the fact that minix doesn't
 * even HAVE color in its ncurses so we have to make do with old
 * curses instead - there isn't a has_color
 */
#ifdef _MINIX
int has_color(void);
void getmaxyx(WINDOW *, int, int);
typedef unsigned long u_long;
#endif

#define QUIT_CHAR 'q'
/* Global variables */
/* typedef unsigned long u_long; * not needed - declared in <sys/types.h> */

struct FileData {
	long FPosn; /* Current position within file */
	long FEnd; /* Total bytes */
	long FLines; /* Total number of CR-terminated lines */
	long *CrArray; /* Array to store line-endings in */
	long LnAtTopOfScrn; /* Will contain the line number at top of screen */
	long FLineCtr; /* Current line within file - last line looked at so far */
	long LineCount; /* Screen lines printed * added for debugging */
	long Start; /* Where to start */
	long Count; /* How many bytes to read */
        int Scrn_y, Scrn_x, ScrnWide;
	FILE *FPtr;  /* Stream pointing to file */
	char *FName; /* set this to point to the filename... */
	char DumpMode; /* Either 'x' or 't' */
	char DumpFlag; /* Either '7' or '8' or '*' */
        char LineNumbers; /* Set to on or off */
	char Monochrome;
	char TabSize; /* a number from 1 to _about_ 8 */
	int addrflag;  /* Either hex or decimal addresses (only in hex view) */
	int EofSet;    /* Has the End of the File been reached? */
	int FgColour, BgColour; /* Colours */
/* Search stuff... */	
	char SearchDirection; /* Forward or backward */
	char *SearchString;  /* Search String */
	long SPosn; /* Search Position - has to be long int */
	struct stat FileInfo; /* Do these two work? */
	struct stat *FileInfoPtr;
	int Quick; /* This was defined so it didn't take so long to display a file in hex mode */
/* Have we defined the queue object? i.e. included dequeue.h ?*/
        queue DirQueuePtr; /* From dequeue.h (finally!) */
	char *BugWriteFName; /* Filename of bugwrite file */
        int MyArgc;
        char **MyArgv;
	int MyCurrArgC;  /* This is added to utilise positioning within MyArgv[] */
};

int debug;
int ruler;

/* Function definitions */
/* - main file (list.c) */
long *AllocateLines(struct FileData *);
#ifndef _MINIX
__inline__ void BackPage(struct FileData *, long int);
#else
void BackPage(struct FileData *, long int);
#endif
int Bye(int, int);
void CartWheel(void);
void debug_function(char *, int, int, int);
int ShowStats(struct FileData *);
int Dump(struct FileData *);
int EditFunction(struct FileData *);
void ShowHelp(struct FileData *);
int NewFile(struct FileData *, char *);
void PrintLine(struct FileData *);
void ScanForCr(struct FileData *);
int Search(struct FileData *);
char *SearchPrompt(struct FileData *);
void Usage(void);
char WhatNext(struct FileData *);
int is_dir(char *);
char *unquote_file(char *);
char *save(char*);
void *ecalloc(int, unsigned int);
void CloseNCurses(void);
/* These three routines borrowed from srchfile.c from SNIPPETS 9707 */
long ffsearch(struct FileData *, int);
long rfsearch(struct FileData *, int);
char *SearchParse(const char *);
int CmdMode(struct FileData *);
/* - filedir routines */

/*  ?? int PointToFile(int, long); * /
struct FileData *FileCommand(struct FileData *); / * Returns the command to run on the file * /
DIR *GetDirectory(DIR *, char *); / * Feeds back the directory mentioned in char * */
FILE *GetItem(DIR *); /* Feeds back one item from a directory */
void PrintDirectory(void); /* Prints list on screen */

/* - bugwrite routines */
int BugWrite(struct FileData *);
int Choose(void);
int WriteToFile(void);
int EmailUser(struct FileData *);
int TakeNotes(FILE *, struct FileData *);

