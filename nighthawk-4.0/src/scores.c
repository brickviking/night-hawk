/***************************************************************************
****************************************************************************
****************************************************************************
*
* Nighthawk Copyright (C) 1997 Jason Nunn
* Nighthawk is under the GNU General Public License.
*
* Sourceforge admin: Eric Gillespie
* night-hawk.sourceforge.net
*
* See README file for a list of contributors.
*
* ----------------------------------------------------------------
* score.c - Scoring code
*
* 27OCT20: Made the warnings in file handling code only print when verbose
* logging set (as recommended by alpha tester - DM on austech forum).
*
****************************************************************************
****************************************************************************
***************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <pwd.h>
#include <sys/types.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include "defs.h"
#include "linked_data_externs.h"
#include "misc_externs.h"

unsigned int	score, sscore, last_score;
tscore_table	score_table[MAX_SCORE_TABLE];
tscore_table	*preserved_session;

/***************************************************************************
* Init score file. Put in it's own function.
***************************************************************************/
void init_scores(void)
{
	strncpy2(score_table[0].name, "Wolfgang", SCORES_NAME_LEN);
	strncpy2(score_table[0].highest_ship, "Tobruk", SCORES_HSHIP_LEN);
	strncpy2(score_table[0].time, "-", SCORES_TIME_LEN);
	score_table[0].score = 10000;

	strncpy2(score_table[1].name, "Rafael", SCORES_NAME_LEN);
	strncpy2(score_table[1].highest_ship, "Zaxon", SCORES_HSHIP_LEN);
	strncpy2(score_table[1].time, "-", SCORES_TIME_LEN);
	score_table[1].score = 5000;

	strncpy2(score_table[2].name, "Eero", SCORES_NAME_LEN);
	strncpy2(score_table[2].highest_ship, "Discovery", SCORES_HSHIP_LEN);
	strncpy2(score_table[2].time, "-", SCORES_TIME_LEN);
	score_table[2].score = 2500;

	strncpy2(score_table[3].name, "Eric", SCORES_NAME_LEN);
	strncpy2(score_table[3].highest_ship, "Friendship", SCORES_HSHIP_LEN);
	strncpy2(score_table[3].time, "-", SCORES_TIME_LEN);
	score_table[3].score = 1500;

	strncpy2(score_table[4].name, "Nelson", SCORES_NAME_LEN);
	strncpy2(score_table[4].highest_ship, "Mearkat", SCORES_HSHIP_LEN);
	strncpy2(score_table[4].time, "-", SCORES_TIME_LEN);
	score_table[4].score = 1000;

	strncpy2(score_table[5].name, "Vincent", SCORES_NAME_LEN);
	strncpy2(score_table[5].highest_ship, "Ophukus", SCORES_HSHIP_LEN);
	strncpy2(score_table[5].time, "-", SCORES_TIME_LEN);
	score_table[5].score = 800;

	strncpy2(score_table[6].name, "Adrian", SCORES_NAME_LEN);
	strncpy2(score_table[6].highest_ship, "Esperence", SCORES_HSHIP_LEN);
	strncpy2(score_table[6].time, "-", SCORES_TIME_LEN);
	score_table[6].score = 500;

	strncpy2(score_table[7].name, "Tanel", SCORES_NAME_LEN);
	strncpy2(score_table[7].highest_ship, "Anoyle", SCORES_HSHIP_LEN);
	strncpy2(score_table[7].time, "-", SCORES_TIME_LEN);
	score_table[7].score = 300;

	strncpy2(score_table[8].name, "Jason", SCORES_NAME_LEN);
	strncpy2(score_table[8].highest_ship, "Seafarer", SCORES_HSHIP_LEN);
	strncpy2(score_table[8].time, "-", SCORES_TIME_LEN);
	score_table[8].score = 100;

	for (int i = 0; i < MAX_SCORE_TABLE; i++)
		strncpy2(score_table[i].fleet, "Nighthawk", SCORES_FLEET_LEN);
}

