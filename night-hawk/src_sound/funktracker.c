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
* playback routines
*
****************************************************************************
****************************************************************************
***************************************************************************/
#include "funktracker_defs.h"
#include "dsp_mixxer.h"

/*funk ptrs*/
tfunk_info  funk_info;
tfunk_hr    *funk_hr_ptr;                   /*header pointer*/
tslot       *funk_pat_ptr;
void        *funk_sam_ptrs[64];                 /*sample pointers ptr*/
tfunk_chan  funk_chan[MAXIMUM_CHANNELS];
void        (*CARD_freq_convert)(int,long) = DSPi_freq_convert;
void        (*CARD_volume_convert)(int,int,int) = DSPi_volume_convert;

/*******************************************
* private vars
*******************************************/
int note_table[64] =
{
13696,12928,12192,11520,10848,10240, 9664, 9120, 8608, 8128, 7680, 7248,
 6848, 6464, 6096, 5760, 5424, 5120, 4832, 4560, 4304, 4064, 3840, 3624,
 3424, 3232, 3048, 2880, 2712, 2560, 2416, 2280, 2152, 2032, 1920, 1808,
 1712, 1616, 1520, 1440, 1360, 1280, 1208, 1144, 1080, 1016,  960,  904,
  856,  808,  760,  720,  680,  640,  600,  568,  536,  504,  480,  448,
    0,    0,    0,    0
};

int com_volume_inc[16] =
{
    1,  2,  3,  4,  5,  6,  7,  8,
   10, 12, 16, 20, 24, 32, 48, 64
};

int com_sine_table[64] =
{
   -6, -19, -31, -43, -55, -66, -76, -86,
  -95,-103,-110,-116,-120,-124,-127,-128,
 -128,-127,-124,-121,-116,-110,-103, -95,
  -86, -77, -66, -55, -44, -32, -19,  -7,
    6,  18,  30,  42,  54,  65,  76,  85,
   94, 102, 109, 115, 120, 124, 126, 127,
  127, 126, 124, 121, 116, 110, 104,  96,
   87,  77,  67,  56,  44,  32,  20,   8
};

int com_triangle_table[64] =
{
   -4, -12, -20, -28, -36, -44, -52, -60,
  -68, -76, -84, -92,-100,-108,-116,-124,
 -124,-116,-108,-100, -92, -84, -76, -68,
  -60, -52, -44, -36, -28, -20, -12,  -4,
    4,  12,  20,  28,  36,  44,  52,  60,
   68,  76,  84,  92, 100, 108, 116, 124,
  123, 115, 107,  99,  91,  83,  75,  67,
   59,  51,  43,  35,  27,  19,  11,   3
};

int com_square_table[64] =
{
  -64,-128,-128,-127,-127,-128,-128,-127,
 -127,-128,-128,-127,-127,-128,-128,-127,
 -127,-128,-128,-127,-127,-128,-128,-127,
 -127,-128,-128,-127,-127,-128,-128, -64,
   95, 127, 127, 126, 126, 127, 127, 126,
  126, 127, 127, 126, 126, 127, 127, 126,
  126, 127, 127, 126, 126, 127, 127, 126,
  126, 127, 127, 126, 126, 127, 127,  95
};

int com_sawtooth_table[64] =
{
   -1,  -5,  -9, -13, -17, -21, -25, -29,
  -33, -37, -41, -45, -49, -53, -57, -61,
  -65, -69, -73, -77, -81, -85, -89, -93,
  -97,-101,-105,-109,-113,-117,-121,-125,
  127, 123, 119, 115, 111, 107, 103,  99,
   95,  91,  87,  83,  79,  75,  71,  67,
   63,  59,  55,  51,  47,  43,  39,  35,
   31,  27,  23,  19,  15,  11,   7,   3
};

int com_random_table[64] =
{
  168, 167, 167, 165, 201, 201, 202, 201,
  244, 244, 244, 244,  85,  82,  85,  85,
   44,  44,  44,  44, 114, 114, 114, 114,
   77,  77,  73,  77, 116, 116, 116, 116,
   87,  87,  87,  87,  31,  31,  31,  31,
  198, 198, 198, 198, 105, 104, 105, 105,
  103, 105, 103, 103, 166, 166, 165, 166,
  171, 172, 171, 171, 217, 217, 217, 217
};

int   c_channel;
tslot *pattern_ptr;  /*ptr to extract data from pattern*/

/**************************************************************************
***************************************************************************
***************************************************************************
***************************************************************************
***************************************************************************
*
* Tracker Code
*
***************************************************************************
***************************************************************************
***************************************************************************
***************************************************************************
**************************************************************************/

/**************************************************************************
*
**************************************************************************/

/***********************************
*
* ebx = ifreq, returns = eax
*
***********************************/
long ifreq_to_rfreq(long ifreq)
{
  if(ifreq > 0)
    return (0x1b4f4d0 / ifreq);
  else
    return 0;
}

long ifreq_to_rfreq_vib(long ifreq)
{
  ifreq += funk_chan[c_channel].ifreq_vibrato;
  if(ifreq <= 0) ifreq = 1;
  return ifreq_to_rfreq(ifreq);
}

/***********************************
*
***********************************/
void volume_convert(void)
{
  if(funk_chan[c_channel].channel_kill == STOP)
    CARD_volume_convert(c_channel,0,0);
  else
    CARD_volume_convert(
      c_channel,
      funk_chan[c_channel].rvolume,
      funk_chan[c_channel].balance);
}

void vol_to_realvol(void)
{
  funk_chan[c_channel].rvolume = funk_chan[c_channel].volume;
}

void vol_to_realvol_vib(void)
{
  funk_chan[c_channel].rvolume =
    funk_chan[c_channel].volume + funk_chan[c_channel].volume_vibrato;
  if(funk_chan[c_channel].rvolume < 0) funk_chan[c_channel].rvolume = 0;
  if(funk_chan[c_channel].rvolume > 255) funk_chan[c_channel].rvolume = 255;
}

/***********************************
* bl = note, returns ebx = ifreq
***********************************/
#define note_2_ifreq(note) note_table[note]

/***********************************
*
* misc. vibrato function used by
* frequency and volume functions
*
* al = waveform
* ah = amplitude
* cl = speed
* ebx = ptr to vibrato table
* returns al = value, ebx = ptr
*
***********************************/
int com_vib_func(int waveform,
                 int amplitude,
                 int speed,
                 int *vib_table_ptr)
{
  register int tmp;

  amplitude++;
  switch(waveform)
  {
    case 1:        /*triangle*/
      tmp = com_triangle_table[*vib_table_ptr];
      break;
    case 2:        /*square*/
      tmp = com_square_table[*vib_table_ptr];
      break;
    case 3:        /*sawtooth*/
      tmp = com_sawtooth_table[*vib_table_ptr];
      break;
    case 4:        /*random*/
      tmp = com_random_table[*vib_table_ptr];
      break;
    default:       /*sine*/
      tmp = com_sine_table[*vib_table_ptr];
      break;
  }
  *vib_table_ptr += (~speed & 0xf) + 1;
  *vib_table_ptr &= 0x3f;
  return (tmp * amplitude) >> 4;
}

