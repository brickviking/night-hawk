extern int	verbose_logging;

/*
 * Functions
 */
extern void end_game(int status);
extern void printf_error(const char *fmt, ...);
extern void print_error(const char *func, const char *cmd);
extern void *alloc_mem(int mem_size);
extern char *alloc_mem_char(int mem_size);
extern void *realloc_mem(void *p, int mem_size);
extern void print_new_error(const char *func);
extern void strncpy2(char *dst, char *src, int len);
extern void strip_leading_whitespace(char *str);
