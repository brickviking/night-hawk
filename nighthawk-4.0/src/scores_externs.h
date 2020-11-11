/*
 * variable externs
 */
extern unsigned int score, sscore, last_score;
extern tscore_table score_table[MAX_SCORE_TABLE];
extern tscore_table *preserved_session;

/*
 * Function externs
 */
extern void init_scores(void);
extern void dump_scores(void);
extern void clr_score_table(void);
extern void load_scores(void);
extern void get_score_username(char *su);
extern void save_scores(int complete);
extern void preserve_session(void);
extern void load_preserved_session(void);