/***************************************************************************
* Dump scores (for debugging purposes). JN, 04SEP20
***************************************************************************/
void dump_scores(void)
{
	printf("%-8s %-20s %-20s %-12s %8s\n",
		"Name", "Fleet", "Highest Ship", "Time", "Score");
	for (int i = 0; i < MAX_SCORE_TABLE; i++) {
		printf("%-8s %-20s %-20s %-12s %8u\n",
			score_table[i].name,
			score_table[i].fleet,
			score_table[i].highest_ship,
			score_table[i].time,
			score_table[i].score);
	}
}

/***************************************************************************
* I think this code was added by Wolfgang Scherer. He only half did
* the job with the file locking. JN, 25AUG20
***************************************************************************/
static FILE *lopen(const char *filename, const char *mode)
{
	FILE *fp;

	fp = fopen(filename, mode);
	if (fp == NULL)
		return NULL;

	/*
	 * This places an exclusive lock in scores file. flock() will
	 * block if file is locked by another process and will not
	 * resume until that lock is removed (or that locking process
	 * holding the lock gets killed. JN, 25AUG20
	 */
	if (flock(fp->_fileno, LOCK_EX) == -1)
		print_error(__func__, "flock(LOCK_EX)");

	return fp;
}

static void lclose(FILE *fp)
{
	/*
	 * Remove lock
	 */
	if (flock(fp->_fileno, LOCK_UN) == -1)
		print_error(__func__, "flock(LOCK_UN)");

	fclose(fp);
}

/***************************************************************************
*
***************************************************************************/
void load_scores(void)
{
	const char *err_msg = "Warning: Could not load scores file ";
	FILE *fp;

	if (verbose_logging == TRUE)
		printf("Loading scores file '%s'.\n", scores_path);

	fp = lopen(scores_path, "r");
	if (fp == NULL) {
		if (verbose_logging == TRUE) /*JN, 27OCT20*/
			printf("%s '%s'.\n", (char *)err_msg, scores_path);
		return;
	}

	if (fread(score_table, sizeof(tscore_table), MAX_SCORE_TABLE, fp)
							!= MAX_SCORE_TABLE) {
		if (verbose_logging == TRUE) /*JN, 27OCT20*/
			printf("%s '%s'.\n", (char *)err_msg, scores_path);
	}
	lclose(fp);
}

/***************************************************************************
* Put these into dedicated functions. JN, 07SEP20
***************************************************************************/
void get_score_username(char *su)
{
	struct passwd *ps;

	ps = getpwuid(getuid());
	if (ps == NULL) {
		strncpy2(su, "unknown", SCORES_NAME_LEN);
		return;
	}
	strncpy2(su, ps->pw_name, SCORES_NAME_LEN);
}

static void get_score_time(char *st)
{
	time_t t;
	char *c;

	t = time(NULL);
	c = ctime(&t);
	memcpy(st, c + 8, 2);
	memcpy(st + 2, "/", 1);
	memcpy(st + 3, c + 4, 3);
	memcpy(st + 6, "/", 1);
	memcpy(st + 7, c + 20, 4);
	st[11] = 0; /*JN, 14SEP20*/
}

/***************************************************************************
* Bit of work to save_score()  (used to be called update_score()).
* JN, 26AUG20
***************************************************************************/
void save_scores(int complete)
{
	const char *err_msg = "Warning: Could not save scores file ";
	FILE *fp;
	int x;

	if (god_mode == TRUE) { /* Them's the breaks for fkn cheats :D, JN, 28AUG20*/
		printf("In GOD mode. Score not saved.\n");
		return;
	}

	load_scores();

	if (verbose_logging == TRUE)
		printf("Saving scores file '%s'.\n", scores_path);
	fp = lopen(scores_path, "w");
	if (fp == NULL) {
		if (verbose_logging == TRUE) /*JN, 27OCT20*/
			printf("%s '%s'.\n", (char *)err_msg, scores_path);
		return;
	}

	for (x = 0; x < MAX_SCORE_TABLE; x++) {
		int i;

		if (score <= score_table[x].score)
			continue;

		for (i = MAX_SCORE_TABLE - 1; i > x; i--)
			memcpy(score_table + i, score_table + i - 1,
						sizeof(tscore_table));

		get_score_username(score_table[x].name); /* JN, 07SEP20*/
		strncpy2(score_table[x].fleet, fleet_ptr->name, SCORES_FLEET_LEN); /*JN, 02OCT20*/

		if (ship_start_cmdline_f == TRUE) { /* Didn't play from beginning*/
			snprintf(score_table[x].highest_ship, SCORES_HSHIP_LEN,
				"*%s", ship_table[ship_ptr]);
		} else { /*Normal play. No cheating with -c cmd line param*/
			if (complete == TRUE) {
				strncpy2(score_table[x].highest_ship,
					"Complete!", SCORES_HSHIP_LEN);
			} else {
				strncpy2(score_table[x].highest_ship,
					(char *)ship_table[ship_ptr],
					SCORES_HSHIP_LEN);
			}
		}

		get_score_time(score_table[x].time); /* JN, 07SEP20*/
		score_table[x].score = score;
		break;
	}

	if (fwrite(score_table, sizeof(tscore_table), MAX_SCORE_TABLE, fp)
							 != MAX_SCORE_TABLE) {
		if (verbose_logging == TRUE) /*JN, 27OCT20*/
			printf("%s '%s'.\n", (char *)err_msg, scores_path);
	}
	lclose(fp);
}

