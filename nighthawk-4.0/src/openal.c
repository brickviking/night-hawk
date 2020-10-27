/***************************************************************************
****************************************************************************
****************************************************************************
*
* Nighthawk Copyright (C) 1997 Jason Nunn
* Nighthawk is under the GNU General Public License.
*
* Sourceforge admin: Eric Gillespie
* night-hawk.sourceforge.net
*
* See README file for a list of contributors.
*
* ================================================================
* openal.c - Nighthawk sound and music. Uses OpenAL library.

* This code created with the help of
* https://ffainelli.github.io/openal-example/
* A really good numpty guide on getting something up and running quickly.
* and http://xmp.sourceforge.net/examples/player-openal.c on how to implement
* sliding window technique. JN, 14SEP20
*
* For the OGG loader code, thanks to
* https://xiph.org/vorbis/doc/vorbisfile/example.html and
* https://gist.github.com/tilkinsc/f91d2a74cff62cc3760a7c9291290b29
* for helping me create this code. Very simple and easy example. JN, 14SEP20
*
****************************************************************************
****************************************************************************
***************************************************************************/
#include <stdio.h>
#include <string.h>
#include <semaphore.h>
#include <AL/al.h>
#include <AL/alc.h>
#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>
#include "defs.h"
#include "misc_externs.h"

static int sound_enabled_f = TRUE;

static ALCdevice	*al_device;
static ALCcontext	*al_context;
static ALfloat		listener_orient[] = {
	0.0, 0.0, 0.0, 0.0, 1.0, 0.0
};

static ALuint		primary_source,
			ship_noise_source,
			fx_mixed_source[NO_OF_FX_MIXED_SOURCES];

static ALuint		music_buffer[NO_OF_MUSIC_BUFFERS];
ALuint			fx_buffer_table[MAX_FX_BUFFER_ENTRIES];

static ALenum		alerr;

/*
 * Music ogg file variables
 */
static FILE		*ogg_fp;
static OggVorbis_File	ogg_vf;
static vorbis_info	*ogg_vi;
static char		ogg_data[OGG_DATA_SIZE];

static int		music_playing_f = FALSE;

/***************************************************************************
*
***************************************************************************/
static void sound_warning(const char *func, char *msg, ALenum e)
{
	if (verbose_logging == FALSE)
		return;

	printf("Warning (sound): %s:%s (err=%X)\n", func, msg, e);
}

/***************************************************************************
* Play sources
***************************************************************************/
static void play_ship_noise_source(int fx_no, float vol, float freq)
{
	alSourceStop(ship_noise_source);
	alSourcei(ship_noise_source, AL_BUFFER, fx_buffer_table[fx_no]);
	alSourcef(ship_noise_source, AL_GAIN, vol);
	alSourcef(ship_noise_source, AL_PITCH, freq);
	alSourcePlay(ship_noise_source);
}

static void play_primary_source(int fx_no, float vol, float freq)
{
	alSourceStop(primary_source);
	alSourcei(primary_source, AL_BUFFER, fx_buffer_table[fx_no]);
	alSourcef(primary_source, AL_GAIN, vol);
	alSourcef(primary_source, AL_PITCH, freq);
	alSourcei(primary_source, AL_LOOPING, AL_FALSE);
	alSourcePlay(primary_source);
}

static void play_primary_source_loop(int fx_no, float vol)
{
	alSourceStop(primary_source);
	alSourcei(primary_source, AL_BUFFER, fx_buffer_table[fx_no]);
	alSourcef(primary_source, AL_GAIN, vol);
	alSourcef(primary_source, AL_PITCH, 1.0);
	alSourcei(primary_source, AL_LOOPING, AL_TRUE);
	alSourcePlay(primary_source);
}

