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
* playback module.
*
****************************************************************************
****************************************************************************
***************************************************************************/
#include <stdio.h>
#include <malloc.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include "funktracker_defs.h"
#include "dsp_mixxer.h"
#include "funktracker.h"
#include "funkload.h"
#include "nighthawk_sound.h"

int cmdline_sr = DEFAULT_SAM_RATE,
    cmdline_prec = DEFAULT_PRECISION,
    cmdline_stereo = DEFAULT_STEREO,
    cmdline_fx_channels = NO_FX_CHANNELS;

int demo_f = 0;

typedef struct
{
  void *buf;
  int  size;
} tfx_table;

tfx_table fx_table[MAX_FX_ENTRIES];

/***************************************************************************
*
***************************************************************************/
void init(void)
{
  register int x;

  for(x = 0;x < MAX_FX_ENTRIES;x++)
    fx_table[x].buf = NULL;
}

/***************************************************************************
*
***************************************************************************/
int load_fx_file(tfx_table *fx_entry,char *filename)
{
  FILE *fp;

  fp = fopen(filename,"rb");
  if(fp != NULL)
  {
    fseek(fp,0,SEEK_END);
    fx_entry->size = ftell(fp);
    fseek(fp,0,SEEK_SET);
    fx_entry->buf = malloc(fx_entry->size);
    if(fx_entry->buf != NULL)
      if(fread(fx_entry->buf,fx_entry->size,1,fp) == 1)
      {
        register int x;

        for(x = 0;x < fx_entry->size;x++)
          *((char *)fx_entry->buf + x) ^= 0x80;
        return 1;
      }
    fclose(fp);
  }
  return 0;
}

int load_fx(void)
{
  if(!load_fx_file(&(fx_table[FX_SELECT]),
      INSTALL_DIR "/data/fx/select.8"))
    return 0;

  if(!load_fx_file(&(fx_table[FX_WEAPON_RELOAD]),
      INSTALL_DIR "/data/fx/weapon_reload.8"))
    return 0;

  if(!load_fx_file(&(fx_table[FX_SHIP_COMPLETE]),
      INSTALL_DIR "/data/fx/ship_complete.8"))
    return 0;

  if(!load_fx_file(&(fx_table[FX_NO_WEAPON]),
      INSTALL_DIR "/data/fx/no_weapon.8"))
    return 0;

  if(!load_fx_file(&(fx_table[FX_POWER_UP]),
      INSTALL_DIR "/data/fx/power_up.8"))
    return 0;

  if(!load_fx_file(&(fx_table[FX_LOW_SHIELDS]),
      INSTALL_DIR "/data/fx/low_shields.8"))
    return 0;

  if(!load_fx_file(&(fx_table[FX_OPEN_DOOR]),
      INSTALL_DIR "/data/fx/door_open.8"))
    return 0;

  if(!load_fx_file(&(fx_table[FX_EXPLOSION_1]),
      INSTALL_DIR "/data/fx/explosion_1.8"))
    return 0;

  if(!load_fx_file(&(fx_table[FX_EXPLOSION_2]),
      INSTALL_DIR "/data/fx/explosion_2.8"))
    return 0;

  if(!load_fx_file(&(fx_table[FX_DROID_HIT_1]),
      INSTALL_DIR "/data/fx/droid_hit_1.8"))
    return 0;

  if(!load_fx_file(&(fx_table[FX_LASER_0]),
      INSTALL_DIR "/data/fx/laser_linarite.8"))
    return 0;

  if(!load_fx_file(&(fx_table[FX_LASER_1]),
      INSTALL_DIR "/data/fx/laser_crocoite_benzol.8"))
    return 0;

  if(!load_fx_file(&(fx_table[FX_LASER_2]),
      INSTALL_DIR "/data/fx/laser_uvarovite.8"))
    return 0;

  if(!load_fx_file(&(fx_table[FX_LASER_3]),
      INSTALL_DIR "/data/fx/laser_tiger_eye.8"))
    return 0;

  if(!load_fx_file(&(fx_table[FX_LASER_BUZZ]),
      INSTALL_DIR "/data/fx/laser_buzz.8"))
    return 0;

  if(!load_fx_file(&(fx_table[FX_DROID_EVASE]),
      INSTALL_DIR "/data/fx/droid_evase.8"))
    return 0;

  if(!load_fx_file(&(fx_table[FX_6XX_VOICE]),
      INSTALL_DIR "/data/fx/6xx_voice.8"))
    return 0;

  if(!load_fx_file(&(fx_table[FX_7XX_VOICE]),
      INSTALL_DIR "/data/fx/7xx_voice.8"))
    return 0;

  if(!load_fx_file(&(fx_table[FX_8XX_VOICE]),
      INSTALL_DIR "/data/fx/8xx_voice.8"))
    return 0;

  if(!load_fx_file(&(fx_table[FX_TRANS_TERM]),
      INSTALL_DIR "/data/fx/trans_terminated.8"))
    return 0;

  if(!load_fx_file(&(fx_table[FX_RANDOM]),
      INSTALL_DIR "/data/fx/rand.8"))
    return 0;

  if(!load_fx_file(&(fx_table[FX_ALERT_SIREN]),
      INSTALL_DIR "/data/fx/alert_siren.8"))
    return 0;

  if(!load_fx_file(&(fx_table[FX_SHIP_NOISE1]),
      INSTALL_DIR "/data/fx/snoise1.raw"))
    return 0;

  if(!load_fx_file(&(fx_table[FX_SHIP_NOISE2]),
      INSTALL_DIR "/data/fx/snoise2.raw"))
    return 0;

  if(!load_fx_file(&(fx_table[FX_SHIP_NOISE3]),
      INSTALL_DIR "/data/fx/snoise3.raw"))
    return 0;

  if(!load_fx_file(&(fx_table[FX_SHIP_NOISE4]),
      INSTALL_DIR "/data/fx/snoise4.raw"))
    return 0;

  if(!load_fx_file(&(fx_table[FX_SHIP_NOISE5]),
      INSTALL_DIR "/data/fx/snoise5.raw"))
    return 0;

  if(!load_fx_file(&(fx_table[FX_SHIP_NOISE6]),
      INSTALL_DIR "/data/fx/snoise6.raw"))
    return 0;

  if(!load_fx_file(&(fx_table[FX_SHIP_NOISE7]),
      INSTALL_DIR "/data/fx/snoise7_science.raw"))
    return 0;

  return 1;
}