/***************************************************************************
* New feature v4.0. These functions are for game continuance. JN, 07SEP20
***************************************************************************/
void preserve_session(void)
{
	const char *err_msg = "Warning: Could not save preserve file";
	FILE *fp;
	tscore_table score_preserve;

	if (preserve_path == NULL)
		return;

	/*
	 * Score not preserved if user hasn't started from beginning.
	 */
	if (ship_start_cmdline_f == TRUE) {
		printf("Warning: You did not play from beginning. "
				"Your score isn't preserved.\n");
		return;
	}

	if (verbose_logging == TRUE)
		printf("Preserving last ship completed '%s'.\n", preserve_path);

	fp = lopen(preserve_path, "w");
	if (fp == NULL) {
		if (verbose_logging == TRUE) /*JN, 27OCT20*/
			printf("%s '%s'.\n", (char *)err_msg, preserve_path);
		return;
	}

	get_score_username(score_preserve.name);
	strncpy2(score_preserve.fleet, fleet_ptr->name, SCORES_FLEET_LEN); /*JN, 02OCT20*/
	strncpy2(score_preserve.highest_ship,
			(char *)ship_table[ship_ptr], SCORES_HSHIP_LEN);
	get_score_time(score_preserve.time);
	score_preserve.score = score;

	if (fwrite(&score_preserve, sizeof(tscore_table), 1, fp) != 1) {
		if (verbose_logging == TRUE) /*JN, 27OCT20*/
			printf("%s '%s'.\n", (char *)err_msg, preserve_path);
	}
	lclose(fp);
}

/***************************************************************************
* New feature v4.0. These functions are for game continuance. JN, 07SEP20
***************************************************************************/
void load_preserved_session(void)
{
	const char *err_msg = "Warning: Could not restore preserved session";
	FILE *fp;

	if (preserve_path == NULL)
		return;
	if (strlen(ship_start))
		return;

	if (verbose_logging == TRUE)
		printf("Loading last session '%s'.\n", preserve_path);

	fp = lopen(preserve_path, "r");
	if (fp == NULL) {
		if (verbose_logging == TRUE) /*JN, 27OCT20*/
			printf("%s '%s'.\n", (char *)err_msg, preserve_path);
		return;
	}

	preserved_session = alloc_mem(sizeof(tscore_table));
	if (fread(preserved_session, sizeof(tscore_table), 1, fp) != 1) {
		if (verbose_logging == TRUE) /*JN, 27OCT20*/
			printf("%s '%s'.\n", (char *)err_msg, preserve_path);
		lclose(fp);
		return;
	}
	lclose(fp);

	if (verbose_logging == FALSE)
		return;
	printf("Preserved session:\n"
		"\tName: %s\n"
		"\tFleet: %s\n"
		"\tShip completed: %s\n"
		"\tScore: %u\n"
		"\tDate: %s\n",
		preserved_session->name,
		preserved_session->fleet,
		preserved_session->highest_ship,
		preserved_session->score,
		preserved_session->time);
}