static void play_fx_mixed_source(snd_event_st *se)
{
	static int sp;
	ALuint src;

	/*
	 * Get next fx source from source array. JN, 18SEP20
	 */
	src = fx_mixed_source[sp];
	sp++;
	if (sp == NO_OF_FX_MIXED_SOURCES)
		sp = 0;

	/*
	 * Setup and play source. JN, 18SEP20
	 */
	alSourceStop(src);
	alSourcei(src, AL_BUFFER, fx_buffer_table[se->fx_no]);
	alSourcef(src, AL_GAIN, se->vol);
	alSourcef(src, AL_PITCH, se->freq);
	alSource3f(src, AL_POSITION, (ALfloat)se->rx, (ALfloat)se->ry, 0.0);
	alSourcei(src, AL_LOOPING, AL_FALSE);
	alSourcePlay(src);
}

static void stop_fx_mixed_source(void)
{
	for (int i = 0; i < NO_OF_FX_MIXED_SOURCES; i++)
		alSourceStop(fx_mixed_source[i]);
}

/***************************************************************************
* Load some ogg data. This is uncompressed music data in the ogg_data buffer.
* Nb/ OGG native format is float samples, so the bitsex, prec and sign is
* what we tell it to produce. Note that ov_read() reads very small buffers,
* whose size is not a practical alBuffer size. So we do multiple reads of
* ov_read() to buld up a nice size buffer equalling OGG_DATA_SIZE. JN, 14SEP20
***************************************************************************/
static int read_ogg_file(void)
{
	int size = 0;

	for (size = 0;;) {
		int d, rs, bs_ptr;

		d = OGG_DATA_SIZE - size;
		if (d > OV_READ_BLOCK_SIZE)
			d = OV_READ_BLOCK_SIZE;
		else if (d == 0)
			break;

		rs = ov_read(
			&ogg_vf,
			ogg_data + size,
			d,
			MUSIC_IS_LITTLEENDIAN,
			MUSIC_16BIT_PRECISION,
			MUSIC_IS_SIGNED,
			&bs_ptr);

		if (rs == 0) /*EOF ?*/
			break;

		if (rs < 0) /*Error ?*/
			printf_error("%s:ov_read()", __func__);

		size += rs;
	}

	return size;
}

/***************************************************************************
*
***************************************************************************/
static void close_ogg_file(void)
{
	if (ogg_fp == NULL)
		return;
	ov_clear(&ogg_vf);
	fclose(ogg_fp);
	ogg_fp = NULL;
}

void stop_music(void)
{
	music_playing_f = FALSE;

	alSourceStop(primary_source);
	alSourcei(primary_source, AL_BUFFER, 0);
	alDeleteBuffers(NO_OF_MUSIC_BUFFERS, music_buffer);
	if ((alerr = alGetError()) != AL_NO_ERROR)
		sound_warning(__func__, "alDeleteBuffers(music_buffer)", alerr);

	close_ogg_file();
}

/***************************************************************************
* Implement sliding windows technique to read OGG data via vorbis library
* and feed it to OpenAL. JN, 15SEP20
***************************************************************************/
static void stream_music(void)
{
	ALint stat;

	/*
	 * If no more data to read from ogg file. That's it. No more to read.
	 * Do nothing.
	 */
	if (ogg_fp == NULL) {
		/*
		 * If buffers are empty. Stop music.
		 */
		alGetSourcei(primary_source, AL_BUFFERS_PROCESSED, &stat);
		if (stat == NO_OF_MUSIC_BUFFERS) {
			if (verbose_logging == TRUE)
				printf("End of music.\n");
			stop_music();
		}
		return;
	}

	/*
	 * Get source status. If buffers are all full. Do nothing.
	 */
	alGetSourcei(primary_source, AL_BUFFERS_PROCESSED, &stat);
	if (stat == 0)
		return; /*buffers full. Nothing to do*/

	while (stat--) { /*buffers are empty. Fill them*/
		int rs;
		ALuint buf;

		/*
		 * Read a music data block
		 */
		rs = read_ogg_file();
		if (rs <= 0) { /*EOF*/
			close_ogg_file();
			return;
		}

		/*
		 * Get OpenAL to give us an empty buffer entry.
		 * Tell OpenAL to load ogg data into this empty buffer
		 * Attach the buffer to a music source.
		 */
		alSourceUnqueueBuffers(primary_source, 1, &buf);
		alBufferData(buf, AL_FORMAT_STEREO16, ogg_data, rs, ogg_vi->rate);
		alSourceQueueBuffers(primary_source, 1, &buf);
		if ((alerr = alGetError()) != AL_NO_ERROR) {
			sound_warning(__func__, "alSourceQueueBuffers()", alerr);
			return;
		}
	}

	/*
	 * If source has stopped playing due to latency issues. Start
	 * playing it again.
	 */
	alGetSourcei(primary_source, AL_SOURCE_STATE, &stat);
	if (stat != AL_PLAYING) {
		alSourcePlay(primary_source);
		if (verbose_logging == TRUE)
			printf("Warning: Music stopped and had to restart it.\n");
	}
}