/***********************************
*
* Decode values form slot patterns
*
***********************************/
int decode_sample_no(void)
{
  funk_chan[c_channel].sample = 
    ((pattern_ptr->not_sam & 0x3) << 4) + (pattern_ptr->sam_com >> 4);
  return funk_chan[c_channel].sample;
}

int decode_note_no(void)
{
  return pattern_ptr->not_sam >> 2;
}

/***********************************
*
* for NOTE 3D: reload samples attrs
*
* returns sample volume
*
***********************************/
int reload_sample2(int sam_no)
{
  register int tmp1;

  funk_chan[c_channel].balance = funk_hr_ptr->funk_sb[sam_no].balance;

  tmp1 = funk_hr_ptr->funk_sb[sam_no].pt_and_sop;
  funk_chan[c_channel].port_type = tmp1 >> 4;
  funk_chan[c_channel].sample_ofs_parm = tmp1 & 0xf;

  tmp1 = funk_hr_ptr->funk_sb[sam_no].vv_waveform;
  funk_chan[c_channel].vib_waveform = tmp1 >> 4;
  funk_chan[c_channel].vol_vib_waveform = tmp1 & 0xf;

  tmp1 = funk_hr_ptr->funk_sb[sam_no].rl_and_as;
  funk_chan[c_channel].retrig_limit = tmp1 >> 4;
  funk_chan[c_channel].arp_speed = tmp1 & 0xf;

  return funk_hr_ptr->funk_sb[sam_no].volume;
}

int reload_sample_attr(void)
{
  return reload_sample2(decode_sample_no());
}

/***********************************
*
* returns sample volume
*
***********************************/
int do_retrig_sample(int sam_no)
{
  if(funk_hr_ptr->funk_sb[sam_no].length > 0)
  {
    if(funk_hr_ptr->funk_sb[sam_no].start == 0xffffffff)
      chmix[c_channel].funkctrl = 0x2;    /*set funkctrl*/
    else
      chmix[c_channel].funkctrl = 0x3;
    chmix[c_channel].sample_addr = funk_sam_ptrs[sam_no];
    chmix[c_channel].start = funk_hr_ptr->funk_sb[sam_no].start;

    if(chmix[c_channel].start > funk_hr_ptr->funk_sb[sam_no].length)
      chmix[c_channel].start = funk_hr_ptr->funk_sb[sam_no].length;

    chmix[c_channel].length = funk_hr_ptr->funk_sb[sam_no].length;
    chmix[c_channel].sample_ptr = 0;
  }
  return reload_sample2(sam_no);
}

int decode_sample(void)
{
  return do_retrig_sample(decode_sample_no());
}

/***********************************
*
***********************************/
void do_full_note(int note,int volume)
{
  funk_chan[c_channel].note = note;
  funk_chan[c_channel].volume = volume;
  funk_chan[c_channel].ifreq = note_2_ifreq(note);
  funk_chan[c_channel].rfreq = ifreq_to_rfreq(funk_chan[c_channel].ifreq);
  vol_to_realvol();
  CARD_freq_convert(c_channel,funk_chan[c_channel].rfreq);
  volume_convert();
}

void load_attr_active(int volume)
{
  funk_chan[c_channel].volume = volume;
  vol_to_realvol();
  volume_convert();
}

/***********************************
*
***********************************/
void normal_decode_system(void)
{
  register int note,volume;

  note = decode_note_no();
  if(note == 0x3d)                /*if ReLOad samples then ...*/
    load_attr_active(reload_sample_attr());
  else
  {
    volume = decode_sample();
    if(note != 0x3e)              /*if SAMPLE ONLY slot,then dont change note*/
    {
      funk_chan[c_channel].note_command = 0xf;
      funk_chan[c_channel].volume_command = 0xf;
      do_full_note(note,volume);
    }
  }
}

void normal_decode_note(void)
{
  register int note,volume;

  note = decode_note_no();
  if(note == 0x3d)                /*if ReLOad samples then ...*/
    load_attr_active(reload_sample_attr());
  else
  {
    volume = decode_sample();
    if(note != 0x3e)              /*if SAMPLE ONLY slot,then dont change note*/
    {
      funk_chan[c_channel].volume_command = 0xf;
      do_full_note(note,volume);
    }
  }
}

void normal_decode_volume(void)
{
  register int note,volume;

  note = decode_note_no();
  if(note == 0x3d)                /*if ReLOad samples then ...*/
    load_attr_active(reload_sample_attr());
  else
  {
    volume = decode_sample();
    if(note != 0x3e)              /*if SAMPLE ONLY slot,then dont change note*/
    {
      funk_chan[c_channel].note_command = 0xf;
      do_full_note(note,volume);
    }
  }
}

/***********************************
*
***********************************/
void comc_decode(void)
{
  register int note,volume;

  note = decode_note_no();
  if(note == 0x3d)                /*if ReLOad samples then ...*/
    load_attr_active(reload_sample_attr());
  else
  {
    volume = decode_sample();
    if(note != 0x3e)              /*if SAMPLE ONLY slot,then dont change note*/
    {
      funk_chan[c_channel].volume_command = 0xf;
      funk_chan[c_channel].volume = volume;
      funk_chan[c_channel].note = note;
      funk_chan[c_channel].ifreq_portdest = note_2_ifreq(note);
      funk_chan[c_channel].rfreq_portdest = ifreq_to_rfreq(funk_chan[c_channel].ifreq_portdest);
      vol_to_realvol();
      CARD_freq_convert(c_channel,funk_chan[c_channel].rfreq);
      volume_convert();
    }
  }
}

/***********************************
*
***********************************/
void comi_decode(void)
{
  register int note,volume;

  note = decode_note_no();
  if(note == 0x3d)                /*if ReLOad samples then ...*/
    funk_chan[c_channel].volume_portdest = reload_sample_attr();
  else
  {
    volume = decode_sample();
    if(note != 0x3e)              /*if SAMPLE ONLY slot,then dont change note*/
    {
      funk_chan[c_channel].note_command = 0xf;
      funk_chan[c_channel].volume_portdest = volume;
      funk_chan[c_channel].note = note;
      funk_chan[c_channel].ifreq = note_2_ifreq(note);
      funk_chan[c_channel].rfreq = ifreq_to_rfreq(funk_chan[c_channel].ifreq);
      vol_to_realvol();
      CARD_freq_convert(c_channel,funk_chan[c_channel].rfreq);
      volume_convert();
    }
    else
    {
      funk_chan[c_channel].volume_portdest = volume;
    }
  }
}

/***************************************************************************
*
***************************************************************************/
#define IPORT_TOP_LIM 448
#define RPORT_TOP_LIM (0x1b4f4d0 / 448)

