#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include <ctype.h>
#include "defs.h"
#include "opengl_externs.h"

int		verbose_logging = TRUE;

/***************************************************************************
* Exit point for nighthawk where system resources that must be cleaned up by
* user space code must do it. Nb/ memory allocation is cleaned up by OS. No
* need to do a bunch of free()'s.
*
* Moving as much code into C files as C++ is yucky !. JN, 27AUG20
***************************************************************************/
void end_game(int status)
{
	if  (!status)
		printf("See ya\n");

	exit(status);
}

/***************************************************************************
* Improved error handling functions. JN 24AUG20
***************************************************************************/
void printf_error(const char *fmt, ...)
{
	va_list		ap;
	char		msg[STR_LEN_HUGE + 1];

	va_start(ap, fmt);
	vsnprintf(msg, sizeof(msg), fmt, ap);
	va_end(ap);

	printf("Error: %s.\n", msg);
	end_game(-1);
}

void print_error(const char *func, const char *cmd)
{
	printf("Error: %s:%s- %s\n", func, cmd, strerror(errno));
	end_game(-1);
}

/***************************************************************************
* Improved memory handing functions. JN, 24AUG20
***************************************************************************/
void *alloc_mem(int mem_size)
{
	void *r;

	r = malloc(mem_size);
	if (r == NULL)
		print_error(__func__, "malloc()");

	return r;
}

/*
 * Because C++ has turned into such as arse of an extension, I need this,
 * or rather, I can't be bothered learning the millionth revision of the C++.
 * I don't need C++ to write good object orientated code. If I could convert
 * this game to C, then I would. Rant over ! :(. JN, 06SEP20
 */
char *alloc_mem_char(int mem_size)
{
	return alloc_mem(mem_size);
}

void *realloc_mem(void *p, int mem_size)
{
	void *r;

	r = realloc(p, mem_size);
	if (r == NULL)
		print_error(__func__, "realloc()");

	return r;
}

void print_new_error(const char *func)
{
	print_error(__func__, "new <class>");
	end_game(-1);
}

/****************************************************************************
* String functions added, JN, 26AUG20
****************************************************************************/
void strncpy2(char *dst, char *src, int len)
{
	strncpy(dst, src, len);
	dst[len] = 0;
}

void strip_leading_whitespace(char *str)
{
	char *s, *se;

	s = se = str + strlen(str) - 1;
	if (s == str)
		return;
	for (; s != str; s--) {
		if(!isspace(*s)) {
			if (s != se)
				s[1] = 0;
			return;
		}
	}
}