/***************************************************************************
* Play music
***************************************************************************/
void play_music(void)
{
	long total_size;
	int i;

	if (verbose_logging == TRUE)
		printf("Playing music file: %s\n", MUSIC_FILEPATH);

	/*
	 * If game has already been played, then primary_source will have
	 * FX buffers binded to it. Unbind them here so that these music\
	 * functions can use this source. JN, 17SEP20
	 */
	alSourcei(primary_source, AL_BUFFER, 0);

	/*
	 * Open music file
	 */
	ogg_fp = fopen(rel_to_abs_filepath(MUSIC_FILEPATH), "r");
	if (ogg_fp == NULL)
		print_error(__func__, "fopen(Rescue_from_Vega.ogg)");

	/*
	 * Plug stdio stream into vorbis decoder and setup
	 */
	if (ov_open_callbacks(ogg_fp, &ogg_vf, NULL,
						0, OV_CALLBACKS_NOCLOSE) < 0)
		printf_error("%s:ov_open_callbacks()", __func__);

	ogg_vi = ov_info(&ogg_vf, -1);
	total_size = ov_pcm_total(&ogg_vf, -1) *
				ogg_vi->channels * MUSIC_16BIT_PRECISION;

	if (verbose_logging == TRUE)
		printf("\tSize: %ld bytes\n"
			"\tRate: %ld Hz\n"
			"\tChan: %u\n",
			total_size, ogg_vi->rate, ogg_vi->channels);

	/*
	 * Create and prime buffers. Start loading blocks of uncompressed
	 * music file and load it into the OpenAL buffers
	 */
	alGenBuffers(NO_OF_MUSIC_BUFFERS, music_buffer);
	if ((alerr = alGetError()) != AL_NO_ERROR) {
		sound_warning(__func__, "alGenBuffers()", alerr);
		return;
	}
	for (i = 0; i < NO_OF_MUSIC_BUFFERS; i++) {
		int rs;

		rs = read_ogg_file();
		if (rs == 0) { /*EOF ?*/
			if (verbose_logging == TRUE) /*JN, 27OCT20*/
				printf("Warning: Unexpected EOF when reading music file.\n");
			break;
		}

		alBufferData(music_buffer[i], AL_FORMAT_STEREO16,
					ogg_data, rs, ogg_vi->rate);
		if ((alerr = alGetError()) != AL_NO_ERROR) {
			sound_warning(__func__, "alBufferData()", alerr);
			return;
		}
	}

	if (verbose_logging == TRUE)
		printf("OpenAL Buffers primed: %d\n", i);

	/*
	 * Attach the buffers to a music source.
	 */
	alSourceQueueBuffers(primary_source, i, music_buffer);

	/*
	 * Start playing music
	 */
	alSourcef(primary_source, AL_GAIN, 1.0);
	alSourcef(primary_source, AL_PITCH, 1.0);
	alSource3f(primary_source, AL_POSITION, 0.0, 0.0, 0.0);
	alSource3f(primary_source, AL_VELOCITY, 0.0, 0.0, 0.0);
	alSourcei(primary_source, AL_LOOPING, AL_FALSE);
	alSourcePlay(primary_source);
	music_playing_f = TRUE;
}

