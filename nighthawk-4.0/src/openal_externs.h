/*vars*/
extern int sound_enabled_f;
extern ALuint fx_buffer_table[MAX_FX_BUFFER_ENTRIES];

/*funcs*/
extern void stop_music(void);
extern void play_music(void);
extern void put_snd_event(snd_event_st *e);
extern void sound_process_fg(void);
extern void shutdown_sound(void);
extern void init_sound(void);
