extern FILE    *sound_engine_fp;

extern char    *host;
extern Display *display;
extern Window  window,root,parent;
extern int     depth,screen;
extern GC      gc_line,gc_dline,gc_bgblt,gc_bltblt;

extern Pixmap  render_screen;

extern int     window_width;
extern int     window_height;
extern int     window_x_pos;
extern int     window_y_pos;

extern int     score,sscore;

extern tkevent kevent[KEVENT_SIZE];
extern int     kevent_r_ptr,kevent_w_ptr;
extern int     demo_count,kevent_buf_ptr,kevent_buf_size;

extern tsprite *flr_sprites;
extern int     sprites_size;
extern tbm     droid_bm;
extern tbm     paradroid_bm;
extern tbm     laser_l_bm;
extern tbm     laser_cb_bm;
extern tbm     laser_uv_bm;
extern tbm     laser_te_bm;
extern tbm     explosion_bm;
extern tbm     power_bay_bm;
extern tbm     ntitle_bm;
extern tbm     doorh_bm;
extern tbm     doorv_bm;
extern tbm     digit_bm;
extern tbm     digit2_bm;
extern tbm     ship_complete_bm;
extern tbm     credit_1_bm;
extern tbm     credit_2_bm;
extern tbm     credit_3_bm;
extern tbm     credit_4_bm;
extern tbm     credit_5_bm;
extern tbm     credit_6_bm;
extern tbm     neg_bm;
extern tbm     paused_bm;
extern tbm     trans_terminated_bm;
extern tbm     get_ready1_bm;
extern tbm     intro_back_bm[4];
extern tbm     demo_mode_bm;

extern unsigned long black_pixel;
extern unsigned long white_pixel[8];
extern unsigned long blue_pixel[8];
extern unsigned long yellow_pixel;
extern unsigned long green_pixel[MAX_SCORE_TABLE];
extern unsigned long red_pixel[MAX_SCORE_TABLE];

extern tdroid_stats droid_stats[];

extern XRectangle rech_pos1[4];
extern XRectangle rech_tmp[4];
extern XRectangle rech_pos2[4];

extern void sound_engine_cmd(char cmd,int sam,int vol,int bal);
extern int x_init(void);
extern void init_colours(void);
extern int loadxpm(char *filename,tbm *bm);
extern int loadxpm_xpm(char *filename,tbm *bm);
extern void drawxpm_ani(tbm *bm,int x,int y,int i,int total);
extern void ramp_colour(
  unsigned short r1,unsigned short g1,unsigned short b1,
  unsigned short r2,unsigned short g2,unsigned short b2,
  unsigned long *base,int run);
extern int load_flr_xpms(void);
extern void free_flr_xpms(void);
extern void free_bm(tbm *bm);
extern void free_sprite_xpms(void);
extern int load_sprite_xpms(void);
extern void Xreaper(void);
extern void free_demo_buf(void);
#ifdef DEVELOPMENT_MODE
extern void save_demo_buf(void);
#endif
extern void queue_kb_events(void);
extern tkevent *get_kb_event(void);
extern int load_demo(void);
extern void playback_kb_events(void);
extern void display_message(char *bp,int y);