#define IPORT_BOT_LIM 13696
#define RPORT_BOT_LIM (0x1b4f4d0 / 13696)

/***********************************
*0Com A: port up  (modified as from R2)
*
* com_val : 00000000
*           \ rate /
*
***********************************/
void com_port_up_log(int note_com_val)
{
  funk_chan[c_channel].ifreq -= (note_com_val + 1) << 2;
  if(funk_chan[c_channel].ifreq <= IPORT_TOP_LIM)
  {
    funk_chan[c_channel].note_command = 0xf;
    funk_chan[c_channel].ifreq = IPORT_TOP_LIM;
  }
  funk_chan[c_channel].rfreq = ifreq_to_rfreq(funk_chan[c_channel].ifreq);
  CARD_freq_convert(c_channel,funk_chan[c_channel].rfreq);
}

void com_port_up_lin(int note_com_val)
{
  funk_chan[c_channel].rfreq += (note_com_val + 1) << 3;
  if(funk_chan[c_channel].rfreq >= RPORT_TOP_LIM)
  {
    funk_chan[c_channel].note_command = 0xf;
    funk_chan[c_channel].rfreq = RPORT_TOP_LIM;
  }
  CARD_freq_convert(c_channel,funk_chan[c_channel].rfreq);
}

/***********************************
*1Com B: port down (modified as of R2)
*
* com_val : 00000000
*           \ rate /
*
***********************************/
void com_port_dn_log(int note_com_val)
{
  funk_chan[c_channel].ifreq += (note_com_val + 1) << 2;
  if(funk_chan[c_channel].ifreq >= IPORT_BOT_LIM)
  {
    funk_chan[c_channel].note_command = 0xf;
    funk_chan[c_channel].ifreq = IPORT_BOT_LIM;
  }
  funk_chan[c_channel].rfreq = ifreq_to_rfreq(funk_chan[c_channel].ifreq);
  CARD_freq_convert(c_channel,funk_chan[c_channel].rfreq);
}

void com_port_dn_lin(int note_com_val)
{
  funk_chan[c_channel].rfreq -= (note_com_val + 1) << 3;
  if(funk_chan[c_channel].rfreq <= RPORT_BOT_LIM)
  {
    funk_chan[c_channel].note_command = 0xf;
    funk_chan[c_channel].rfreq = RPORT_BOT_LIM;
  }
  CARD_freq_convert(c_channel,funk_chan[c_channel].rfreq);
}

/***********************************
*2Com C: port to note (modified as of R2)
*
***********************************/
void com_porta_log_subm1(void)
{
  funk_chan[c_channel].note_command = 0xf;
  funk_chan[c_channel].ifreq = funk_chan[c_channel].ifreq_portdest;
  funk_chan[c_channel].rfreq = ifreq_to_rfreq(funk_chan[c_channel].ifreq);
  CARD_freq_convert(c_channel,funk_chan[c_channel].rfreq);
}

void com_porta_log(void)
{
  if(funk_chan[c_channel].ifreq == funk_chan[c_channel].ifreq_portdest)
    funk_chan[c_channel].note_command = 0xf;
  else
    if(funk_chan[c_channel].ifreq > funk_chan[c_channel].ifreq_portdest)
    {
      com_port_up_log(funk_chan[c_channel].note_com_val);
      if(funk_chan[c_channel].ifreq <= funk_chan[c_channel].ifreq_portdest)
        com_porta_log_subm1();
    }
    else
    {
      com_port_dn_log(funk_chan[c_channel].note_com_val);
      if(funk_chan[c_channel].ifreq >= funk_chan[c_channel].ifreq_portdest)
        com_porta_log_subm1();
    }
}

void com_porta_lin_subm1(void)
{
  funk_chan[c_channel].note_command = 0xf;
  funk_chan[c_channel].rfreq = funk_chan[c_channel].rfreq_portdest;
  CARD_freq_convert(c_channel,funk_chan[c_channel].rfreq);
}

void com_porta_lin(void)
{
  if(funk_chan[c_channel].rfreq == funk_chan[c_channel].rfreq_portdest)
    funk_chan[c_channel].note_command = 0xf;
  else
    if(funk_chan[c_channel].rfreq < funk_chan[c_channel].rfreq_portdest)
    {
      com_port_up_lin(funk_chan[c_channel].note_com_val);
      if(funk_chan[c_channel].rfreq >= funk_chan[c_channel].rfreq_portdest)
        com_porta_lin_subm1();
    }
    else
    {
      com_port_dn_lin(funk_chan[c_channel].note_com_val);
      if(funk_chan[c_channel].rfreq <= funk_chan[c_channel].rfreq_portdest)
        com_porta_lin_subm1();
    }
}

/***********************************
*3Com D: vibrato
*
* com_val :   0000   0000
*           \speed/ \amplitude/
*
*vib_waveform = 0   : sine
*               1   : triangle
*               2   : square
*               3   : sawtooth
*               4   : random
*
***********************************/
#define VIBAMP 1

void com_vibrato(void)
{
  funk_chan[c_channel].ifreq_vibrato = com_vib_func(
    funk_chan[c_channel].vib_waveform,
    funk_chan[c_channel].note_com_val & 0xf, /*amp*/
    funk_chan[c_channel].note_com_val >> 4,  /*speed*/
    &funk_chan[c_channel].vib_ptr) << VIBAMP;
  funk_chan[c_channel].rfreq = ifreq_to_rfreq_vib(funk_chan[c_channel].ifreq);
  CARD_freq_convert(c_channel,funk_chan[c_channel].rfreq);
}

/***********************************
*4Com E: vibrato Fanin
*
* com_val :   0000   0000
*           \speed/ \fanin value/
*
*vib_waveform = 0   : sine
*               1   : triangle
*               2   : square
*               3   : sawtooth
*               4   : random
*
***********************************/
void com_vib_fanin(void)
{
  register int fanin,speed;

  fanin = funk_chan[c_channel].note_com_val & 0xf;
  speed = (funk_chan[c_channel].note_com_val & 0xf0) >> 2;

  if(funk_chan[c_channel].note_beat_count < 0xf)
  {
    if(funk_chan[c_channel].note_comspd_count < speed)
      funk_chan[c_channel].note_comspd_count++;
    else
    {
      funk_chan[c_channel].note_comspd_count = 0;
      funk_chan[c_channel].note_beat_count++;
    }
  }
  funk_chan[c_channel].ifreq_vibrato = com_vib_func(
    funk_chan[c_channel].vib_waveform,
    fanin,                                 /*amp*/
    funk_chan[c_channel].note_beat_count,  /*speed*/
    &funk_chan[c_channel].vib_ptr) << VIBAMP;
  funk_chan[c_channel].rfreq = ifreq_to_rfreq_vib(funk_chan[c_channel].ifreq);
  CARD_freq_convert(c_channel,funk_chan[c_channel].rfreq);
}

