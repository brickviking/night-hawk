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
extern tbm     game_over1_bm;
extern tbm     credit_1_bm;
extern tbm     credit_2_bm;
extern tbm     credit_3_bm;
extern tbm     get_ready2_bm;
extern tbm     neg_bm;
extern tbm     paused_bm;

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
