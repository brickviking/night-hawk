extern int ferr_val;
extern char *ferr_messages[];
extern void (*virtualmixxer)(void);

extern void clear_slot(tslot *slot);
extern void clear_sam_entry(tfunk_sb *funk_sb);
extern void set_BPM(void);
extern int alloc_funk_hr(void);
extern int alloc_pat_blk(void);
extern void dealloc_funk_mem(void);
extern void varedit_init(void);
extern long find_file_size(FILE *fp);
extern void find_pats_seqs(void);
extern void load_funk_module(char *filename);
extern void funk_init_for_play(void);
extern void dsp_init_for_play(void);
