/***************************************************************************
****************************************************************************
****************************************************************************
*
* FunktrackerGOLD - By Jason Nunn
* Copyright (C) 1996,1998 Jason Nunn
*
* FunktrackerGOLD now comes under the GNU General Public License. Please
* read the COPYING notice in this distribution.
*
* ================================================================
*
****************************************************************************
****************************************************************************
***************************************************************************/
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#if defined(Linux) || defined( FreeBSD) || defined(__DragonFly__)
 #include <sys/soundcard.h>
#elif defined(__NetBSD__ || __OpenBSD__)
 #include <soundcard.h>
#endif
#include "funktracker_defs.h"
#include "funktracker.h"
#include "funkload.h"

tchmix chmix[MAXIMUM_CHANNELS];

                   /*00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 16 */
int shift_table[] = {-1,-1,-1,-1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3};
int dsp_fp;

unsigned long mix_buffer_size;
sDD *left_mix_buffer = NULL;
sDD *right_mix_buffer = NULL;

#pragma pack(1)
uDB *final_mix_buffer08 = NULL;
uDW *final_mix_buffer16 = NULL;
uDD *final_mix_buffer32 = NULL;
#pragma pack()

#ifdef DIGITAL_ECHOING
#define echo_dly_factor  256 /* @ 22050hz*/
#define echo_buffer_size (512 * 16)
#define echo_mask        (echo_buffer_size - 1)
int echo_seg;
#endif

void DAC0816_channel_mixxer(int chan_no,sDD *lbuf,sDD *rbuf);
void DAC1616_channel_mixxer(int chan_no,sDD *lbuf,sDD *rbuf);
void (*channel_mixxer)(int,sDD *,sDD *) = DAC0816_channel_mixxer;

typedef struct
{
  int precision;
  int stereo;
  int fram_min;
  int frag_min;
  int fram_max;
  int frag_max;
} tcard_caps;

tcard_caps card_caps[] = {
  { 8,0,   8, 6,  16, 7},
  { 8,1,   8, 7,  18, 8},
  {16,0,   8, 8,  24, 9},
  {16,1,   8, 9,  32,10},
  { 0,0,   0, 0,   0, 0}
};

/**************************************************************************
*
**************************************************************************/
void calc_mix_buffer(int bpm)
{
  funk_info.hz_rate = 2 * (bpm / 5);
  mix_buffer_size = 
    (int)(((double)funk_info.sampling_rate / funk_info.hz_rate) + 0.5);
}

void init_dsp_buffers(void)
{
#ifdef DIGITAL_ECHOING
  register int chan;
#endif
  register int x;

  calc_mix_buffer(funk_info.bpm_rate);
  ioctl(dsp_fp,SNDCTL_DSP_RESET);
  for(x = 0;x < mix_buffer_size;x++)
  {
    *(left_mix_buffer + x) = 0;
    *(right_mix_buffer + x) = 0;

    if(funk_info.precision == 8)
      if(funk_info.stereo)
        *(final_mix_buffer16 + x) = 0;
      else
        *(final_mix_buffer08 + x) = 0;
    else
      if(funk_info.stereo)
        *(final_mix_buffer32 + x) = 0;
      else
        *(final_mix_buffer16 + x) = 0;
  }
#ifdef DIGITAL_ECHOING
  for(chan = 0;chan < MAXIMUM_CHANNELS;chan++)
    for(x = 0;x < echo_buffer_size;x++)
      *(chmix[chan].echo_buffer + x) = 0;
#endif
  if(funk_info.sample_precision == 8)
    channel_mixxer = DAC0816_channel_mixxer;
  else
    channel_mixxer = DAC1616_channel_mixxer;
}

void dsp_dealloc_bufs(void)
{
#ifdef DIGITAL_ECHOING
  register int x;
#endif

  if(left_mix_buffer != NULL) free(left_mix_buffer);
  if(right_mix_buffer != NULL) free(right_mix_buffer);
  if(final_mix_buffer08 != NULL) free(final_mix_buffer08);
  if(final_mix_buffer16 != NULL) free(final_mix_buffer16);
  if(final_mix_buffer32 != NULL) free(final_mix_buffer32);

#ifdef DIGITAL_ECHOING
  for(x = 0;x < MAXIMUM_CHANNELS;x++)
    if(chmix[x].echo_buffer != NULL)
      free(chmix[x].echo_buffer);
#endif
}