/***********************************
*5Com F: vibrato Fanout
*
* com_val :   0000   0000
*           \speed/ \fanin value/
*
*vib_waveform = 0   : sine
*               1   : triangle
*               2   : square
*               3   : sawtooth
*               4   : random
*
***********************************/
void com_vib_fanout(void)
{
  register int fanin,speed;

  fanin = funk_chan[c_channel].note_com_val & 0xf;
  speed = (funk_chan[c_channel].note_com_val & 0xf0) >> 2;

  if(funk_chan[c_channel].note_beat_count > 0)
  {
    if(funk_chan[c_channel].note_comspd_count < speed)
      funk_chan[c_channel].note_comspd_count++;
    else
    {
      funk_chan[c_channel].note_comspd_count = 0;
      funk_chan[c_channel].note_beat_count--;
    }
  }
  funk_chan[c_channel].ifreq_vibrato = com_vib_func(
    funk_chan[c_channel].vib_waveform,
    fanin,                                 /*amp*/
    funk_chan[c_channel].note_beat_count,  /*speed*/
    &funk_chan[c_channel].vib_ptr) << VIBAMP;
  funk_chan[c_channel].rfreq = ifreq_to_rfreq_vib(funk_chan[c_channel].ifreq);
  CARD_freq_convert(c_channel,funk_chan[c_channel].rfreq);
}

/***********************************
*6Com G: volume sld up
*
* com_val : 0000   0000
*                 \rate/
*
***********************************/
void com_vol_up(void)
{
  funk_chan[c_channel].volume +=
    com_volume_inc[funk_chan[c_channel].volume_com_val & 0xf];

  if(funk_chan[c_channel].volume > 255)
  {
    funk_chan[c_channel].volume_command = 0xf;
    funk_chan[c_channel].volume = 255;
  }
  vol_to_realvol();
  volume_convert();
}

/***********************************
*7Com H: volume slide down
*
* com_val : 0000   0000
*                 \rate/
*
***********************************/
void com_vol_dn(void)
{
  funk_chan[c_channel].volume -=
    com_volume_inc[funk_chan[c_channel].volume_com_val & 0xf];
  if(funk_chan[c_channel].volume < 0)
  {
    funk_chan[c_channel].volume_command = 0xf;
    funk_chan[c_channel].volume = 0;
  }
  vol_to_realvol();
  volume_convert();
}

/***********************************
*8Com I: volume porta (Rapid ctrl)
*
***********************************/
void com_vol_porta_subm1(void)
{
  funk_chan[c_channel].volume_command = 0xf;
  funk_chan[c_channel].volume = funk_chan[c_channel].volume_portdest;
  vol_to_realvol();
  volume_convert();
}

void com_vol_porta(void)
{
  if(funk_chan[c_channel].volume == funk_chan[c_channel].volume_portdest)
    funk_chan[c_channel].volume_command = 0xf;
  else
  {
    if(funk_chan[c_channel].volume >= funk_chan[c_channel].volume_portdest)
    {
      com_vol_dn();
      if(funk_chan[c_channel].volume <= funk_chan[c_channel].volume_portdest)
        com_vol_porta_subm1();
    }
    else
    {
      com_vol_up();
      if(funk_chan[c_channel].volume >= funk_chan[c_channel].volume_portdest)
        com_vol_porta_subm1();
    }
  }
}

/*********************************
*
*********************************/
void vol_sys_rapid_p1(void)
{
  if((funk_chan[c_channel].volume_command >= 6) &&
     (funk_chan[c_channel].volume_command <= 8))
  {
    if(funk_chan[c_channel].volume_comspd_count <
       (funk_chan[c_channel].volume_com_val >> 4))
    {
      funk_chan[c_channel].volume_comspd_count++;
    }
    else
    {
      funk_chan[c_channel].volume_comspd_count = 0;
      switch(funk_chan[c_channel].volume_command)
      {
      case 0x6:
        com_vol_up();
        break;
      case 0x7:
        com_vol_dn();
        break;
      case 0x8:
        com_vol_porta();
        break;
      }
    }
  }
}

/***********************************
*9Com J: Volume Reverb
*
* A simulated echo effect
*
* com_val : 0000   0000
*
*
***********************************/
void com_reverb(void)
{
  register int incre = (funk_chan[c_channel].volume_com_val & 0xf);
  register int speed = (funk_chan[c_channel].volume_com_val & 0xf0) >> 2;
  if(funk_chan[c_channel].volume_comspd_count < speed)
  {
    funk_chan[c_channel].volume_comspd_count++;
    if(funk_chan[c_channel].volume > 0)
    {
      funk_chan[c_channel].volume -= com_volume_inc[incre];
      if(funk_chan[c_channel].volume < 0)
        funk_chan[c_channel].volume = 0;
    }
  }
  else /*echo*/
  {
    funk_chan[c_channel].volume_comspd_count = 0;
    funk_chan[c_channel].volume_beat_count -=
      (-funk_chan[c_channel].volume_com_val & 0xf0) >> 4;
    if(funk_chan[c_channel].volume_beat_count > 0)
    {
      funk_chan[c_channel].volume = funk_chan[c_channel].volume_beat_count;
    }
    else
    {
      funk_chan[c_channel].volume_command = 0xf;
      funk_chan[c_channel].volume = 0;
    }
  }
  vol_to_realvol();
  volume_convert();
}

/***********************************
*ACom K: tremola
*
* com_val :  0000    0000
*                   \rate/
*
***********************************/
void com_tremola(void)
{
  funk_chan[c_channel].volume_vibrato = com_vib_func(
    funk_chan[c_channel].vol_vib_waveform,
    funk_chan[c_channel].volume_com_val & 0xf, /*amp*/
    funk_chan[c_channel].volume_com_val >> 4,  /*speed*/
    &funk_chan[c_channel].vol_vib_ptr);
  vol_to_realvol_vib();
  volume_convert();
}

/***********************************
*BCom L: arpeggio
*
* com_val :  0000    0000
*            \N1/    \N2/
*
***********************************/
void com_arpeggio(void)
{
  register int arp_note;

  if(funk_chan[c_channel].note_comspd_count < funk_chan[c_channel].arp_speed)
    funk_chan[c_channel].note_comspd_count++;
  else
  {
    funk_chan[c_channel].note_comspd_count = 0;
    arp_note = funk_chan[c_channel].note;
    switch(funk_chan[c_channel].note_beat_count)
    {
      case 1:
        arp_note += (funk_chan[c_channel].note_com_val >> 4);
        break;
      case 2:
        arp_note += (funk_chan[c_channel].note_com_val & 0xf);
        break;
    }
    if(arp_note > 59)
      arp_note = funk_chan[c_channel].note;
    funk_chan[c_channel].ifreq = note_2_ifreq(arp_note);
    funk_chan[c_channel].rfreq = ifreq_to_rfreq(funk_chan[c_channel].ifreq);
    CARD_freq_convert(c_channel,funk_chan[c_channel].rfreq);
    if(funk_chan[c_channel].note_beat_count == 2)
      funk_chan[c_channel].note_beat_count = 0;
    else
      funk_chan[c_channel].note_beat_count++;
  }
}