void unload_fx(void)
{
  register int x;

  for(x = 0;x < MAX_FX_ENTRIES;x++)
    if(fx_table[x].buf != NULL)
      free(fx_table[x].buf);
}

/***************************************************************************
*
***************************************************************************/
int cmdline(int argc,char *argv[])
{
  register int x;
  int y;

  for(x = 1;x < argc;x++)
  {
    if(argv[x][0] == '-')
    {
      switch(argv[x][1])
      {
        case 'p':
          sscanf(argv[x],"-p%d",&funk_info.funk_pd_size);
          break;
        case 's':
          sscanf(argv[x],"-s%d",&cmdline_sr);
          break;
        case 'P':
          sscanf(argv[x],"-P%d",&y);
          if(y == 8) cmdline_prec = 8;
          if(y == 16) cmdline_prec = 16;
/* if we haven't defined a precision (or a CORRECT precision) then default to 8 
 * I just wish we had a way of warning the user that s/he had better choose 8
 * or 16 (not 1, like I tried to do 8-) */
					if (!cmdline_prec) cmdline_prec = 8;
          break;
        case 'S':
          sscanf(argv[x],"-S%d",&cmdline_stereo);
          break;
        case 'x':
          sscanf(argv[x],"-x%d",&cmdline_fx_channels);
          break;
      }
    }
  }
  return 1;
}

/***************************************************************************
*
***************************************************************************/
void fire_fx_sub(int s,int vol,int bal,int funk,int fx_start_ch)
{
  register int x;

  for(x = fx_start_ch;x < funk_info.no_active_channels;x++)
    if(!chmix[x].funkctrl)
    {
      chmix[x].start = 0;
      chmix[x].length = fx_table[s].size;
      chmix[x].sample_addr = fx_table[s].buf;
      chmix[x].sample_ptr = 0;
      DSPi_freq_convert(x,11000);
      DSPi_volume_convert(x,vol,bal);
      chmix[x].funkctrl = funk;
      return;
    }
}

void fire_fx(int s,int vol,int bal,int funk)
{
  if(demo_f)
    fire_fx_sub(s,vol,bal,funk,4);
  else
    fire_fx_sub(s,vol,bal,funk,1);
}

void ship_noise(int s,int vol,int freq)
{
  if(demo_f)
    return;

  chmix[0].start = 0;
  chmix[0].length = fx_table[s].size;
  chmix[0].sample_addr = fx_table[s].buf;
  chmix[0].sample_ptr = 0;
  DSPi_freq_convert(0,freq << 8);
  DSPi_volume_convert(0,vol,0x80);
  chmix[0].funkctrl = 0x3;
}

