extern tchmix chmix[MAXIMUM_CHANNELS];
extern int shift_table[];
extern int dsp_fp;

extern void init_dsp_buffers(void);
extern int open_dsp(int srate,int prec,int st);
extern void close_dsp(void);
extern void DSPx08_mono_mixxer(void);
extern void DSPx08_stereo_mixxer(void);
extern void DSPx16_mono_mixxer(void);
extern void DSPx16_stereo_mixxer(void);
extern void DSPi_freq_convert(int chan_no,long rfreq);
extern void DSPi_volume_convert(int chan_no,int rvolume,int balance);
