#define SONG1_FILE  INSTALL_DIR  "/data/music/Rescue_from_Vega.fnk"
#define SONG2_FILE  INSTALL_DIR  "/data/music/introtune.fnk"

#define PIPE_PACKET_SIZE 8

#define MAX_FX_ENTRIES   32

#define SND_CMD_LOAD_SONG      'a'
#define SND_CMD_UNLOAD_SONG    'b'
#define SND_CMD_SILENCE        'c'
#define SND_CMD_QUIT           'd'
#define SND_CMD_FX_INIT        'e'
#define SND_CMD_FX             'f'
#define SND_CMD_SET_MASTER     'g'
#define SND_CMD_RANDOM         'h'
#define SND_CMD_LOAD_SONG_DEMO 'i'
#define SND_CMD_FADE_OUT       'j'
#define SND_CMD_FX_SHIP_NOISE  'k'
#define SND_CMD_SILENCE_SNOISE 'l'

#define FX_SELECT           0
#define FX_WEAPON_RELOAD    1
#define FX_SHIP_COMPLETE    2
#define FX_NO_WEAPON        3
#define FX_POWER_UP         4
#define FX_LOW_SHIELDS      5
#define FX_OPEN_DOOR        6
#define FX_EXPLOSION_1      7
#define FX_EXPLOSION_2      8
#define FX_DROID_HIT_1      9
#define FX_LASER_0          10
#define FX_LASER_1          11
#define FX_LASER_2          12
#define FX_LASER_3          13
#define FX_LASER_BUZZ       14
#define FX_DROID_EVASE      15
#define FX_6XX_VOICE        16
#define FX_7XX_VOICE        17
#define FX_8XX_VOICE        18
#define FX_TRANS_TERM       19
#define FX_RANDOM           20
#define FX_ALERT_SIREN      21

/*ship noises*/
#define FX_SHIP_NOISE_START 22

#define FX_SHIP_NOISE1      22
#define FX_SHIP_NOISE2      23
#define FX_SHIP_NOISE3      24
#define FX_SHIP_NOISE4      25
#define FX_SHIP_NOISE5      26
#define FX_SHIP_NOISE6      27
#define FX_SHIP_NOISE7      28
