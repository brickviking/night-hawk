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
#include <string.h>
#include <malloc.h>
#include "funktracker_defs.h"
#include "dsp_mixxer.h"
#include "funktracker.h"

int ferr_val;
char *ferr_messages[] =
{
  "Operation successful.",                              /*0*/
  "Fatal memory allocation error (no memory left).",    /*1*/
  "File open error.",                                   /*2*/
  "Fatal file read error (pretty serious shit).",       /*3*/
  "Unrecognised Module signature.",                     /*4*/
  "Song is corrupt!!!!!.",                              /*5*/
  "Song exceeds set pattern limit.",                    /*6*/
  "Warning: R1 Funk module. Some cmds arranged & added (RTFM please).", /*7*/
  "File create error.",                                 /*8*/
  "Fatal file write error (pretty serious shit).",      /*9*/
  "Can't save module with empty Sequence table.",       /*10*/
  "Unknown samples format.",                            /*11*/
  "Module too complex to be saved as .MOD format.",     /*12*/
  "Unknown song file. Can't make out extension type.",  /*13*/
  "Already at precision. Conversion not necessary."     /*14*/
};

void (*virtualmixxer)(void);

/***************************************************************************
*
* 00000000 11111111 22222222
* \    /\     /\  / \      /
*  note  sample com  command value
*
***************************************************************************/
void clear_slot(tslot *slot)
{
  slot->not_sam = 0xfc;
  slot->sam_com = 0x0f;
  slot->com_val = 0x00;
}

void clear_sam_entry(tfunk_sb *funk_sb)
{
  register int y;

  for(y = 0;y < 19;y++) funk_sb->sname[y] = ' ';
  funk_sb->start = 0xffffffff;
  funk_sb->length = 0;
  funk_sb->volume = 0xff;
  funk_sb->balance = 0x80;
  funk_sb->pt_and_sop = 0x08;
  funk_sb->vv_waveform = 0x0;
  funk_sb->rl_and_as = 0x40;
}

void set_BPM(void)
{
  register int x;

  funk_hr_ptr->info[3] &= 1;
  x = funk_info.bpm_rate - 125;
  if(x < 0) x = (-x & 63) | 64;
  funk_hr_ptr->info[3] |= x << 1;
}

/***************************************************************************
*
***************************************************************************/
int alloc_funk_hr(void)
{
  if((funk_hr_ptr = malloc(sizeof(tfunk_hr))) != NULL)
    return FERR_OK;
  return FERR_MALLOC;
}

int alloc_pat_blk(void)
{
  register int x,y;
  tslot *pat_pos;

  long alloc_size =
    funk_info.funk_pd_size *
    64 *
    funk_info.no_active_channels *
    sizeof(tslot);

  if((funk_pat_ptr = malloc(alloc_size)) != NULL)
  {
    for(x = 0;x < funk_info.funk_pd_size;x++)
    {
      pat_pos = funk_pat_ptr + (x * 64 * funk_info.no_active_channels);
      for(y = 0;y < (64 * funk_info.no_active_channels);y++)
        clear_slot(pat_pos + y);
    }
    return FERR_OK;
  }
  return FERR_MALLOC;
}

void dealloc_funk_mem(void)
{
  register int x;

  if(funk_hr_ptr != NULL)
  {
    free(funk_hr_ptr);
    funk_hr_ptr = NULL;
  }
  if(funk_pat_ptr != NULL)
  {
    free(funk_pat_ptr);
    funk_pat_ptr = NULL;
  }
  for(x = 0;x < 64;x++)
    if(funk_sam_ptrs[x] != NULL)
    {
      free(funk_sam_ptrs[x]);
      funk_sam_ptrs[x] = NULL;
    }
}

void varedit_init(void)
{
  register int chan_no;

  for(chan_no = 0;chan_no < funk_info.no_active_channels;chan_no++)
    funk_chan[chan_no].channel_kill = PLAY;
}

/***************************************************************************
*
***************************************************************************/
long find_file_size(FILE *fp)
{
  register long x;

  fseek(fp,0,SEEK_END);
  x = ftell(fp);
  rewind(fp);
  return x;
}