/***********************************
*CCom M: sample offset
*
* This is a all present sample
* offset that is effective all the
* time for a given channel
*
*
***********************************/
void com_sample_offset(void)
{
  register long offs = funk_chan[c_channel].com_val <<
                       funk_chan[c_channel].sample_ofs_parm;
  if(offs > funk_hr_ptr->funk_sb[funk_chan[c_channel].sample].length)
    offs = funk_hr_ptr->funk_sb[funk_chan[c_channel].sample].length;
  chmix[c_channel].sample_ptr += offs;
}

/***********************************
*DCom N: volume
*
***********************************/
void com_volume(void)
{
  funk_chan[c_channel].volume = funk_chan[c_channel].volume_com_val;
  vol_to_realvol();
  volume_convert();
}

/***********************************
*ECom O0: misc control (cmd revised since R1)
*
*value  control set
*------------------
*0     vibrato sine
*1     vibrato triangle
*2     vibrato square
*3     vibrato sawtooth
*4     vibrato random
*5     tremola sine
*6     tremola triangle
*7     tremola square
*8     tremola sawtooth
*9     tremola random
*a     halt note system
*b     halt volume system
*c     halt all systems
*d     invert funkcrtl
*e     fine balance pann left
*f     fine balance pann right
*
***********************************/
void com_misccrtl(void)
{
  switch(funk_chan[c_channel].com_val & 0xf)
  {
    case 0x0:
      funk_chan[c_channel].vib_waveform = 0;
      break;
    case 0x1:
      funk_chan[c_channel].vib_waveform = 1;
      break;
    case 0x2:
      funk_chan[c_channel].vib_waveform = 2;
      break;
    case 0x3:
      funk_chan[c_channel].vib_waveform = 3;
      break;
    case 0x4:
      funk_chan[c_channel].vib_waveform = 4;
      break;
    case 0x5:
      funk_chan[c_channel].vol_vib_waveform = 0;
      break;
    case 0x6:
      funk_chan[c_channel].vol_vib_waveform = 1;
      break;
    case 0x7:
      funk_chan[c_channel].vol_vib_waveform = 2;
      break;
    case 0x8:
      funk_chan[c_channel].vol_vib_waveform = 3;
      break;
    case 0x9:
      funk_chan[c_channel].vol_vib_waveform = 4;
      break;
    case 0xa:
      funk_chan[c_channel].note_command = 0xf;
      break;
    case 0xb:
      funk_chan[c_channel].volume_command = 0xf;
      break;
    case 0xc:
      funk_chan[c_channel].note_command = 0xf;
      funk_chan[c_channel].volume_command = 0xf;
      break;
    case 0xd:
      /* NOT USED*/
      break;
    case 0xe:
    case 0xf:
      funk_chan[c_channel].volume_command = funk_chan[c_channel].command;
      funk_chan[c_channel].volume_com_val = funk_chan[c_channel].com_val;
      break;
  }
}

void com_pan_left(void)
{
  funk_chan[c_channel].balance--;
  if(funk_chan[c_channel].balance < 0)
  {
    funk_chan[c_channel].volume_command = 0xf;
    funk_chan[c_channel].balance = 0;
  }
  volume_convert();
}

void com_pan_right(void)
{
  funk_chan[c_channel].balance++;
  if(funk_chan[c_channel].balance > 0xff)
  {
    funk_chan[c_channel].volume_command = 0xf;
    funk_chan[c_channel].balance = 0xff;
  }
  volume_convert();
}

/***********************************
* Com O1: volume cut               *
*                                  *
***********************************/
void com_volume_cut(void)
{
  if(funk_chan[c_channel].volume_comspd_count <
     (funk_chan[c_channel].volume_com_val & 0xf))
  {
    funk_chan[c_channel].volume_comspd_count++;
  }
  else
  {
    funk_chan[c_channel].volume_command = 0xf;
    funk_chan[c_channel].volume = 0;
    vol_to_realvol();
    volume_convert();
  }
}

/***********************************
* Com O2: Note Delay
*
*NB/ will only work within a slot time period.
*when the next slot is loaded, the command and value are
*purged (just like retrig)
***********************************/
void com_note_delay(void)
{
  if(funk_chan[c_channel].comspd_count <
     (funk_chan[c_channel].com_val & 0xf))
  {
    funk_chan[c_channel].comspd_count++;
  }
  else
  {
    funk_chan[c_channel].command = 0xf;
    pattern_ptr = funk_chan[c_channel].delay_pattern_ptr;
    normal_decode_system();
  }
}

/***********************************
* Com O3: set arpeggio speed
*
***********************************/
void com_arp_speed_set(void)
{
  funk_chan[c_channel].arp_speed = funk_chan[c_channel].com_val & 0xf;
}

/***********************************
* Com O4: fine port_up
*
* com_val :     0000
*           \ rate /
*
***********************************/
void com_fine_port_up(void)
{
  register int x = funk_chan[c_channel].note_com_val & 0xf;
  if(funk_chan[c_channel].port_type)
    com_port_up_lin(x);
  else
    com_port_up_log(x);
}

/***********************************
* Com O5: fine port_dn
*
* com_val :     0000
*           \ rate /
*
***********************************/
void com_fine_port_dn(void)
{
  register int x = funk_chan[c_channel].note_com_val & 0xf;
  if(funk_chan[c_channel].port_type)
    com_port_dn_lin(x);
  else
    com_port_dn_log(x);
}

/***********************************
* Com O6: fine vol sld up
*
* com_val :        0000
*                 \rate/
*
***********************************/

/*refer to com_vol_up(void)*/

/***********************************
* Com O7: fine vol sld dn
*
* com_val :        0000
*                 \rate/
*
***********************************/

/*refer to com_vol_dn(void)*/

/***********************************
* Com O8: volume crest
*
* com_val :        0000
*                 \speed/ (prehand)
*
* volume slides up then down,
* starting at current frequency
*
***********************************/
void com_vol_crest(void)
{
  switch(funk_chan[c_channel].volume_beat_count)
  {
    case 1: /*@@wait_a_bit*/
      if(funk_chan[c_channel].volume_comspd_count <
        (-((funk_chan[c_channel].volume_com_val & 0xf) + 1) & 0xf))
      {
        funk_chan[c_channel].volume_comspd_count++;
      }
      else
      {
        funk_chan[c_channel].volume_comspd_count = 0;
        funk_chan[c_channel].volume_beat_count++;
      }
      break;
    case 2: /*@@slide_down*/
      funk_chan[c_channel].volume -=
        (com_volume_inc[funk_chan[c_channel].volume_com_val & 0xf] >> 2) + 1;
      if(funk_chan[c_channel].volume <= funk_chan[c_channel].volume_portdest)
      {
        funk_chan[c_channel].volume_command = 0xf;
        funk_chan[c_channel].volume = funk_chan[c_channel].volume_portdest;
      }
      vol_to_realvol();
      volume_convert();
      break;
    default:
      funk_chan[c_channel].volume +=
        com_volume_inc[funk_chan[c_channel].volume_com_val & 0xf];
      if(funk_chan[c_channel].volume > 0xff)
      {
        funk_chan[c_channel].volume_comspd_count = 0;
        funk_chan[c_channel].volume_beat_count++;
        funk_chan[c_channel].volume = 0xff;
      }
      vol_to_realvol();
      volume_convert();
      break;
  }
}

