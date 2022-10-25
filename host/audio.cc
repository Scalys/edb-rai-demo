#include <stdio.h>
#include <alsa/asoundlib.h>

#include "misc.h"
#include "edb_rai_model.h"

#include "audio.h"


size_t g_samples = edb_rai_SAMPLES_PER_CHANNEL;
uint8_t *g_buffer = 0;
size_t g_buffer_size = 0;
size_t g_input_vector_size = g_samples * EDB_RAI_AUDIO_CHANNELS;
float *g_input_vector;
snd_pcm_t *g_capture_handle = 0;


int audio_init(const char *audio_dev)
{
    unsigned int rate = EDB_RAI_AUDIO_RATE;
    snd_pcm_hw_params_t *hw_params;
    snd_pcm_format_t format = EDB_RAI_AUDIO_FORMAT;
    int ret = 0;

    ret = snd_pcm_open(&g_capture_handle, audio_dev, SND_PCM_STREAM_CAPTURE, 0);
    if (ret < 0) {
        err("cannot open audio device %s (%s)\n", audio_dev, snd_strerror(ret));
        goto abort;
    }

    ret = snd_pcm_hw_params_malloc(&hw_params);
    if (ret < 0) {
        err("cannot allocate hardware parameter structure (%s)\n", snd_strerror(ret));
        goto abort;
    }

    ret = snd_pcm_hw_params_any(g_capture_handle, hw_params);
    if (ret < 0) {
        err("cannot initialize hardware parameter structure (%s)\n", snd_strerror(ret));
        goto abort;
    }

    ret = snd_pcm_hw_params_set_access(g_capture_handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED);
    if (ret < 0) {
        err("cannot set access type (%s)\n", snd_strerror(ret));
        goto abort;
    }

    ret = snd_pcm_hw_params_set_format(g_capture_handle, hw_params, format);
    if (ret < 0) {
        err("cannot set sample format (%s)\n", snd_strerror(ret));
        goto abort;
    }

    ret = snd_pcm_hw_params_set_rate_near(g_capture_handle, hw_params, &rate, 0);
    if (ret < 0) {
        err("cannot set sample rate (%s)\n", snd_strerror(ret));
        goto abort;
    }

    ret = snd_pcm_hw_params_set_channels(g_capture_handle, hw_params, EDB_RAI_AUDIO_CHANNELS);
    if (ret < 0) {
        err("cannot set channel count (%s)\n", snd_strerror(ret));
        goto abort;
    }

    ret = snd_pcm_hw_params(g_capture_handle, hw_params);
    if (ret < 0) {
        err("cannot set parameters (%s)\n", snd_strerror(ret));
        goto abort;
    }

    snd_pcm_hw_params_free(hw_params);

    ret = snd_pcm_prepare(g_capture_handle);
    if (ret < 0) {
        err("cannot prepare audio interface for use (%s)\n", snd_strerror(ret));
        goto abort;
    }

    g_buffer_size = g_samples * snd_pcm_format_width(format) / 8 * EDB_RAI_AUDIO_CHANNELS;
    g_buffer = (uint8_t*)malloc(g_buffer_size * sizeof(g_buffer[0]));
    g_input_vector = (float*)malloc(g_input_vector_size * sizeof(g_input_vector[0]));

    printf("buffer size: %d\n", g_buffer_size);
    printf("input vector size: %d\n", g_input_vector_size);

abort:
    return ret;
}

void audio_deinit()
{
    free(g_buffer);
    snd_pcm_close(g_capture_handle);
}

float *get_input_vector()
{
    int ret = 0;

    ret = snd_pcm_readi(g_capture_handle, g_buffer, g_samples);
    if (ret != g_samples) {
        err("read from audio interface failed (%s)\n",  snd_strerror(ret));
        return 0;
    }

    int16_t val;
    float f;
    for (int i = 0; i < g_samples * EDB_RAI_AUDIO_CHANNELS; i++) {
        val = (int16_t)g_buffer[i * 2] + ((int16_t)g_buffer[i * 2 + 1]) * 256;
        f = ((float) val) / (float) 32768;
        if (f > 1)
            f = 1;
        else if (f < -1)
            f = -1;
        g_input_vector[i] = f;
    }

    return g_input_vector;
}