/***************************************************************************
*
***************************************************************************/
void find_pats_seqs(void)
{
  register int x;

  funk_info.no_of_sequences = 0;
  funk_info.no_of_patterns = 0;
  for(x = 0;x < 256;x++)
    if(funk_hr_ptr->order_list[x] == 0xFF)
      break;
    else
    {
      funk_info.no_of_sequences++;
      if(funk_hr_ptr->order_list[x] > funk_info.no_of_patterns)
        funk_info.no_of_patterns = funk_hr_ptr->order_list[x];
    }
  funk_info.no_of_sequences--;
  funk_info.no_of_patterns++;
}

/***************************************************************************
* L O A D   S O N G
***************************************************************************/
void load_funk_module(char *filename)
{
  FILE *funk_fp;

  int dsp_load_sample(int sample_no)
  {
    register unsigned long samsize =
      funk_hr_ptr->funk_sb[sample_no].length * 
      (funk_info.sample_precision >> 3);
    funk_sam_ptrs[sample_no] = malloc(samsize);
    if(funk_sam_ptrs[sample_no] == NULL)
    {
      ferr_val = FERR_MALLOC;
      return 0;
    }
    if(fread(funk_sam_ptrs[sample_no],1,samsize,funk_fp) != samsize)
    {
      ferr_val = FERR_FREAD;
      return 0;
    }
    return 1;
  }

  void clean_old_fnk(void)
  {
    register int x,y;

    for(x = 0;x < 64;x++)
      for(y = 0;y < 19;y++)
        if(((uDB)funk_hr_ptr->funk_sb[x].sname[y] < ' ') ||
           ((uDB)funk_hr_ptr->funk_sb[x].sname[y] > 127))
          funk_hr_ptr->funk_sb[x].sname[y] = ' ';
  }

  void load(void)
  {
    register int x;
    register long file_size,pattern_block_size;
    char tmp[3];

    file_size = find_file_size(funk_fp);
    dealloc_funk_mem();
    ferr_val = alloc_funk_hr();
    if(ferr_val == FERR_OK)
    {
      if(fread(funk_hr_ptr,sizeof(tfunk_hr),1,funk_fp) != 1)
      {
        ferr_val = FERR_FREAD;
        return;
      }
      if(strncmp(funk_hr_ptr->sig,"Funk",4) != 0)
      {
        ferr_val = FERR_FUNK_SIG;
        return;
      }
      if(funk_hr_ptr->LZH_check_size != file_size)
      {
        ferr_val = FERR_FCORRUPT;
        return;
      }
      funk_info.no_active_channels = 8;
      funk_info.sample_precision = 8;
      funk_info.bpm_rate = 125;
      if(funk_hr_ptr->funk_type[0] == 'F')
        if((funk_hr_ptr->funk_type[1] == 'v') ||
           (funk_hr_ptr->funk_type[1] == 'k') ||
           (funk_hr_ptr->funk_type[1] == '2'))
        {
          tmp[0] = funk_hr_ptr->funk_type[2];
          tmp[1] = funk_hr_ptr->funk_type[3];
          tmp[2] = 0;
          sscanf(tmp,"%d",(int *)&(funk_info.no_active_channels));
          if(funk_hr_ptr->funk_type[1] == '2')
          {
            if(funk_hr_ptr->info[3] & 1) funk_info.sample_precision = 16;
            funk_info.bpm_rate = funk_hr_ptr->info[3] >> 1;
            if(funk_info.bpm_rate & 64)
              funk_info.bpm_rate = -(funk_info.bpm_rate & 63);
            funk_info.bpm_rate += 125;
          }
        }
      find_pats_seqs();
      if(funk_info.no_of_patterns > funk_info.funk_pd_size)
      {
        ferr_val = FERR_PAT_LIM;
        return;
      }
      ferr_val = alloc_pat_blk();
      if(ferr_val == FERR_OK)
      {
        pattern_block_size =
          funk_info.no_of_patterns * 64 * funk_info.no_active_channels;
        if(fread(funk_pat_ptr,sizeof(tslot),
                 pattern_block_size,funk_fp) != pattern_block_size)
        {
          ferr_val = FERR_FREAD;
          return;
        }
        for(x = 0;x < 64;x++)
          if(funk_hr_ptr->funk_sb[x].length)
            if(!dsp_load_sample(x))
              return;
        if(funk_hr_ptr->funk_type[1] != '2')
        {
          clean_old_fnk();
          ferr_val = FERR_OLD_FK;
        }
        varedit_init();
      }
    }
  }

  ferr_val = FERR_OK;
  if((funk_fp = fopen(filename,"rb")) != NULL)
  {
    load();
    fclose(funk_fp);
  }
  else
    ferr_val = FERR_FOPEN;
}