int dsp_alloc_bufs(void)
{
  register int s = 1;
#ifdef DIGITAL_ECHOING
  register int x;
#endif

  left_mix_buffer = malloc(mix_buffer_size * sizeof(long));
  right_mix_buffer = malloc(mix_buffer_size * sizeof(long));
  if(funk_info.precision == 8)
    if(funk_info.stereo)
      final_mix_buffer16 = malloc(mix_buffer_size * 2);
    else
      final_mix_buffer08 = malloc(mix_buffer_size * 1);
  else
    if(funk_info.stereo)
      final_mix_buffer32 = malloc(mix_buffer_size * 4);
    else
      final_mix_buffer16 = malloc(mix_buffer_size * 2);

#ifdef DIGITAL_ECHOING
  for(x = 0;x < MAXIMUM_CHANNELS;x++)
    chmix[x].echo_buffer = malloc(echo_buffer_size * 4);
#endif

  if(left_mix_buffer == NULL)
    s = 0;
  if(right_mix_buffer == NULL)
    s = 0;
  if((final_mix_buffer08 == NULL) &&
     (final_mix_buffer16 == NULL) &&
     (final_mix_buffer32 == NULL))
    s = 0;

#ifdef DIGITAL_ECHOING
  for(x = 0;x < MAXIMUM_CHANNELS;x++)
    if(chmix[x].echo_buffer == NULL)
      s = 0;
#endif
  return s;
}

/**************************************************************************
* Open up (blocking) /dev/dsp
**************************************************************************/
void close_dsp(void)
{
  if(dsp_fp != -1)
  {
    ioctl(dsp_fp,SNDCTL_DSP_RESET);
    close(dsp_fp);
  }
}

int open_dsp(int srate,int prec,int st)
{
  register int frames = 8,fragment = 6;

  funk_info.no_active_channels = 4;
  funk_info.sample_precision = 8;
  funk_info.sampling_rate = srate;
  funk_info.precision = prec;
  if(prec == 8)
    funk_info.funk_card_type = FKCARD_VOX08;
  if(prec == 16)
    funk_info.funk_card_type = FKCARD_VOX16;
  funk_info.stereo = st;
  if(funk_info.sampling_rate < 6000)
    funk_info.sampling_rate = 6000;
  if(funk_info.sampling_rate > 44100)
    funk_info.sampling_rate = 44100;
  calc_mix_buffer(MIN_BPM_RATE);

  if((frames != -1) || (fragment != -1))
  {
    register int p = 0;

    for(;;)
    {
      if(!card_caps[p].precision)
        break;
      if(card_caps[p].precision == funk_info.precision)
        if(card_caps[p].stereo == funk_info.stereo)
        {
          float sr;

          sr = ((float)funk_info.sampling_rate - 6000) / (44100 - 6000);
          frames = 
            (int)rint(sr * (float)(card_caps[p].fram_max -
            card_caps[p].fram_min)) + card_caps[p].fram_min;
          fragment = 
            (int)rint(sr * (float)(card_caps[p].frag_max -
            card_caps[p].frag_min)) + card_caps[p].frag_min;
          break;
        }
      p++;
    }
  }

#ifdef DIGITAL_ECHOING
  echo_seg = (echo_dly_factor * funk_info.sampling_rate) / 22050;
  printf("Digital echoing enabled..\n");
#endif

  printf("Mix length: %d (%dhz)\n",
    (int)mix_buffer_size,funk_info.sampling_rate);
  if(dsp_alloc_bufs())
  {
    printf("Allocated dsp buffers..\n");
    dsp_fp = open("/dev/dsp",O_WRONLY,0);
    if(dsp_fp != -1)
    {
      int frag;

      printf("Opened /dev/dsp: %d bit %s, %d Hz (%d frames @ %d bytes)\n",
        funk_info.precision,
        funk_info.stereo == 8 ? "mono" : "stereo",
        funk_info.sampling_rate,
        frames,1 << fragment);
      frag = (frames << 16) + fragment;
      ioctl(dsp_fp,SNDCTL_DSP_SAMPLESIZE,&(funk_info.precision));
      ioctl(dsp_fp,SNDCTL_DSP_STEREO,&(funk_info.stereo));
      ioctl(dsp_fp,SNDCTL_DSP_SPEED,&(funk_info.sampling_rate));
      ioctl(dsp_fp,SNDCTL_DSP_SETFRAGMENT,&frag);
      return 1;
    }
    else
      printf("Error: Couldn't open /dev/dsp. Program aborted.\n");
    dsp_dealloc_bufs();
  }
  else
    printf("Error: Allocating dsp buffers. Program aborted.\n");
  return 0;
}