/***************************************************************************
* Load RAW mono 8bit 1ch unsigned 11025Hz FX file. From v2.2. JN, 16SEP20
***************************************************************************/
static void load_raw_fx_file(char *filename, char **data, unsigned int *size)
{
	FILE *fp;

	if (verbose_logging == TRUE)
		printf("Loading RAW FX file: %s\n", filename);

	fp = fopen(rel_to_abs_filepath(filename), "rb");
	if (fp == NULL)
		print_error(__func__, "fopen()");

	fseek(fp, 0, SEEK_END);
	*size = (unsigned int)ftell(fp);
	fseek(fp, 0, SEEK_SET);
	*data = alloc_mem(*size);

	if (fread(*data, *size, 1, fp) != 1)
		print_error(__func__, "fread()");

	fclose(fp);
}

/***************************************************************************
* load FX. These only have to be loaded once. JN, 16SEP20
***************************************************************************/
static void load_fx_sub(ALuint *fx_buffer, char *filename)
{
	char *data;
	unsigned int size;

	load_raw_fx_file(filename, &data, &size);
	alGenBuffers(1, fx_buffer);
	alBufferData(*fx_buffer, AL_FORMAT_MONO8, data, size, 11025);
	free(data);
}

static void load_fx(void)
{
	load_fx_sub(&(fx_buffer_table[FX_SELECT]), "/fx/select.8");
	load_fx_sub(&(fx_buffer_table[FX_WEAPON_RELOAD]), "/fx/weapon_reload.8");
	load_fx_sub(&(fx_buffer_table[FX_SHIP_COMPLETE]), "/fx/ship_complete.8");
	load_fx_sub(&(fx_buffer_table[FX_NO_WEAPON]), "/fx/no_weapon.8");
	load_fx_sub(&(fx_buffer_table[FX_POWER_UP]), "/fx/power_up.8");
	load_fx_sub(&(fx_buffer_table[FX_LOW_SHIELDS]), "/fx/low_shields.8");
	load_fx_sub(&(fx_buffer_table[FX_OPEN_DOOR]), "/fx/door_open.8");
	load_fx_sub(&(fx_buffer_table[FX_EXPLOSION_1]), "/fx/explosion_1.8");
	load_fx_sub(&(fx_buffer_table[FX_EXPLOSION_2]), "/fx/explosion_2.8");
	load_fx_sub(&(fx_buffer_table[FX_DROID_HIT_1]), "/fx/droid_hit_1.8");
	load_fx_sub(&(fx_buffer_table[FX_LASER_LINARITE]), "/fx/laser_linarite.8");
	load_fx_sub(&(fx_buffer_table[FX_LASER_CROC_BENZ]), "/fx/laser_crocoite_benzol.8");
	load_fx_sub(&(fx_buffer_table[FX_LASER_UVAROVITE]), "/fx/laser_uvarovite.8");
	load_fx_sub(&(fx_buffer_table[FX_LASER_TIGER_EYE]), "/fx/laser_tiger_eye.8");
	load_fx_sub(&(fx_buffer_table[FX_LASER_BUZZ]), "/fx/laser_buzz.8");
	load_fx_sub(&(fx_buffer_table[FX_DROID_EVASE]), "/fx/droid_evase.8");
	load_fx_sub(&(fx_buffer_table[FX_6XX_VOICE]), "/fx/6xx_voice.8");
	load_fx_sub(&(fx_buffer_table[FX_7XX_VOICE]), "/fx/7xx_voice.8");
	load_fx_sub(&(fx_buffer_table[FX_8XX_VOICE]), "/fx/8xx_voice.8");
	load_fx_sub(&(fx_buffer_table[FX_TRANS_TERM]), "/fx/trans_terminated.8");
	load_fx_sub(&(fx_buffer_table[FX_RANDOM]), "/fx/rand.8");
	load_fx_sub(&(fx_buffer_table[FX_ALERT_SIREN]), "/fx/alert_siren.8");
	load_fx_sub(&(fx_buffer_table[FX_SHIP_NOISE1]), "/fx/snoise1.raw");
	load_fx_sub(&(fx_buffer_table[FX_SHIP_NOISE2]), "/fx/snoise2.raw");
	load_fx_sub(&(fx_buffer_table[FX_SHIP_NOISE3]), "/fx/snoise3.raw");
	load_fx_sub(&(fx_buffer_table[FX_SHIP_NOISE4]), "/fx/snoise4.raw");
	load_fx_sub(&(fx_buffer_table[FX_SHIP_NOISE5]), "/fx/snoise5.raw");
	load_fx_sub(&(fx_buffer_table[FX_SHIP_NOISE6]), "/fx/snoise6.raw");
	load_fx_sub(&(fx_buffer_table[FX_SHIP_NOISE7]), "/fx/snoise7_science.raw");
	/*New 25SEP20*/
	load_fx_sub(&(fx_buffer_table[FX_DROID_HIT_2]), "/fx/droid_hit_2.8");
	load_fx_sub(&(fx_buffer_table[FX_FLOOR_COMPLETE]), "/fx/floor_complete.8");
	load_fx_sub(&(fx_buffer_table[FX_9XX_VOICE]), "/fx/9xx_voice.8");
}

