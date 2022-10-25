#ifndef AUDIO_H_
#define AUDIO_H_


#define EDB_RAI_AUDIO_RATE 16000
#define EDB_RAI_AUDIO_FORMAT (SND_PCM_FORMAT_S16_LE)
#define EDB_RAI_AUDIO_CHANNELS (edb_rai_NUM_CHANNELS)

extern size_t g_input_vector_size;
extern size_t g_samples;

int audio_init(const char *audio_dev);
void audio_deinit();
float *get_input_vector();


#endif // AUDIO_H_
