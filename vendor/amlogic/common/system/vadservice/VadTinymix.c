#include <tinyalsa/asoundlib.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <getopt.h>
#include <pthread.h>
#include <sys/prctl.h>
//#include <utils/Log.h>
//#include <android/log.h>

static int tinymixSetValue(struct mixer *mixer, const char *control,
                             char **values, unsigned int num_values);

pthread_t threadID;
char thread_exit;

static void *vadThreadloop(void *data)
{
    unsigned int size;
    struct pcm *pcm = (struct pcm *)data;
    char *buffer;

    thread_exit = 0;
    prctl(PR_SET_NAME, (unsigned long)"audio_vad_thread");
    size = pcm_frames_to_bytes(pcm, pcm_get_buffer_size(pcm));
    buffer = malloc(size);

    while (!thread_exit && !pcm_read(pcm, buffer, size)) {
    }
    free(buffer);
    pcm_close(pcm);
    return (void *)0;
}
void vadTinycapStop(void)
{
    thread_exit = 1;
}

int vadTinycapStart(int device, int rate, int channels)
{
    struct pcm_config config;
    struct pcm *pcm;
    unsigned int card = 0;
    //unsigned int frames;
    enum pcm_format format = PCM_FORMAT_S16_LE;
    int ret = 0;

    memset(&config, 0, sizeof(config));
    config.channels = channels;
    config.rate = rate;
    config.period_size = 1024;
    config.period_count = 4;
    config.format = format;

    pcm = pcm_open(card, device, PCM_IN, &config);
    if (!pcm || !pcm_is_ready(pcm)) {
        fprintf(stderr, "Unable to open PCM device (%s)\n", pcm_get_error(pcm));
        return -1;
    }
    ret = pthread_create(&threadID, NULL, &vadThreadloop, pcm);
    if (ret != 0) {
        fprintf(stderr, "%s err\n", __func__);
        goto err;
    }
    return 0;

err:
    pcm_close(pcm);
    return ret;
}

int vadTinymix(char *id, char *val)
{
    struct mixer *mixer;
    int card = 0;
    int ret = 0;

    mixer = mixer_open(card);
    if (!mixer) {
        fprintf(stderr, "Failed to open mixer\n");
        return ENODEV;
    }
    ret = tinymixSetValue(mixer, id, &val, 1);
    mixer_close(mixer);
    return 0;
}

static int tinymixSetValue(struct mixer *mixer, const char *control,
                             char **values, unsigned int num_values)
{
    struct mixer_ctl *ctl;
    enum mixer_ctl_type type;
    unsigned int num_ctl_values;
    unsigned int i;

    if (isdigit(control[0]))
        ctl = mixer_get_ctl(mixer, atoi(control));
    else
        ctl = mixer_get_ctl_by_name(mixer, control);

    if (!ctl) {
        fprintf(stderr, "Invalid mixer control: %s\n", control);
        return ENOENT;
    }

    type = mixer_ctl_get_type(ctl);
    num_ctl_values = mixer_ctl_get_num_values(ctl);

    if (type == MIXER_CTL_TYPE_BYTE) {
        return ENOENT;
    }

    if (isdigit(values[0][0])) {
        if (num_values == 1) {
            /* Set all values the same */
            int value = atoi(values[0]);

            for (i = 0; i < num_ctl_values; i++) {
                if (mixer_ctl_set_value(ctl, i, value)) {
                    fprintf(stderr, "Error: invalid value\n");
                    return EINVAL;
                }
            }
        } else {
            /* Set multiple values */
            if (num_values > num_ctl_values) {
                fprintf(stderr,
                        "Error: %u values given, but control only takes %u\n",
                        num_values, num_ctl_values);
                return EINVAL;
            }
            for (i = 0; i < num_values; i++) {
                if (mixer_ctl_set_value(ctl, i, atoi(values[i]))) {
                    fprintf(stderr, "Error: invalid value for index %d\n", i);
                    return EINVAL;
                }
            }
        }
    } else {
        if (type == MIXER_CTL_TYPE_ENUM) {
            if (num_values != 1) {
                fprintf(stderr, "Enclose strings in quotes and try again\n");
                return EINVAL;
            }
            if (mixer_ctl_set_enum_by_string(ctl, values[0])) {
                fprintf(stderr, "Error: invalid enum value\n");
                return EINVAL;
            }
        } else {
            fprintf(stderr, "Error: only enum types can be set with strings\n");
            return EINVAL;
        }
    }

    return 0;
}