/****************************************************************************
* Sound cmd queue handling (Note: similar to kevent handling).
* Background processes will with commands on the queue. Foreground (ie:draw())
* process will pull cmds off hte queue and process them. JN, 14SEP20
****************************************************************************/
static sem_t		snd_event_sem;
static snd_event_st	snd_event[SND_EVENT_SIZE];
static int		snd_event_r_ptr, snd_event_w_ptr;

static void init_snd_event(void)
{
	if (sem_init(&snd_event_sem, 0, 1) == -1)
		print_error(__func__, "sem_init()");
}

static void end_snd_event(void)
{
	sem_destroy(&snd_event_sem);
}

void put_snd_event(snd_event_st *e)
{
	if (sound_enabled_f == FALSE)
		return;

	if (sem_wait(&snd_event_sem) == -1)
		print_error(__func__, "sem_wait()");

	memcpy(&snd_event[snd_event_w_ptr], e, sizeof(snd_event_st));
	snd_event_w_ptr++;
	snd_event_w_ptr &= (SND_EVENT_SIZE - 1);

	if (sem_post(&snd_event_sem) == -1)
		print_error(__func__, "sem_post()");
}

static int get_snd_event(snd_event_st *e)
{
	if(snd_event_r_ptr == snd_event_w_ptr)
		return 0;

	if (sem_wait(&snd_event_sem) == -1)
		print_error(__func__, "sem_wait()");

	memcpy(e, &snd_event[snd_event_r_ptr], sizeof(snd_event_st));
	snd_event_r_ptr++;
	snd_event_r_ptr &= (SND_EVENT_SIZE - 1);

	if (sem_post(&snd_event_sem) == -1)
		print_error(__func__, "sem_post()");

	return 1;
}