/**************************************************************************
080808080808080808080808080808080808080808080808080808080808080808080808080
080808080808080808080808080808080808080808080808080808080808080808080808080
080808080808080808080808080808080808080808080808080808080808080808080808080
*
* 08 bit into 16bit unit channel DAC buffer Mixer
*
* 00
* ||_____ loop sample
* |______ DAC sample play
*
080808080808080808080808080808080808080808080808080808080808080808080808080
080808080808080808080808080808080808080808080808080808080808080808080808080
080808080808080808080808080808080808080808080808080808080808080808080808080
**************************************************************************/
void DAC0816_channel_mixxer(int chan_no,sDD *lbuf,sDD *rbuf)
{
  register unsigned long sample_no;
  register int sam;
#ifdef DIGITAL_ECHOING
  register long sam_reverb;
  register unsigned int echo_rpos;

  echo_rpos = chmix[chan_no].echo_delay * echo_seg;
#endif

  for(sample_no = 0;sample_no < mix_buffer_size;sample_no++)
  {
    if(chmix[chan_no].funkctrl & 0x2)
#pragma pack(1)
      sam = *((sDB *)chmix[chan_no].sample_addr +
            (unsigned long)chmix[chan_no].sample_ptr);
#pragma pack()
    else
      sam = 0;

#ifdef DIGITAL_ECHOING
    sam_reverb = *(chmix[chan_no].echo_buffer +
                 ((chmix[chan_no].echo_ptr - echo_rpos) & echo_mask));
    *(chmix[chan_no].echo_buffer +
     (chmix[chan_no].echo_ptr & echo_mask)) =
      sam + ((sam_reverb * chmix[chan_no].echo_feedback) >> 4);
    sam += (sam_reverb * chmix[chan_no].echo_decay) >> 4;
    chmix[chan_no].echo_ptr++;
#endif

    *(lbuf++) += (sam * chmix[chan_no].left_volume) >>
      shift_table[funk_info.no_active_channels];
    *(rbuf++) += (sam * chmix[chan_no].right_volume) >>
      shift_table[funk_info.no_active_channels];
    chmix[chan_no].sample_ptr += chmix[chan_no].freq;
    if(chmix[chan_no].sample_ptr >= chmix[chan_no].length)
    {
      if(chmix[chan_no].funkctrl & 0x1)
        chmix[chan_no].sample_ptr = chmix[chan_no].start;
      else
        chmix[chan_no].funkctrl = 0;
    }
  }
}

/**************************************************************************
161616161616161616161616161616161616161616161616161616161616161616161616161
161616161616161616161616161616161616161616161616161616161616161616161616161
161616161616161616161616161616161616161616161616161616161616161616161616161
*
* 16 bit into 16bit unit channel DAC buffer Mixer
*
* 00
* ||_____ loop sample
* |______ DAC sample play
*
161616161616161616161616161616161616161616161616161616161616161616161616161
161616161616161616161616161616161616161616161616161616161616161616161616161
161616161616161616161616161616161616161616161616161616161616161616161616161
**************************************************************************/
void DAC1616_channel_mixxer(int chan_no,sDD *lbuf,sDD *rbuf)
{
  register unsigned long sample_no;
  register int sam,shift_t;
#ifdef DIGITAL_ECHOING
  register long sam_reverb;
  register unsigned int echo_rpos;

  echo_rpos = chmix[chan_no].echo_delay * echo_seg;
#endif

  shift_t = 8 + shift_table[funk_info.no_active_channels];
  for(sample_no = 0;sample_no < mix_buffer_size;sample_no++)
  {
    if(chmix[chan_no].funkctrl & 0x2)
#pragma pack(1)
      sam = *((sDW *)chmix[chan_no].sample_addr +
            (unsigned long)chmix[chan_no].sample_ptr);
#pragma pack()
    else
      sam = 0;
#ifdef DIGITAL_ECHOING
    sam_reverb = *(chmix[chan_no].echo_buffer +
                 ((chmix[chan_no].echo_ptr - echo_rpos) & echo_mask));
    *(chmix[chan_no].echo_buffer +
     (chmix[chan_no].echo_ptr & echo_mask)) =
      sam + ((sam_reverb * chmix[chan_no].echo_feedback) >> 4);
    sam += (sam_reverb * chmix[chan_no].echo_decay) >> 4;
    chmix[chan_no].echo_ptr++;
#endif
    *(lbuf++) += (sam * chmix[chan_no].left_volume) >> shift_t;
    *(rbuf++) += (sam * chmix[chan_no].right_volume) >> shift_t;
    chmix[chan_no].sample_ptr += chmix[chan_no].freq;
    if(chmix[chan_no].sample_ptr >= chmix[chan_no].length)
    {
      if(chmix[chan_no].funkctrl & 0x1)
        chmix[chan_no].sample_ptr = chmix[chan_no].start;
      else
        chmix[chan_no].funkctrl = 0;
    }
  }
}

