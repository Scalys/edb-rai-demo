#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

#include "host.h"
#include "audio.h"
#include "misc.h"

#include "edb_rai_model.h"

int g_args_debug = 0;
int g_args_help  = 0;

static void usage()
{
    printf("Usage: edb-rai-demo [OPTION]... TA DEVICE\n\
OpenEnclave confidential computing demo with protected RealityAI ML algorithm.\n\
\n\
Provided the Alsa device handle, this demo listens for an audio from a USB\n\
microphone and recognizes following modes: silence, keyboard typing, human voice.\n\
\n\
Note: to find the expected Alsa device handle see the output of 'arecord -l'\n\
command. Then see use the format hw:car,device. E.g.:\n\
  card 3: USB_1 [Jabra SPEAK 410 USB], device 0: USB Audio [USB Audio]\n\
means that expected device would be 'hw:3,0'.\n\
\n\
Options:\n\
  -h, --help              show help\n\
  -d, --debug             enable debug output\n\
");
}


static int init(const char *ta_name, const char *audio_dev)
{
    int ret = create_enclave(ta_name);
    if (ret != 0)
        err("failed to create enclave with result = %d\n", ret);

    audio_init(audio_dev);

    return ret;
}

static int uninit()
{
    int ret = terminate_enclave();
    if (ret != 0)
        err("failed to terminate enclave with result = %d\n", ret);

    audio_deinit();

    return ret;
}

int main(int argc, char *argv[])
{
    edb_rai_AICLASS aic;
    float input_vector[g_input_vector_size];
    const char *alsa_device = 0;
    const char *ta_name = 0;
    int c;
    int ret;

    while (1) {
        static struct option long_opts[] = {
            {"debug", no_argument, &g_args_debug, 1},
            {"help",  no_argument, &g_args_help,  1},
            {0, 0, 0, 0}
        };

        int opt_index = 0;
        c = getopt_long(argc, argv, "hd", long_opts, &opt_index);
        if (c == -1)
            break;

        switch (c)
        {
        case 0:
            if (long_opts[opt_index].flag != 0)
                break;
            printf("option %s", long_opts[opt_index].name);
            if (optarg)
                printf (" with arg %s", optarg);
            printf ("\n");
            break;
        case 'h':
            usage();
            exit(0);
        case 'd':
            g_args_debug = true;
        case '?':
            break;
        default:
            abort();
        }
    }

    if (g_args_help) {
        usage();
        exit(0);
    }
    if (argc != (optind + 2)) {
        usage();
        exit(1);
    }
    ta_name = argv[optind];
    alsa_device = argv[optind + 1];

    init(ta_name, alsa_device);
    while (true) {
        call_enclave();
    }
    uninit();

    return 0;
}