/***********************************
* Com O9: Echo Feedback Gain (was volume though in R1)
*
***********************************/
void com_echo_feedback(void)
{
  chmix[c_channel].echo_feedback = funk_chan[c_channel].com_val & 0xf;
}

/***********************************
* Com OA: master volume set (cmd revised since R1)
*
* master effects all volumes.
*
* format:  4 3 2 1
*         \| \---/
*         dir   increment
*          1 = up
*          0 = down
***********************************/
void com_master_set(void)
{
  register int tmp = c_channel;
  register int dec_val = (funk_chan[c_channel].com_val & 0x7) << 2;

  if(funk_chan[c_channel].com_val & 0x8)
  {
    funk_info.master_volume += dec_val;
    if(funk_info.master_volume > 0xff)
      funk_info.master_volume = 0xff;
  }
  else
  {
    funk_info.master_volume -= dec_val;
    if(funk_info.master_volume < 0)
      funk_info.master_volume = 0;
  }
  for(c_channel = 0;c_channel < funk_info.no_active_channels;c_channel++)
    volume_convert();
  c_channel = tmp;
}

/***********************************
* Com OB: Echo Delay
*
* In the Funktracker Revision 1 (DOS32 Funktracker), this command
* was "Expand Loop". As the command wasn't used very often (and
* wasn't very useful anyway, it has been replaced by this extremely
* useful command.
***********************************/
void com_echo_delay(void)
{
  chmix[c_channel].echo_delay = funk_chan[c_channel].com_val & 0xf;
}

/***********************************
* Com OC: Echo Decay
*
* Refer to the above comment in Com OB. This command used to be
* "Colapse Loop". I don't think i've ever used these old commands
* actually.
***********************************/
void com_echo_decay(void)
{
   chmix[c_channel].echo_decay = funk_chan[c_channel].com_val & 0xf;
}

/***********************************
* Com OD: note retrig
*
* com_val : ????   0000
*                 \speed/ (prehand)
*
* command only handled on full
* or sample only slots
*
***********************************/
void com_note_retrig(void)
{
  if(funk_chan[c_channel].retrig_spd_count <
    (funk_chan[c_channel].com_val & 0xf))
  {
    funk_chan[c_channel].retrig_spd_count++;
  }
  else
  {
    funk_chan[c_channel].retrig_spd_count = 0;
    if(funk_chan[c_channel].retrig_count < funk_chan[c_channel].retrig_limit)
    {
      funk_chan[c_channel].retrig_count++;
      do_retrig_sample(funk_chan[c_channel].sample);
    }
    else
      funk_chan[c_channel].command = 0xf;
  }
}

/***********************************
* Com OE: set channel balance
*
***********************************/
void com_balance(void)
{
  funk_chan[c_channel].balance = (funk_chan[c_channel].com_val & 0xf) << 4;
  volume_convert();
}

/***********************************
* Com OF: tempo
*
***********************************/
void com_tempo(void)
{
  funk_info.tempo = funk_chan[c_channel].com_val & 0xf;
}

/***************************************************************************
*
* Command control for null slots only
*
***************************************************************************/
void cnc_vs_basic(void)
{
  funk_chan[c_channel].volume_command = funk_chan[c_channel].command;
  funk_chan[c_channel].volume_com_val = funk_chan[c_channel].com_val;
  funk_chan[c_channel].volume_comspd_count = 0;
}

void com_null_ctrl(void)
{
  switch(funk_chan[c_channel].command)
  {
    case 0x0:                                          /*com a*/
      funk_chan[c_channel].note_command = funk_chan[c_channel].command;
      funk_chan[c_channel].note_com_val = funk_chan[c_channel].com_val;
      break;
    case 0x1:                                          /*com b*/
      funk_chan[c_channel].note_command = funk_chan[c_channel].command;
      funk_chan[c_channel].note_com_val = funk_chan[c_channel].com_val;
      break;
    case 0x2:                                          /*com c*/
      funk_chan[c_channel].note_command = funk_chan[c_channel].command;
      funk_chan[c_channel].note_com_val = funk_chan[c_channel].com_val;
      break;
    case 0x3:                                          /*com d*/
      funk_chan[c_channel].note_command = funk_chan[c_channel].command;
      funk_chan[c_channel].note_com_val = funk_chan[c_channel].com_val;
      funk_chan[c_channel].note_comspd_count = 0;
      break;
    case 0x4:                                          /*com e*/
      funk_chan[c_channel].note_beat_count = 0;
      goto set_vib_fan;
    case 0x5:                                          /*com f*/
      funk_chan[c_channel].note_beat_count = 0xf;
set_vib_fan:
      if(funk_chan[c_channel].note_command == 0x3)/*if vibrato command*/
        funk_chan[c_channel].note_beat_count =
          funk_chan[c_channel].note_com_val >> 4;
      funk_chan[c_channel].note_command = funk_chan[c_channel].command;
      funk_chan[c_channel].note_com_val = funk_chan[c_channel].com_val;
      funk_chan[c_channel].note_comspd_count = 0;
      break;
    case 0x6:                                          /*com g*/
      cnc_vs_basic();
      break;
    case 0x7:                                          /*com h*/
      cnc_vs_basic();
      break;
    case 0x8:                                          /*com i*/
      cnc_vs_basic();
      break;
    case 0x9:                                          /*com j*/
      funk_chan[c_channel].volume_command = funk_chan[c_channel].command;
      funk_chan[c_channel].volume_com_val = funk_chan[c_channel].com_val;
      funk_chan[c_channel].volume_comspd_count = 0;
      funk_chan[c_channel].volume_beat_count = funk_chan[c_channel].volume;
      break;
    case 0xa:                                          /*com k*/
      cnc_vs_basic();
      break;
    case 0xb:                                          /*com l*/
      funk_chan[c_channel].note_command = funk_chan[c_channel].command;
      funk_chan[c_channel].note_com_val = funk_chan[c_channel].com_val;
      funk_chan[c_channel].note_comspd_count = 0;
      funk_chan[c_channel].note_beat_count = 1;
      break;
    case 0xd:                                          /*com n*/
      funk_chan[c_channel].volume_command = funk_chan[c_channel].command;
      funk_chan[c_channel].volume_com_val = funk_chan[c_channel].com_val;
      com_volume();
      break;
    case 0xe:                                          /*O commands*/
      switch(funk_chan[c_channel].com_val & 0xf0)
      {
        case 0x00:                                     /*com o0*/
          com_misccrtl();
          break;
        case 0x10:                                     /*com o1*/
          cnc_vs_basic();
          break;
        case 0x30:                                     /*com o3*/
          com_arp_speed_set();
          break;
        case 0x40:                                     /*com o4*/
          funk_chan[c_channel].note_command = funk_chan[c_channel].command;
          funk_chan[c_channel].note_com_val = funk_chan[c_channel].com_val;
          com_fine_port_up();
          break;
        case 0x50:                                     /*com o5*/
          funk_chan[c_channel].note_command = funk_chan[c_channel].command;
          funk_chan[c_channel].note_com_val = funk_chan[c_channel].com_val;
          com_fine_port_dn();
          break;
        case 0x60:                                     /*com o6*/
          funk_chan[c_channel].volume_command = funk_chan[c_channel].command;
          funk_chan[c_channel].volume_com_val = funk_chan[c_channel].com_val;
          com_vol_up();
          break;
        case 0x70:                                     /*com o7*/
          funk_chan[c_channel].volume_command = funk_chan[c_channel].command;
          funk_chan[c_channel].volume_com_val = funk_chan[c_channel].com_val;
          com_vol_dn();
          break;
        case 0x80:                                     /*com o8*/
          funk_chan[c_channel].volume_command = funk_chan[c_channel].command;
          funk_chan[c_channel].volume_com_val = funk_chan[c_channel].com_val;
          funk_chan[c_channel].volume_comspd_count = 0;
          funk_chan[c_channel].volume_beat_count = 0;
          funk_chan[c_channel].volume_portdest = funk_chan[c_channel].volume;
          break;
        case 0x90:                                     /*com o9*/
          com_echo_feedback();
          break;
        case 0xa0:                                     /*com oa*/
          com_master_set();
          break;
        case 0xb0:                                     /*com ob*/
          com_echo_delay();
          break;
        case 0xc0:                                     /*com oc*/
          com_echo_decay();
          break;
        case 0xe0:                                     /*com oe*/
          com_balance();
          break;
        case 0xf0:                                     /*com of*/
          com_tempo();
          break;
      }
      break;
  }
}

