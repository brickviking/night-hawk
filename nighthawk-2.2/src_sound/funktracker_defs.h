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
* defs
*
****************************************************************************
****************************************************************************
***************************************************************************/
#include "../options.h"

#define FUNK_VERSION  "1.5"
#define WELCOME \
"FunktrackerGOLD version " FUNK_VERSION ", Copyright (C) 1994,1996,1998 Jason Nunn\n\n" \
"FunktrackerGOLD is now under the GNU GENERAL PUBLIC LICENSE AGREEMENT,\n" \
"and comes with ABSOLUTELY NO WARRANTY. This is free software, and you are\n" \
"welcome to redistribute it under certain conditions.\n\n" \
"   Contact: 32 Rothdale Rd, Moil Darwin NT 0810, AUSTRALIA\n" \
"   This mail address may no longer be current by the time you read this\n" \
"     Email: jsno@arafura.net.au\n" \
"       WWW: http://jsno.arafuraconnect.com.au/proj_linux/funk.html\n" \
"\n"

#define STOP 0
#define PLAY 1

#define MAX_BPM_RATE 188
#define MIN_BPM_RATE 62

#define FERR_OK         0
#define FERR_MALLOC     1
#define FERR_FOPEN      2
#define FERR_FREAD      3
#define FERR_FUNK_SIG   4
#define FERR_FCORRUPT   5
#define FERR_PAT_LIM    6
#define FERR_OLD_FK     7
#define FERR_FCREATE    8
#define FERR_FWRITE     9
#define FERR_ESEQTABLE  10
#define FERR_SAMUNKNOWN 11
#define FERR_MODCOMPLEX 12
#define FERR_SNGUNKNOWN 13
#define FERR_PREC_NN    14

#define EM_SAMPLE   0
#define EM_SEQUENCE 1
#define EM_PATTERN  2
#define EM_MAIN     3
#define EM_QUIT     4

/******************************************
* defs for 32bit Linux 1.2.13
*
* we use these to reference alignment critical
* structures in our funk file
*
******************************************/
typedef                char sDB;  /*1 byte*/
typedef unsigned       char uDB;
typedef          short int  sDW;  /*2 bytes*/
typedef unsigned short int  uDW;
typedef                int  sDD;  /*4 bytes*/
typedef unsigned       int  uDD;
typedef             double  sDQ;  /*ansi double - 8 bytes*/

/*********************************************************************
* This structure is used for talking to either a DAC mixxer or DSP mixxer
**********************************************************************/
typedef struct _tchmix
{
  unsigned long start;
  unsigned long length;
  unsigned char funkctrl;
  void          *sample_addr;
  double        sample_ptr;
  double        freq;
  int           left_volume;
  int           right_volume;
  long          *echo_buffer;
  unsigned int  echo_ptr;
  int           echo_delay;  /*16 point* @ 0.02 per sec each*/
  int           echo_decay;  /*also 16 point*/
  int           echo_feedback;
} tchmix;

/*statistical types for info block*/
#define FKCPU_UNKNOWN  0
#define FKCPU_IBMPC1   1
#define FKCPU_IBMPC2   2
#define FKCPU_I386     3
#define FKCPU_I486     4
#define FKCPU_PENTIUM  5
#define FKCPU_LINUX    6
#define FKCPU_FREEBSD  7
#define FKCPU_NOTUSED8 8
#define FKCPU_NOTUSED9 9
#define FKCPU_NOTUSEDA 10
#define FKCPU_NOTUSEDB 11
#define FKCPU_NOTUSEDC 12
#define FKCPU_NOTUSEDD 13
#define FKCPU_NOTUSEDE 14
#define FKCPU_OTHER    15

#define FKCARD_SB20      0
#define FKCARD_SBPRO     1
#define FKCARD_GUSV      2
#define FKCARD_SB10      3
#define FKCARD_SB16      4
#define FKCARD_GUSF      5
#define FKCARD_RIPPED    6
#define FKCARD_PAS       7
#define FKCARD_VOX08     8
#define FKCARD_VOX16     9
#define FKCARD_NOTUSEDA 10
#define FKCARD_NOTUSEDB 11
#define FKCARD_NOTUSEDC 12
#define FKCARD_NOTUSEDD 13
#define FKCARD_NOTUSEDE 14
#define FKCARD_OTHER    15

/*******************************************
* alignment essential structures
*******************************************/
#pragma pack(1)

typedef struct _tfunk_sb
{
  sDB sname[19];
  uDD start;
  uDD length;
  uDB volume;
  uDB balance;
  uDB pt_and_sop;
  uDB vv_waveform;
  uDB rl_and_as;
} tfunk_sb;

typedef struct _tfunk_hr
{
  sDB      sig[4];
  uDB      info[4];
  uDD      LZH_check_size;
  sDB      funk_type[4];
  uDB      loop_order;
  uDB      order_list[256];
  uDB      break_list[128];
  tfunk_sb funk_sb[64];
} tfunk_hr;

typedef struct _tslot
{
  uDB not_sam;
  uDB sam_com;
  uDB com_val;
} tslot;

#pragma pack()
/*******************************************
* alignment not essential
*******************************************/
typedef struct _tfunk_chan
{
/*-control system-----------------*/
  int channel_kill;
  int command;
  int com_val;
  int comspd_count;
  int sample;
  int port_type;
  int sample_ofs_parm;
  int vib_waveform;
  int vol_vib_waveform;
  int retrig_spd_count;
  int retrig_count;
  int retrig_limit;
  int arp_speed;
  int balance;
  tslot *delay_pattern_ptr;
/*-note system--------------------*/
  unsigned char note_command;
  unsigned char note_com_val;
  int note_comspd_count;
  int note;
  long ifreq;
  long ifreq_vibrato;
  long ifreq_portdest;
  long rfreq;
  long rfreq_portdest;
  int vib_ptr;
  int note_beat_count;
/*-volume system-----------------*/
  unsigned char volume_command;
  unsigned char volume_com_val;
  int volume_comspd_count;
  int volume;                  /*changed from uchar to DD*/
  int volume_vibrato;
  int volume_portdest;
  int rvolume;
  int vol_vib_ptr;
  int volume_beat_count;
} tfunk_chan;

typedef struct _tfunk_info
{
  unsigned char funk_cpu_type;
  unsigned char funk_card_type;
  int           trek_status;
  int           sequence_ofs;
  int           pattern_ofs_display;
  int           sequence_ofs_display;
  int           pattern_ofs;
  int           tempo;
  int           tempo_count;
  int           no_of_patterns;
  int           no_of_sequences;
  int           master_volume;
  unsigned long sample_block_size;
  int           funk_pd_size;
  int           no_active_channels;
  int           precision;
  int           sample_precision;
  int           bpm_rate;
  int           hz_rate;
  int           stereo;
  int           sampling_rate;
} tfunk_info;