/***************************************************************************
* This function processes sound commands from the snd_event
*
* The idea of OpenAL is to have sound sources apart of the objects in an
* object orientated game. However, sound in this game is controlled via the
* background context (a signal() timer), and I don't want to put OpenAL in
* this context and I don't want to completely rewrite the classes to
* accomodate OpenGL. So for now, we'll keep a sound fx queue that the
* background context queues fx commands on and the foreground (draw) context
* reads and plays the fx etc. JN, 16SEO20
***************************************************************************/
void sound_process_fg(void)
{
	if (sound_enabled_f == FALSE)
		return;

	if (music_playing_f == TRUE)
		stream_music();

	for (;;) {
		snd_event_st se;

		if (!get_snd_event(&se))
			break;

		switch (se.cmd) {
			case SND_CMD_FX_PRI:
			if (verbose_logging == TRUE)
				printf("Snd Cmd: SND_CMD_FX_PRI (%d)\n", se.fx_no);
			play_primary_source(se.fx_no, se.vol, se.freq);
			break;

			case SND_CMD_FX_PRI_STOP:
			if (verbose_logging == TRUE)
				printf("Snd Cmd: SND_CMD_FX_PRI_STOP\n");
			alSourceStop(primary_source);
			break;

			case SND_CMD_FX_MIXED:
			if (verbose_logging == TRUE)
				printf("Snd Cmd: SND_CMD_FX_MIXED (%d)\n", se.fx_no);
			play_fx_mixed_source(&se);
			break;

			case SND_CMD_FX_MIXED_STOP:
			if (verbose_logging == TRUE)
				printf("Snd Cmd: SND_CMD_FX_MIXED_STOP\n");
			stop_fx_mixed_source();
			break;

			case SND_CMD_FX_SNOISE:
			if (verbose_logging == TRUE)
				printf("Snd Cmd: SND_CMD_FX_SNOISE\n");
			play_ship_noise_source(se.fx_no, se.vol, se.freq);
			break;

			case SND_CMD_FX_SNOISE_STOP:
			if (verbose_logging == TRUE)
				printf("Snd Cmd: SND_CMD_FX_SNOISE_STOP\n");
			alSourceStop(ship_noise_source);
			break;

			case SND_CMD_RANDOM:
			if (verbose_logging == TRUE)
				printf("Snd Cmd: SND_CMD_RANDOM\n");
			play_primary_source_loop(FX_RANDOM, 0.2);
			break;

			case SND_CMD_FX_STOP_ALL:
			if (verbose_logging == TRUE)
				printf("Snd Cmd: SND_CMD_FX_STOP_ALL\n");
			alSourceStop(primary_source);
			alSourceStop(ship_noise_source);
			stop_fx_mixed_source();
			break;

			case SND_CMD_PLAY_SONG:
			if (verbose_logging == TRUE)
				printf("Snd Cmd: SND_CMD_PLAY_SONG\n");
			play_music();
			break;

			case SND_CMD_STOP_SONG:
			if (verbose_logging == TRUE)
				printf("Snd Cmd: SND_CMD_STOP_SONG\n");
			stop_music();
			break;
		}
	}
}

/***************************************************************************
* Sound clean up. This has to handle variables even if they have not been
* initialised. JN, 15SEP20
***************************************************************************/
void shutdown_sound(void)
{
	if (verbose_logging == TRUE)
		printf("Shutting down the OpenAL sound system..\n");

	end_snd_event();

	/*
	 * Stop and unbind all sources (if they are on). JN, 18SEP20
	 */
	alSourceStop(primary_source);
	alSourcei(primary_source, AL_BUFFER, 0);

	alSourceStop(ship_noise_source);
	alSourcei(ship_noise_source, AL_BUFFER, 0);

	for (int i = 0; i < NO_OF_FX_MIXED_SOURCES; i++) {
		alSourceStop(fx_mixed_source[i]);
		alSourcei(fx_mixed_source[i], AL_BUFFER, 0);
	}

	/*
	 * Deallocated/delete/destroy etc. JN, 18SEP20
	 */
	alDeleteSources(1, &primary_source);
	alDeleteSources(1, &ship_noise_source);
	alDeleteSources(NO_OF_FX_MIXED_SOURCES, fx_mixed_source);
	alDeleteBuffers(NO_OF_MUSIC_BUFFERS, music_buffer);
	alDeleteBuffers(MAX_FX_BUFFER_ENTRIES, fx_buffer_table);

	al_device = alcGetContextsDevice(al_context);
	alcMakeContextCurrent(NULL);
	alcDestroyContext(al_context);
	alcCloseDevice(al_device);
}