/***************************************************************************
*
* Command control for full slots
*
***************************************************************************/
void cfc_vs_basic(void)
{
  funk_chan[c_channel].volume_command = funk_chan[c_channel].command;
  funk_chan[c_channel].volume_com_val = funk_chan[c_channel].com_val;
  funk_chan[c_channel].volume_comspd_count = 0;
  normal_decode_volume();
}

void com_full_ctrl(void)
{
  switch(funk_chan[c_channel].command)
  {
    case 0x0:                                          /*com a*/
      funk_chan[c_channel].note_command = funk_chan[c_channel].command;
      funk_chan[c_channel].note_com_val = funk_chan[c_channel].com_val;
      normal_decode_note();
      break;
    case 0x1:                                          /*com b*/
      funk_chan[c_channel].note_command = funk_chan[c_channel].command;
      funk_chan[c_channel].note_com_val = funk_chan[c_channel].com_val;
      normal_decode_note();
      break;
    case 0x2:                                          /*com c*/
      funk_chan[c_channel].note_command = funk_chan[c_channel].command;
      funk_chan[c_channel].note_com_val = funk_chan[c_channel].com_val;
      comc_decode();
      break;
    case 0x3:                                          /*com d*/
      funk_chan[c_channel].note_command = funk_chan[c_channel].command;
      funk_chan[c_channel].note_com_val = funk_chan[c_channel].com_val;
      funk_chan[c_channel].note_comspd_count = 0;
      normal_decode_note();
      break;
    case 0x4:                                          /*com e*/
      funk_chan[c_channel].note_beat_count = 0;
      goto set_vib_fan;
    case 0x5:                                          /*com f*/
      funk_chan[c_channel].note_beat_count = 0xf;
set_vib_fan:
      if(funk_chan[c_channel].note_command == 0x3) /*if vibrato command*/
        funk_chan[c_channel].note_beat_count =
          funk_chan[c_channel].note_com_val >> 4;
      funk_chan[c_channel].note_command = funk_chan[c_channel].command;
      funk_chan[c_channel].note_com_val = funk_chan[c_channel].com_val;
      funk_chan[c_channel].note_comspd_count = 0;
      normal_decode_note();
      break;
    case 0x6:                                          /*com g*/
      cfc_vs_basic();
      break;
    case 0x7:                                          /*com h*/
      cfc_vs_basic();
      break;
    case 0x8:                                          /*com i*/
      funk_chan[c_channel].volume_command = funk_chan[c_channel].command;
      funk_chan[c_channel].volume_com_val = funk_chan[c_channel].com_val;
      funk_chan[c_channel].volume_comspd_count = 0;
      comi_decode();
      break;
    case 0x9:                                          /*com j*/
      funk_chan[c_channel].volume_command = funk_chan[c_channel].command;
      funk_chan[c_channel].volume_com_val = funk_chan[c_channel].com_val;
      funk_chan[c_channel].volume_comspd_count = 0;
      normal_decode_volume();
      funk_chan[c_channel].volume_beat_count = funk_chan[c_channel].volume;
      break;
    case 0xa:                                          /*com k*/
      cfc_vs_basic();
      break;
    case 0xb:                                          /*com l*/
      funk_chan[c_channel].note_command = funk_chan[c_channel].command;
      funk_chan[c_channel].note_com_val = funk_chan[c_channel].com_val;
      funk_chan[c_channel].note_comspd_count = 0;
      funk_chan[c_channel].note_beat_count = 1;
      normal_decode_note();
      break;
    case 0xc:                                          /*com m*/
      normal_decode_system();
      com_sample_offset();
      break;
    case 0xd:                                          /*com n*/
      funk_chan[c_channel].volume_command = funk_chan[c_channel].command;
      funk_chan[c_channel].volume_com_val = funk_chan[c_channel].com_val;
      normal_decode_volume();
      com_volume();
      break;
    case 0xe:                                          /*O commands*/
      switch(funk_chan[c_channel].com_val & 0xf0)
      {
        case 0x00:                                     /*com o0*/
          normal_decode_system();
          com_misccrtl();
          break;
        case 0x10:                                     /*com o1*/
          cfc_vs_basic();
          break;
        case 0x20:                                     /*com o2*/
          funk_chan[c_channel].comspd_count = 0;
          funk_chan[c_channel].delay_pattern_ptr = pattern_ptr;
          break;
        case 0x30:                                     /*com o3*/
          normal_decode_system();
          com_arp_speed_set();
          break;
        case 0x40:                                     /*com o4*/
          funk_chan[c_channel].note_command = funk_chan[c_channel].command;
          funk_chan[c_channel].note_com_val = funk_chan[c_channel].com_val;
          normal_decode_note();
          com_fine_port_up();
          break;
        case 0x50:                                     /*com o5*/
          funk_chan[c_channel].note_command = funk_chan[c_channel].command;
          funk_chan[c_channel].note_com_val = funk_chan[c_channel].com_val;
          normal_decode_note();
          com_fine_port_dn();
          break;
        case 0x60:                                     /*com o6*/
          funk_chan[c_channel].volume_command = funk_chan[c_channel].command;
          funk_chan[c_channel].volume_com_val = funk_chan[c_channel].com_val;
          normal_decode_volume();
          com_vol_up();
          break;
        case 0x70:                                     /*com o7*/
          funk_chan[c_channel].volume_command = funk_chan[c_channel].command;
          funk_chan[c_channel].volume_com_val = funk_chan[c_channel].com_val;
          normal_decode_volume();
          com_vol_dn();
          break;
        case 0x80:                                     /*com o8*/
          funk_chan[c_channel].volume_command = funk_chan[c_channel].command;
          funk_chan[c_channel].volume_com_val = funk_chan[c_channel].com_val;
          funk_chan[c_channel].volume_comspd_count = 0;
          funk_chan[c_channel].volume_beat_count = 0;
          normal_decode_volume();
          funk_chan[c_channel].volume_portdest = funk_chan[c_channel].volume;
          break;
        case 0x90:                                     /*com o9*/
          normal_decode_system();
          com_echo_feedback();
          break;
        case 0xa0:                                     /*com oa*/
          com_master_set();
          normal_decode_system();
          break;
        case 0xb0:                                     /*com ob*/
          normal_decode_system();
          com_echo_delay();
          break;
        case 0xc0:                                     /*com oc*/
          normal_decode_system();
          com_echo_decay();
          break;
        case 0xd0:                                     /*com od*/
          funk_chan[c_channel].retrig_count = 1;
          funk_chan[c_channel].retrig_spd_count = 0;
          normal_decode_system();
          break;
        case 0xe0:                                     /*com oe*/
          normal_decode_system();
          com_balance();
          break;
        case 0xf0:                                     /*com of*/
          normal_decode_system();
          com_tempo();
          break;
      }
      break;
    case 0xf:
      normal_decode_system();
      break;
  }
}