/***************************************************************************
* post master volume setting
***************************************************************************/
void master_volumed(void)
{
  register int no,v;

  v = funk_info.master_volume + 1;
  for(no = 0;no < mix_buffer_size;no++)
  {
    *(left_mix_buffer + no) = (*(left_mix_buffer + no) * v) >> 8;
    *(right_mix_buffer + no) = (*(right_mix_buffer + no) * v) >> 8;
  }
}

/***************************************************************************
* mixxer control (x bit samples mixxed into 8 bit mono output..god help us)
***************************************************************************/
void DSPx08_mono_mixxer(void)
{
  register int no;

  funk_tracker();
  for(no = 0;no < funk_info.no_active_channels;no++)
    channel_mixxer(no,left_mix_buffer,right_mix_buffer);
  master_volumed();
  for(no = 0;no < mix_buffer_size;no++)
  {
    *(final_mix_buffer08 + no) =
      (uDB)(((*(left_mix_buffer + no) + *(right_mix_buffer + no)) ^ 0x10000)
      >> 9);
    *(left_mix_buffer + no) = 0;
    *(right_mix_buffer + no) = 0;
  }
  write(dsp_fp,final_mix_buffer08,mix_buffer_size);
}

/***************************************************************************
* mixxer control (x bit samples mixxed into 16 bit mono output)
***************************************************************************/
void DSPx16_mono_mixxer(void)
{
  register int no;

  funk_tracker();
  for(no = 0;no < funk_info.no_active_channels;no++)
    channel_mixxer(no,left_mix_buffer,right_mix_buffer);
  master_volumed();
  for(no = 0;no < mix_buffer_size;no++)
  {
    *(final_mix_buffer16 + no) =
      (*(left_mix_buffer + no) + *(right_mix_buffer + no)) >> 1;
    *(left_mix_buffer + no) = 0;
    *(right_mix_buffer + no) = 0;
  }
  write(dsp_fp,final_mix_buffer16,mix_buffer_size * 2);
}

/***************************************************************************
* mixxer control (x bit samples mixxed into 8 bit stereo output)
***************************************************************************/
void DSPx08_stereo_mixxer(void)
{
  register int no;

  funk_tracker();
  for(no = 0;no < funk_info.no_active_channels;no++)
    channel_mixxer(no,left_mix_buffer,right_mix_buffer);
  master_volumed();
  for(no = 0;no < mix_buffer_size;no++)
  {
    *(final_mix_buffer16 + no) =
       ((*(left_mix_buffer + no) ^ 0x8000) & 0xff00) +
       (((*(right_mix_buffer + no) >> 8) ^ 0x80) & 0x00ff);
    *(left_mix_buffer + no) = 0;
    *(right_mix_buffer + no) = 0;
  }
  write(dsp_fp,final_mix_buffer16,mix_buffer_size * 2);
}

/***************************************************************************
* mixxer control (x bit samples mixxed into 16 bit stereo output)
***************************************************************************/
void DSPx16_stereo_mixxer(void)
{
  register int no;

  funk_tracker();
  for(no = 0;no < funk_info.no_active_channels;no++)
    channel_mixxer(no,left_mix_buffer,right_mix_buffer);
  master_volumed();
  for(no = 0;no < mix_buffer_size;no++)
  {
    *(final_mix_buffer32 + no) =
      (*(left_mix_buffer + no) << 16) + (*(right_mix_buffer + no) & 0xffff);
    *(left_mix_buffer + no) = 0;
    *(right_mix_buffer + no) = 0;
  }
  write(dsp_fp,final_mix_buffer32,mix_buffer_size * 4);
}

/**************************************************************************
***************************************************************************
* This routines are hardware dependant routines. that translate real freq.
* and volume values into the specific card scalarity.
***************************************************************************
**************************************************************************/
void DSPi_freq_convert(int chan_no,long rfreq)
{
  chmix[chan_no].freq = (double)rfreq / funk_info.sampling_rate;
}

void DSPi_volume_convert(int chan_no,int rvolume,int balance)
{
  chmix[chan_no].right_volume = (rvolume * balance) >> 8;
  chmix[chan_no].left_volume = (rvolume * (~balance & 0xff)) >> 8;
}