/***************************************************************************
*
***************************************************************************/
void mixxer_init(void)
{
  register int chan_no;

  funk_info.trek_status = STOP;
  funk_info.no_active_channels = cmdline_fx_channels;
  funk_info.master_volume = 0xff;
  funk_info.bpm_rate = 125;
  for(chan_no = 0;chan_no < funk_info.no_active_channels;chan_no++)
  {
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

/***************************************************************************
*
***************************************************************************/
void decode_packet(char packet[],int *sam,int *vol,int *bal)
{
  *sam = ((packet[1] - 'a') << 4) + (packet[2] - 'a');
  *vol = ((packet[3] - 'a') << 4) + (packet[4] - 'a');
  *bal = ((packet[5] - 'a') << 4) + (packet[6] - 'a');
}

int main(int argc,char *argv[])
{
  register int running = 1;

  init();
  printf("Nighthawk sound engine (with FunktrackerGOLD) running.\n");
  funk_info.funk_pd_size = MAXIMUM_PATTERNS;
  if(cmdline(argc,argv))
  {
    if(funk_info.funk_pd_size < 1) funk_info.funk_pd_size = 1;
    if(funk_info.funk_pd_size > 128) funk_info.funk_pd_size = 128;
    cmdline_stereo &= 1;
    printf("Max Pattern allocation: %d\n",funk_info.funk_pd_size);
    if(open_dsp(cmdline_sr,cmdline_prec,cmdline_stereo))
    {
      if(cmdline_fx_channels < 4) cmdline_fx_channels = 4;
      if(cmdline_fx_channels > 16 ) cmdline_fx_channels = 16;
      printf("FX channels: %d\n",cmdline_fx_channels);
      if(load_fx()) 
      {
        struct timeval tv = {0,0};

        printf("FX files loaded.\n");
        mixxer_init();
        while(running)
        {
          fd_set rfds;

          virtualmixxer();
          FD_ZERO(&rfds);
          FD_SET(0,&rfds);
          if(select(1,&rfds,NULL,NULL,&tv))
          {
            char packet[PIPE_PACKET_SIZE];

            read(0,packet,PIPE_PACKET_SIZE);
            switch(packet[0])
            {
              case SND_CMD_LOAD_SONG:
                load_funk_module(SONG1_FILE);
                if(ferr_val == FERR_OK)
                {
                  funk_init_for_play();
                  funk_info.master_volume = 0xb0;
                  funk_info.trek_status = PLAY;
                }
                else
                  dealloc_funk_mem();
                break;
              case SND_CMD_LOAD_SONG_DEMO:
                demo_f = 1;
                load_funk_module(SONG2_FILE);
                if(ferr_val == FERR_OK)
                {
                  funk_init_for_play();
                  funk_info.trek_status = PLAY;
                }
                else
                  dealloc_funk_mem();
                break;
              case SND_CMD_UNLOAD_SONG:
                demo_f = 0;
                funk_info.trek_status = STOP;
                dealloc_funk_mem();
                mixxer_init();
                break;
              case SND_CMD_SILENCE:
                {
                  register int x;

                  for(x = 0;x < funk_info.no_active_channels;x++)
                    DSPi_volume_convert(x,0,0x80);
                }
                break;
              case SND_CMD_SILENCE_SNOISE:
                DSPi_volume_convert(0,0,0x80);
                break;
              case SND_CMD_QUIT:
                running = 0;
                printf("Nighthawk sound engine ending nicely.\n");
                break;
              case SND_CMD_FX_INIT:
                mixxer_init();
                break;
              case SND_CMD_FX:
                {
                  int s,v,b;

                  decode_packet(packet,&s,&v,&b);
                  fire_fx(s,v,b,2);
                }
                break;
              case SND_CMD_FX_SHIP_NOISE:
                {
                  int s,v,f;

                  decode_packet(packet,&s,&v,&f);
                  ship_noise(s,v,f);
                }
                break;
              case SND_CMD_SET_MASTER:
                funk_info.master_volume =
                  ((packet[1] - 'a') << 4) + (packet[2] - 'a');
                break;
              case SND_CMD_RANDOM:
                {
                  register int x;

                  for(x = 0;x < funk_info.no_active_channels;x++)
                    chmix[x].funkctrl = 0;
                  fire_fx(FX_RANDOM,0xa0,0x80,3);
                }
                break;
              case SND_CMD_FADE_OUT:
                funk_info.master_volume -= 8;
                if(funk_info.master_volume < 0)
                  funk_info.master_volume = 0;
                break;
            }
          }
        }
      }
      else
        printf("Error: loading FX files.\n");
      unload_fx();
    }
    dealloc_funk_mem();
    close_dsp();
  }
  return 1;
}