/***************************************************************************
*
* Command control for rapid "on the fly"
* Rapid commands that have a speed parameter
*
***************************************************************************/
void com_rapid_ctrl(void)
{
  for(c_channel = 0;c_channel < funk_info.no_active_channels;c_channel++)
  {
    vol_sys_rapid_p1();
    switch(funk_chan[c_channel].note_command)
    {
      case 0x0:
        if(funk_chan[c_channel].port_type)
          com_port_up_lin(funk_chan[c_channel].note_com_val);
        else
          com_port_up_log(funk_chan[c_channel].note_com_val);
        break;
      case 0x1:
        if(funk_chan[c_channel].port_type)
          com_port_dn_lin(funk_chan[c_channel].note_com_val);
        else
          com_port_dn_log(funk_chan[c_channel].note_com_val);
        break;
      case 0x2:
        if(funk_chan[c_channel].port_type)
          com_porta_lin();
        else
          com_porta_log();
        break;
      case 0x3:                                        /*command d*/
        com_vibrato();
        break;
      case 0x4:                                        /*command e*/
        com_vib_fanin();
        break;
      case 0x5:                                        /*command f*/
        com_vib_fanout();
        break;
      case 0xb:                                        /*command l*/
        com_arpeggio();
        break;
    }
    switch(funk_chan[c_channel].volume_command)
    {
      case 0x9:                                        /*command j*/
        com_reverb();
        break;
      case 0xa:                                        /*command k*/
        com_tremola();
        break;
      case 0xe:
        switch(funk_chan[c_channel].volume_com_val)
        {
          case 0x0e:
            com_pan_left();
            break;
          case 0x0f:
            com_pan_right();
            break;
        }
        switch(funk_chan[c_channel].volume_com_val & 0xf0)
        {
          case 0x10:                                   /*command o1*/
            com_volume_cut();
            break;
          case 0x80:                                   /*command o8*/
            com_vol_crest();
            break;
        }
        break;
    }
    if(funk_chan[c_channel].command == 0xe)
      switch(funk_chan[c_channel].com_val & 0xf0)
      {
        case 0x20:                                    /*command o2*/
          com_note_delay();
          break;
        case 0xd0:                                    /*command od*/
          com_note_retrig();
          break;
      }
  }
}

/***************************************************************************
*
* ESI = current pattern
*
* format:
*
* 00000000 11111111 22222222
* \    /\     /\  / \      /
*  note  sample com  command value
*
* - if note:  = 3D, then reload sample attrs
*             = 3F, then it's a null slot
*             = 3E, then sample only slot
*
***************************************************************************/
void trekk_slot(int pattern_no)
{
  register tslot *pattern_addr =
    funk_pat_ptr + (pattern_no * 64 * funk_info.no_active_channels) +
    (funk_info.pattern_ofs * funk_info.no_active_channels);
  for(c_channel = 0;c_channel < funk_info.no_active_channels;c_channel++)
  {
    pattern_ptr = pattern_addr + c_channel;
    funk_chan[c_channel].command = pattern_ptr->sam_com & 0xf;
    funk_chan[c_channel].com_val = pattern_ptr->com_val;
    if((pattern_ptr->not_sam >> 2) == 0x3f)
      com_null_ctrl();
    else
      com_full_ctrl();                  /*IF FULL SLOT or SAMPLE ONLY SLOT*/
  }
}

/***************************************************************************
****************************************************************************
****************************************************************************
****************************************************************************
*
* The actual tracker thingy :) ....0.02 second tick (or roughly)
*
****************************************************************************
****************************************************************************
****************************************************************************
***************************************************************************/
void funk_tracker(void)
{
  if(funk_info.trek_status == PLAY)
  {
    com_rapid_ctrl();
    if(funk_info.tempo_count < funk_info.tempo)
      funk_info.tempo_count++;
    else
    {
      funk_info.tempo_count = 0;
      funk_info.pattern_ofs_display = funk_info.pattern_ofs;
      funk_info.sequence_ofs_display = funk_info.sequence_ofs;
      trekk_slot(funk_hr_ptr->order_list[funk_info.sequence_ofs]);
      if(funk_info.pattern_ofs ==
        funk_hr_ptr->break_list[funk_hr_ptr->order_list[funk_info.sequence_ofs]])
      {
        if(funk_info.sequence_ofs < funk_info.no_of_sequences)
        {
          funk_info.sequence_ofs++;
          funk_info.pattern_ofs = 0;
        }
        else
          if(funk_hr_ptr->loop_order == 0xFF)
            funk_info.trek_status = STOP;
          else
            if(funk_hr_ptr->loop_order > funk_info.no_of_sequences)
              funk_info.trek_status = STOP;
            else
            {
              funk_info.sequence_ofs = funk_hr_ptr->loop_order;
              funk_info.pattern_ofs = 0;
            }
      }
      else
        funk_info.pattern_ofs++;
    }
  }
}
