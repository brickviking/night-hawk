extern tfunk_info  funk_info;
extern tfunk_hr    *funk_hr_ptr;
extern tslot       *funk_pat_ptr;
extern void        *funk_sam_ptrs[64];
extern tfunk_chan  funk_chan[MAXIMUM_CHANNELS];
extern void        (*CARD_freq_convert)(int,long);
extern void        (*CARD_volume_convert)(int,int,int,int);
extern int         c_channel;

extern int         do_retrig_sample(int sam_no);
extern void        do_full_note(int note,int volume);
extern void        funk_tracker(void);