/***************************************************************************
*
***************************************************************************/
void dsp_init_for_play(void)
{
  if(funk_info.precision == 8)
    if(funk_info.stereo)
      virtualmixxer = DSPx08_stereo_mixxer;
    else
      virtualmixxer = DSPx08_mono_mixxer;
  else
    if(funk_info.stereo)
      virtualmixxer = DSPx16_stereo_mixxer;
    else
      virtualmixxer = DSPx16_mono_mixxer;

  init_dsp_buffers();
}

void funk_init_for_play(void)
{
  register int chan_no;

  funk_info.trek_status = STOP;
  funk_info.sequence_ofs = 0;
  funk_info.pattern_ofs = 0;
  funk_info.pattern_ofs_display = funk_info.pattern_ofs;
  funk_info.sequence_ofs_display = funk_info.sequence_ofs;
  funk_info.tempo = 4;
  funk_info.tempo_count = 0;
  funk_info.master_volume = 0xff;
  find_pats_seqs();

  for(chan_no = 0;chan_no < funk_info.no_active_channels;chan_no++)
  {
/*-control system-----------------*/
    funk_chan[chan_no].command = 0xf;
    funk_chan[chan_no].com_val = 0;
    funk_chan[chan_no].comspd_count = 0;
    funk_chan[chan_no].sample = 0x3f;
    funk_chan[chan_no].port_type = 0;
    funk_chan[chan_no].sample_ofs_parm = 0x08;
    funk_chan[chan_no].vib_waveform = 0;
    funk_chan[chan_no].vol_vib_waveform = 0;
    funk_chan[chan_no].retrig_limit = 4;
    funk_chan[chan_no].arp_speed = 0;
    funk_chan[chan_no].balance = 0x80;
/*-note system--------------------*/
    funk_chan[chan_no].note_command = 0xf;
    funk_chan[chan_no].note_com_val = 0;
    funk_chan[chan_no].note_comspd_count = 0;
    funk_chan[chan_no].note = 0x3f;
    funk_chan[chan_no].ifreq = 0;
    funk_chan[chan_no].ifreq_vibrato = 0;
    funk_chan[chan_no].ifreq_portdest = 0;
    funk_chan[chan_no].rfreq = 0;
    funk_chan[chan_no].rfreq_portdest = 0;
    funk_chan[chan_no].vib_ptr = 0;
    funk_chan[chan_no].note_beat_count = 0;
/*-volume system------------------*/
    funk_chan[chan_no].volume_command = 0xf;
    funk_chan[chan_no].volume_com_val = 0;
    funk_chan[chan_no].volume_comspd_count = 0;
    funk_chan[chan_no].volume = 0;
    funk_chan[chan_no].volume_vibrato = 0;
    funk_chan[chan_no].volume_portdest = 0;
    funk_chan[chan_no].rvolume = 0;
    funk_chan[chan_no].vol_vib_ptr = 0;
    funk_chan[chan_no].volume_beat_count = 0;

/*-card dependant----------------*/
    chmix[chan_no].start = 0xffffffff;
    chmix[chan_no].length = 0x100000;
    chmix[chan_no].funkctrl = 0;
    chmix[chan_no].sample_ptr = 0;
    chmix[chan_no].freq = 0;
    chmix[chan_no].left_volume = 0;
    chmix[chan_no].right_volume = 0;
    chmix[chan_no].echo_ptr = 0;
    chmix[chan_no].echo_delay = 0;
    chmix[chan_no].echo_decay = 0;
    chmix[chan_no].echo_feedback = 0;
  }
  dsp_init_for_play();
}