/***************************************************************************
*
***************************************************************************/
void init_sound(void)
{
	if (verbose_logging == TRUE)
		printf("Initing the OpenAL sound system..\n");

	init_snd_event();

	/*
	 * Create an OpenAL device. If this function fails, then we have no
	 * soundcard ?. This is the only OpenAL that will fail and not end
	 * the program. All functions that fail from now on are assumed to
	 * be fatal errors and will end program. JN, 14SEP20
	 */
	al_device = alcOpenDevice(NULL);
	if (al_device == NULL) {
		printf(
"Warning: Your system appears to have no sound capability ??. "
"Game will play with no sound.\n");
		sound_enabled_f = FALSE;
		return;
	}

	alGetError(); /*Dummy reset*/

	al_context = alcCreateContext(al_device, NULL);
	if (al_context == NULL) {
		sound_warning(__func__, ":alcCreateContext()", 0);
		sound_enabled_f = FALSE;
		return;
	}

	alcMakeContextCurrent(al_context);

	/*
	 * Setup listener (there is only one listener)
	 */
	alListener3f(AL_POSITION, 0.0, 0.0, 0.0);
	alListener3f(AL_VELOCITY, 0.0, 0.0, 0.0);
	alListenerfv(AL_ORIENTATION, listener_orient);

	/*
	 * Setup primary FX source. This is for mutually exclusive sound
	 * like music, transport select FX sound, random noise sound when you
	 * die, siren alert sound when you start game etc. This only has to be
	 * done once. JN, 16SEP20
	 */
	alGenSources(1, &primary_source);
	if ((alerr = alGetError()) != AL_NO_ERROR) {
		sound_warning(__func__, "alGenSources(primary_source)", alerr);
		return;
	}
	alSource3f(primary_source, AL_POSITION, 0.0, 0.0, 0.0);
	alSource3f(primary_source, AL_VELOCITY, 0.0, 0.0, 0.0);

	/*
	 * Ship noise source. This needs its own source. JN, 17SEP20
	 */
	alGenSources(1, &ship_noise_source);
	if ((alerr = alGetError()) != AL_NO_ERROR) {
		sound_warning(__func__, "alGenSources(ship_noise_source)", alerr);
		return;
	}
	alSource3f(ship_noise_source, AL_POSITION, 0.0, 0.0, 0.0);
	alSource3f(ship_noise_source, AL_VELOCITY, 0.0, 0.0, 0.0);
	alSourcei(ship_noise_source, AL_LOOPING, AL_TRUE);

	/*
	 * Create our mixed FX sources. These are mixed/simultaneous FX
	 * sounds. JN, 17SEP20
	 */
	alGenSources(NO_OF_FX_MIXED_SOURCES, fx_mixed_source);
	if ((alerr = alGetError()) != AL_NO_ERROR) {
		sound_warning(__func__, "alGenSources(fx_mixed_source)", alerr);
		return;
	}
	for (int i = 0; i < NO_OF_FX_MIXED_SOURCES; i++) {
		alSource3f(fx_mixed_source[i], AL_VELOCITY, 0.0, 0.0, 0.0);
		alSourcei(fx_mixed_source[i], AL_SOURCE_RELATIVE, AL_TRUE);
		alSourcef(fx_mixed_source[i],
			AL_REFERENCE_DISTANCE, FX_REFERENCE_DISTANCE);
		alSourcef(fx_mixed_source[i],
			AL_MAX_DISTANCE, FX_MAX_DISTANCE);
		alSourcef(fx_mixed_source[i],
			AL_ROLLOFF_FACTOR, FX_ROLLOFF_FACTOR);
	}

	alDistanceModel(AL_INVERSE_DISTANCE_CLAMPED);

	/*
	 * Load FX buffers
	 */
	load_fx();
}
