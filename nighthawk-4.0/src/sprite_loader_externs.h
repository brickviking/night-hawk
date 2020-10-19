/*
 * Variables
 */
extern tflr_bm     *flr_bm;
extern int         flr_bm_size;

extern bm_t        droid_bm[10],
            paradroid_bm[10],
            laser_l_bm,
            laser_cb_bm,
            laser_uv_bm,
            laser_te_bm,
            explosion_bm[7],
            power_bay_bm[5],
            ntitle_bm,
            doorh_bm[9],
            doorv_bm[9],
            digit_bm[10],
            ship_complete_bm,
            credit_6_bm,
            neg_bm,
            trans_terminated_bm,
            get_ready1_bm,
            intro_back_bm[4];
extern bm_t        font_bm[FONT_SIZE];
extern int         ship_bm_offs_x[];

/*
 * Functions
 */
extern void load_bm(char *fn, bm_t *bm, int aligned_f);
extern void load_bm_ani(char *fn, bm_t *bm, unsigned int len, int aligned_f);
extern void load_font(void);
extern void load_flr_xpms(void);
extern void load_sprite_xpms(void);
extern void load_fleet_logos(void);
