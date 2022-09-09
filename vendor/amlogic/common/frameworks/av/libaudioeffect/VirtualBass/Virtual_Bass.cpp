/*
 * Copyright (C) 2017 Amlogic Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *  DESCRIPTION
 *      This file implements a special EQ  from Amlogic.
 *
 */
#define LOG_TAG "Virtual_Bass_Effect"
#define LOG_NDEBUG 0

#include <cutils/log.h>
#include <utils/Log.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdio.h>
#include <unistd.h>

#include <hardware/audio_effect.h>
#include <cutils/properties.h>

#include "IniParser.h"
#include "Virtual_Bass.h"

extern "C"{

#include "Virtual_Bass_Arithmetic.h"

#define MODEL_SUM_DEFAULT_PATH "/vendor/etc/tvconfig/model/model_sum.ini"
#define AUDIO_EFFECT_DEFAULT_PATH "/vendor/etc/tvconfig/audio/AMLOGIC_AUDIO_EFFECT_t962x_r311_FHD.ini"

// effect_handle_t interface implementation for Virtual_Bass effect
extern const struct effect_interface_s VirtualBassInterface;

//Avl effect TYPE: 4a959f5c-e33a-4df2-8c3f-3066f9275edf
//cvteAvl effect TYPE: 4dbd5e70-567f-4545-8c45-e12165e50267
//Avl effect UUID: 08246a2a-b2d3-4621-b804-42c9b478eb9d
//cvteAvl effect UUID: a7eb1f3d-2c99-4664-8593-19405951e302
const effect_descriptor_t VirtualBassDescriptor={
       //{0x4a959f5c,0xe33a,0x4df2,0x8c3f,{0x30,0x66,0xf9,0x27,0x5e,0xdf}},
       //{0x08246a2a,0xb2d3,0x4621,0xb804,{0x42,0xc9,0xb4,0x78,0xeb,0x9d}},
       {0x4dbd5e70,0x567f,0x4545,0x8c45,{0xe1,0x21,0x65,0xe5,0x02,0x67}},
       {0xa7eb1f3d,0x2c99,0x4664,0x8593,{0x19,0x40,0x59,0x51,0xe3,0x02}},
        EFFECT_CONTROL_API_VERSION,
        EFFECT_FLAG_TYPE_POST_PROC | EFFECT_FLAG_DEVICE_IND | EFFECT_FLAG_NO_PROCESS | EFFECT_FLAG_OFFLOAD_SUPPORTED,
        AVL_CUP_LOAD_ARM9E,
        AVL_MEM_USAGE,
        "VirtualBass",
        "Amlogic",
};

enum Virtual_Bass_state_e {
    VIRTUAL_BASS_STATE_UNINITIALIZED,
    VIRTUAL_BASS_STATE_INITIALIZED,
    VIRTUAL_BASS_STATE_ACTIVE,
};

/*parameter number passed from the Java layer corresponds to this*/
typedef enum {
    VIRTUAL_BASS_PARAM_ENABLE,
    VIRTUAL_BASS_PARAM_PEAK_LEVEL,
    VIRTUAL_BASS_PARAM_DYNAMIC_THRESHOLD,
    VIRTUAL_BASS_PARAM_NOISE_THRESHOLD,
    VIRTUAL_BASS_PARAM_RESPONSE_TIME,
    VIRTUAL_BASS_PARAM_RELEASE_TIME,
    VIRTUAL_BASS_PARAM_SOURCE_IN,

} VirtualBassparams;

typedef struct VirtualBasscfg_s{
    /*in dB*/
    float peak_level;
    float dynamic_threshold;
    float noise_threshold;
    /*in ms*/
    int response_time;
    /*in ms*/
    int release_time;
} VirtualBasscfg;

typedef struct VirtualBassdata_s {
    /* Virtual_Bass API needed data */
    void        *agc;
    VirtualBasscfg      tbcfg;
    int32_t     enable;
    int32_t     *usr_cfg;
    int32_t     source_num;
    int32_t     param_num;
    int32_t     soure_id;
} VirtualBassdata;

typedef struct VirtualBassContext_s {
    const struct effect_interface_s *itfe;
    effect_config_t                 config;
    Virtual_Bass_state_e            state;
    VirtualBassdata                 gVirtualBassdata;
} VirtualBassContext;

const char *VirtualBassStatusstr[] = {"Disable", "Enable"};

int Virtual_Bass_get_model_name(char *model_name, int size)
{
    int ret = -1;
    char node[PROPERTY_VALUE_MAX];

    ret = property_get("tv.model_name", node, NULL);

    if (ret < 0)
        snprintf(model_name, size, "DEFAULT");
    else
        snprintf(model_name, size, "%s", node);

    ALOGD("%s: Model Name -> %s", __FUNCTION__, model_name);
    return ret;
}

int Virtual_Bass_get_ini_file(char *ini_name, int size)
{
    int result = -1;
    char model_name[50] = {0};
    IniParser* pIniParser = NULL;
    const char *ini_value = NULL;
    const char *filename = MODEL_SUM_DEFAULT_PATH;

    Virtual_Bass_get_model_name(model_name, sizeof(model_name));
    pIniParser = new IniParser();
    if (pIniParser->parse(filename) < 0) {
        ALOGW("%s: Load INI file -> %s Failed", __FUNCTION__, filename);
        goto exit;
    }

    ini_value = pIniParser->GetString(model_name, "AMLOGIC_AUDIO_EFFECT_INI_PATH", AUDIO_EFFECT_DEFAULT_PATH);
    if (ini_value == NULL || access(ini_value, F_OK) == -1) {
        ALOGD("%s: INI File is not exist", __FUNCTION__);
        goto exit;
    }

    ALOGD("%s: INI File -> %s", __FUNCTION__, ini_value);
    strncpy(ini_name, ini_value, size);

    result = 0;
exit:
    delete pIniParser;
    pIniParser = NULL;
    return result;
}

int Virtual_Bass_parse_mode_config(VirtualBassContext *pContext, int param_num, const char *buffer)
{
    int i;
    char *Rch = (char *)buffer;
    VirtualBassdata *data = &pContext->gVirtualBassdata;

    if (data->usr_cfg == NULL) {
        data->usr_cfg = (int *)calloc(param_num, sizeof(int));
        if (!data->usr_cfg) {
            ALOGE("%s: alloc failed", __FUNCTION__);
            return -EINVAL;
        }
    }

    for (i = 0; i < param_num; i++) {
        if (i == 0)
            Rch = strtok(Rch, ",");
        else
            Rch = strtok(NULL, ",");
        if (Rch == NULL) {
            ALOGE("%s: Config Parse failed, using default config", __FUNCTION__);
            return -1;
        }
        data->usr_cfg[i] = atoi(Rch);
    }

    if (param_num != 5)
        return -1;

    pContext->gVirtualBassdata.tbcfg.peak_level = data->usr_cfg[0];
    pContext->gVirtualBassdata.tbcfg.dynamic_threshold = data->usr_cfg[1];
    pContext->gVirtualBassdata.tbcfg.noise_threshold = data->usr_cfg[2];
    pContext->gVirtualBassdata.tbcfg.response_time = data->usr_cfg[3];
    pContext->gVirtualBassdata.tbcfg.release_time = data->usr_cfg[4];

    return 0;
}

int Virtual_Bass_load_ini_file(VirtualBassContext *pContext)
{
    int result = -1;
    char ini_name[100] = {0};
    const char *ini_value = NULL;
    VirtualBassdata *data = &pContext->gVirtualBassdata;
    IniParser* pIniParser = NULL;

    if (Virtual_Bass_get_ini_file(ini_name, sizeof(ini_name)) < 0)
        goto error;

    pIniParser = new IniParser();
    if (pIniParser->parse((const char *)ini_name) < 0) {
        ALOGD("%s: %s load failed", __FUNCTION__, ini_name);
        goto error;
    }

    ini_value = pIniParser->GetString("VirtualBass", "VirtualBass_enable", "0");
    if (ini_value == NULL)
        goto error;
    ALOGD("%s: enable -> %s", __FUNCTION__, ini_value);
    data->enable = atoi(ini_value);

    ini_value = pIniParser->GetString("VirtualBass", "virtualbass_paramnum", "5");
    if (ini_value == NULL)
        goto error;
    ALOGD("%s: sound band num -> %s", __FUNCTION__, ini_value);
    data->param_num = atoi(ini_value);

    ini_value = pIniParser->GetString("VirtualBass", "virtualbass_config", "NULL");
    if (ini_value == NULL)
         goto error;
    ALOGD("%s: condig -> %s", __FUNCTION__, ini_value);

    result = Virtual_Bass_parse_mode_config(pContext, data->param_num, ini_value);
    if (result != 0)
        goto error;

    ALOGD("%s: %s", __FUNCTION__, result == 0 ? "sucessful" : "failed");
    delete pIniParser;
    pIniParser = NULL;

    return 0;

error:
    ALOGD("%s: %s", __FUNCTION__, result == 0 ? "sucessful" : "failed");
    delete pIniParser;
    pIniParser = NULL;

    /*parser ini fail, use default value*/
    pContext->gVirtualBassdata.tbcfg.peak_level = -12;
    pContext->gVirtualBassdata.tbcfg.dynamic_threshold = -12;
    pContext->gVirtualBassdata.tbcfg.noise_threshold = -60;
    pContext->gVirtualBassdata.tbcfg.response_time = 100;
    pContext->gVirtualBassdata.tbcfg.release_time = 3000;

    return result;
}

int Virtual_Bass_init(VirtualBassContext *pContext)
{
    VirtualBassdata *data = &pContext->gVirtualBassdata;

    pContext->config.inputCfg.accessMode = EFFECT_BUFFER_ACCESS_READ;
    pContext->config.inputCfg.channels = AUDIO_CHANNEL_OUT_STEREO;
    pContext->config.inputCfg.format = AUDIO_FORMAT_PCM_16_BIT;
    pContext->config.inputCfg.samplingRate = 48000;
    pContext->config.inputCfg.bufferProvider.getBuffer = NULL;
    pContext->config.inputCfg.bufferProvider.releaseBuffer = NULL;
    pContext->config.inputCfg.bufferProvider.cookie = NULL;
    pContext->config.inputCfg.mask = EFFECT_CONFIG_ALL;
    pContext->config.outputCfg.accessMode = EFFECT_BUFFER_ACCESS_ACCUMULATE;
    pContext->config.outputCfg.channels = AUDIO_CHANNEL_OUT_STEREO;
    pContext->config.outputCfg.format = AUDIO_FORMAT_PCM_16_BIT;
    pContext->config.outputCfg.samplingRate = 48000;
    pContext->config.outputCfg.bufferProvider.getBuffer = NULL;
    pContext->config.outputCfg.bufferProvider.releaseBuffer = NULL;
    pContext->config.outputCfg.bufferProvider.cookie = NULL;
    pContext->config.outputCfg.mask = EFFECT_CONFIG_ALL;

    if (data->agc == NULL) {
        VirtualBassInit();
        VirtualBassSetParameter();

        ALOGI("Virtual_Bass_init! peak_level = %fdB, dynamic_theshold = %fdB, noise_threshold = %fdB\n",
            data->tbcfg.peak_level, data->tbcfg.dynamic_threshold, data->tbcfg.noise_threshold);
    } else {
        ALOGE("%s, Virtual_Bass is exist\n", __FUNCTION__);
    }

    ALOGD("%s: sucessful", __FUNCTION__);
    return 0;
}

/*Set I/O configuration: sample rate, channel, format, etc.*/
int Virtual_Bass_configure(VirtualBassContext *pContext, effect_config_t *pConfig)
{
    if (pConfig->inputCfg.samplingRate != pConfig->outputCfg.samplingRate)
        return -EINVAL;
    if (pConfig->inputCfg.channels != pConfig->outputCfg.channels)
        return -EINVAL;
    if (pConfig->inputCfg.format != pConfig->outputCfg.format)
        return -EINVAL;
    if (pConfig->inputCfg.channels != AUDIO_CHANNEL_OUT_STEREO) {
        ALOGW("%s: channels in = 0x%x channels out = 0x%x", __FUNCTION__,
            pConfig->inputCfg.channels, pConfig->outputCfg.channels);
        pConfig->inputCfg.channels = pConfig->outputCfg.channels = AUDIO_CHANNEL_OUT_STEREO;
    }
    if (pConfig->outputCfg.accessMode != EFFECT_BUFFER_ACCESS_WRITE &&
        pConfig->outputCfg.accessMode != EFFECT_BUFFER_ACCESS_ACCUMULATE)
        return -EINVAL;
    if (pConfig->inputCfg.format != AUDIO_FORMAT_PCM_16_BIT) {
        ALOGW("%s: format in = 0x%x format out = 0x%x", __FUNCTION__,
            pConfig->inputCfg.format, pConfig->outputCfg.format);
        pConfig->inputCfg.format = pConfig->outputCfg.format = AUDIO_FORMAT_PCM_16_BIT;
    }
    memcpy(&pContext->config, pConfig, sizeof(effect_config_t));

    return 0;
}

int Virtual_Bass_setParameter(VirtualBassContext *pContext, void *pParam, void *pValue)
{
    int32_t param = *(int32_t *)pParam;
    int32_t value;
    VirtualBassdata *data=&pContext->gVirtualBassdata;
    VirtualBasscfg *tbcfg=&data->tbcfg;

    switch (param) {
       case VIRTUAL_BASS_PARAM_PEAK_LEVEL:
            value = *(int32_t *)pValue;
            tbcfg->peak_level = (float)value;
            VirtualBassSetParameter();
            ALOGD("%s: set peak_level -> %f ", __FUNCTION__, tbcfg->peak_level);
            break;
       case VIRTUAL_BASS_PARAM_DYNAMIC_THRESHOLD:
            value = *(int32_t *)pValue;
            tbcfg->dynamic_threshold = (float)value;
            VirtualBassSetParameter();
            ALOGD("%s: set dynamic_threshold -> %f ", __FUNCTION__, tbcfg->dynamic_threshold);
            break;
       case VIRTUAL_BASS_PARAM_NOISE_THRESHOLD :
            value = *(int32_t *)pValue;
            tbcfg->noise_threshold = (float)value;
            VirtualBassSetParameter();
            ALOGD("%s: set noise_threshold -> %f ", __FUNCTION__, tbcfg->noise_threshold);
            break;
       case VIRTUAL_BASS_PARAM_RESPONSE_TIME:
            value = *(int32_t *)pValue;
            tbcfg->response_time = value / 48; // UI set sample
            VirtualBassSetParameter();
            ALOGD("%s: set response_time-> %d ", __FUNCTION__, tbcfg->response_time);
            break;
       case VIRTUAL_BASS_PARAM_ENABLE:
            value = *(int32_t *)pValue;
            data->enable = value;
            ALOGD("%s: Set status -> %s", __FUNCTION__, VirtualBassStatusstr[value]);
            break;
       case VIRTUAL_BASS_PARAM_RELEASE_TIME:
            value = *(int32_t *)pValue;
            tbcfg->release_time = value * 1000; // UI set s, here change s to ms
            VirtualBassSetParameter();
            ALOGD("%s: set release_time-> %d ", __FUNCTION__, tbcfg->release_time);
            break;
       case VIRTUAL_BASS_PARAM_SOURCE_IN:
            value = *(int32_t *)pValue;
            if (value < 0) {
                ALOGE("%s: incorrect mode value %d", __FUNCTION__, value);
                return -EINVAL;
            }
            data->soure_id = value;
            ALOGD("%s: Set source_id -> %d", __FUNCTION__, value);
            break;
       default:
            ALOGE("%s: unknown param %08x", __FUNCTION__, param);
            return -EINVAL;
       }
    return 0;
}

int Virtual_Bass_getParameter(VirtualBassContext*pContext, void *pParam, size_t *pValueSize, void *pValue)
{
    int32_t param = *(int32_t *)pParam;
    int32_t value;
    VirtualBassdata *data=&pContext->gVirtualBassdata;
    VirtualBasscfg *tbcfg=&data->tbcfg;

    switch (param) {
    case VIRTUAL_BASS_PARAM_PEAK_LEVEL:
        if (*pValueSize < sizeof(int32_t)) {
            *pValueSize = 0;
            return -EINVAL;
        }
        *(int*)pValue = (int)tbcfg->peak_level;
        ALOGD("%s: Get peak_level -> %f ", __FUNCTION__, tbcfg->peak_level);
        break;

    case VIRTUAL_BASS_PARAM_DYNAMIC_THRESHOLD:
        if (*pValueSize < sizeof(int32_t)) {
            *pValueSize = 0;
            return -EINVAL;
        }
        *(float*)pValue = tbcfg->dynamic_threshold;
        ALOGD("%s: Get dynamic_threshold -> %f ", __FUNCTION__, tbcfg->dynamic_threshold);
        break;

    case VIRTUAL_BASS_PARAM_NOISE_THRESHOLD:
        if (*pValueSize < sizeof(int32_t)) {
            *pValueSize = 0;
            return -EINVAL;
        }
        *(float*)pValue = tbcfg->noise_threshold;
        ALOGD("%s: Get noise_threshold -> %f ", __FUNCTION__, tbcfg->noise_threshold);
        break;

    case VIRTUAL_BASS_PARAM_RESPONSE_TIME:
        if (*pValueSize < sizeof(int32_t)) {
            *pValueSize = 0;
            return -EINVAL;
        }
        *(int32_t *) pValue = tbcfg->response_time * 48; // UI set sample
        ALOGD("%s: Get response_time-> %d ", __FUNCTION__, tbcfg->response_time);
        break;

    case VIRTUAL_BASS_PARAM_ENABLE:
        if (*pValueSize < sizeof(int32_t)) {
            *pValueSize = 0;
            return -EINVAL;
        }
        value = data->enable;
        *(int32_t *) pValue = value;
        ALOGD("%s: Get status -> %s", __FUNCTION__, VirtualBassStatusstr[value]);
        break;
    case VIRTUAL_BASS_PARAM_RELEASE_TIME:
        if (*pValueSize < sizeof(int32_t)) {
            *pValueSize = 0;
            return -EINVAL;
        }
        *(int32_t *) pValue = tbcfg->release_time / 1000; // UI get s, here change ms to s
        ALOGD("%s: Get release_time-> %d ", __FUNCTION__, tbcfg->release_time);
        break;
    case VIRTUAL_BASS_PARAM_SOURCE_IN:
        if (*pValueSize < sizeof(int32_t)) {
            *pValueSize = 0;
            return -EINVAL;
        }
        value = data->soure_id;
        *(int32_t *) pValue = value;
        ALOGD("%s: Get aource_id -> %d", __FUNCTION__, value);
        break;
    default:
        ALOGE("%s: unknown param %d", __FUNCTION__, param);
        return -EINVAL;
    }
    return 0;
}

int Virtual_Bass_release(VirtualBassContext *pContext)
{
    VirtualBassdata *data = &pContext->gVirtualBassdata;

    if (data->usr_cfg != NULL) {
        free(data->usr_cfg);
        data->usr_cfg = NULL;
    }

    return 0;
}

//-------------------Effect Control Interface Implementation--------------------------

int Virtual_Bass_process(effect_handle_t self, audio_buffer_t *inBuffer, audio_buffer_t *outBuffer)
{
    VirtualBassContext* pContext = ( VirtualBassContext *)self;
ALOGD("%s   ", __FUNCTION__);
    if (pContext == NULL)
        return -EINVAL;

    if (inBuffer == NULL || inBuffer->raw == NULL ||
        outBuffer == NULL || outBuffer->raw == NULL ||
        inBuffer->frameCount != outBuffer->frameCount ||
        inBuffer->frameCount == 0)
        return -EINVAL;
    if (pContext->state != VIRTUAL_BASS_STATE_ACTIVE)
        return -ENODATA;

    int16_t *in  = (int16_t *)inBuffer->raw;
    int16_t *out = (int16_t *)outBuffer->raw;
    VirtualBassdata*data = &pContext->gVirtualBassdata;

    if (!data->enable) {
        for (size_t i = 0; i < inBuffer->frameCount; i++) {
            *out++ = *in++;
            *out++ = *in++;
        }
				ALOGD("%s: enable ", __FUNCTION__);
    } else {
        VirtualBassProcess(data->agc);
			ALOGD("%s: disable ", __FUNCTION__);
    }

    return 0;
}

int Virtual_Bass_command(effect_handle_t self, uint32_t cmdCode, uint32_t cmdSize,
        void *pCmdData, uint32_t *replySize, void *pReplyData)
{
    VirtualBassContext* pContext = (VirtualBassContext *)self;
    effect_param_t *p;
    int voffset;
    if (pContext == NULL || pContext->state == VIRTUAL_BASS_STATE_UNINITIALIZED) {
        return -EINVAL;
    }
    ALOGD("%s: cmd = %u", __FUNCTION__, cmdCode);
    switch (cmdCode) {
    case EFFECT_CMD_INIT:
        if (pReplyData == NULL || replySize == NULL || *replySize != sizeof(int)) {
            return -EINVAL;
        }
        *(int *) pReplyData = Virtual_Bass_init(pContext);
        break;
    case EFFECT_CMD_SET_CONFIG:
        if (pCmdData == NULL || cmdSize != sizeof(effect_config_t) ||
            pReplyData == NULL || replySize == NULL || *replySize != sizeof(int))
            return -EINVAL;
        *(int *) pReplyData = Virtual_Bass_configure(pContext, (effect_config_t *) pCmdData);
        break;
    case EFFECT_CMD_RESET:
        break;
    case EFFECT_CMD_ENABLE:
        if (pReplyData == NULL || replySize == NULL || *replySize != sizeof(int))
            return -EINVAL;
        if (pContext->state != VIRTUAL_BASS_STATE_INITIALIZED)
            return -ENOSYS;
        pContext->state = VIRTUAL_BASS_STATE_ACTIVE;
        *(int *)pReplyData = 0;
        break;
    case EFFECT_CMD_DISABLE:
        if (pReplyData == NULL || replySize == NULL || *replySize != sizeof(int))
            return -EINVAL;
        if (pContext->state != VIRTUAL_BASS_STATE_ACTIVE)
            return -ENOSYS;
        pContext->state = VIRTUAL_BASS_STATE_INITIALIZED;
        *(int *)pReplyData = 0;
        break;
    case EFFECT_CMD_GET_PARAM:
        if (pCmdData == NULL ||
            cmdSize != (int)(sizeof(effect_param_t) + sizeof(uint32_t)) ||
            pReplyData == NULL || replySize == NULL ||
            *replySize < (int)(sizeof(effect_param_t) + sizeof(uint32_t) + sizeof(uint32_t)))
            return -EINVAL;
        p = (effect_param_t *)pCmdData;
        memcpy(pReplyData, pCmdData, sizeof(effect_param_t) + p->psize);
        p = (effect_param_t *)pReplyData;

        voffset = ((p->psize - 1) / sizeof(int32_t) + 1) * sizeof(int32_t);

        p->status = Virtual_Bass_getParameter(pContext, p->data, (size_t  *)&p->vsize, p->data + voffset);
        *replySize = sizeof(effect_param_t) + voffset + p->vsize;
        break;
    case EFFECT_CMD_SET_PARAM:
        if (pCmdData == NULL ||
            cmdSize != (int)(sizeof(effect_param_t) + sizeof(uint32_t) + sizeof(uint32_t)) ||
            pReplyData == NULL || replySize == NULL || *replySize != sizeof(int32_t))
            return -EINVAL;
        p = (effect_param_t *)pCmdData;
        if (p->psize != sizeof(uint32_t) || p->vsize != sizeof(uint32_t)) {
            *(int32_t *)pReplyData = -EINVAL;
            break;
        }
        *(int *)pReplyData = Virtual_Bass_setParameter(pContext, (void *)p->data, p->data + p->psize);
        break;
    case EFFECT_CMD_OFFLOAD:
        *(int *)pReplyData = 0;
        break;
    case EFFECT_CMD_SET_DEVICE:
    case EFFECT_CMD_SET_VOLUME:
    case EFFECT_CMD_SET_AUDIO_MODE:
        break;
    default:
        ALOGE("%s: invalid command %d", __FUNCTION__, cmdCode);
        return -EINVAL;
    }

    return 0;

}

int Virtual_Bass_getDescriptor(effect_handle_t self, effect_descriptor_t *pDescriptor)
{
    VirtualBassContext * pContext = ( VirtualBassContext *) self;

    if (pContext == NULL || pDescriptor == NULL) {
        ALOGE("%s: invalid param", __FUNCTION__);
        return -EINVAL;
    }

    *pDescriptor = VirtualBassDescriptor;

    return 0;
}

//-------------------- Effect Library Interface Implementation------------------------

int VirtualBassLib_Create(const effect_uuid_t * uuid, int32_t sessionId __unused, int32_t ioId __unused, effect_handle_t * pHandle)
{
    if (pHandle == NULL || uuid == NULL) {
        return -EINVAL;
    }

    if (memcmp(uuid, &VirtualBassDescriptor.uuid, sizeof(effect_uuid_t)) != 0) {
        return -EINVAL;
    }

    VirtualBassContext *pContext = new VirtualBassContext;
    if (!pContext) {
        ALOGE("%s: alloc Virtual_BassContext failed", __FUNCTION__);
        return -EINVAL;
    }
    memset(pContext, 0, sizeof(VirtualBassContext));
    if (Virtual_Bass_load_ini_file(pContext) < 0) {
        ALOGE("%s: Load INI File faied, use default param", __FUNCTION__);
        pContext->gVirtualBassdata.enable = 0;
    }

    pContext->itfe = &VirtualBassInterface;
    pContext->state = VIRTUAL_BASS_STATE_UNINITIALIZED;

    *pHandle = (effect_handle_t)pContext;

    pContext->state = VIRTUAL_BASS_STATE_INITIALIZED;

    ALOGD("%s: %p", __FUNCTION__, pContext);

    return 0;
}

int VirtualBassLib_Release(effect_handle_t handle)
{
    VirtualBassContext * pContext = (VirtualBassContext *)handle;

    if (pContext == NULL) {
        return -EINVAL;
    }
    Virtual_Bass_release(pContext);
    pContext->state = VIRTUAL_BASS_STATE_UNINITIALIZED;
    delete pContext;

    return 0;
}

int VirtualBassLib_GetDescriptor(const effect_uuid_t *uuid, effect_descriptor_t *pDescriptor)
{
    if (pDescriptor == NULL || uuid == NULL) {
        ALOGE("%s: called with NULL pointer", __FUNCTION__);
        return -EINVAL;
    }

    if (memcmp(uuid, &VirtualBassDescriptor.uuid, sizeof(effect_uuid_t)) == 0) {
        *pDescriptor = VirtualBassDescriptor;
        return 0;
    }

    return  -EINVAL;
}

// effect_handle_t interface implementation for VirtualBassInterface effect

const struct effect_interface_s VirtualBassInterface = {
        Virtual_Bass_process,
        Virtual_Bass_command,
        Virtual_Bass_getDescriptor,
        NULL,
};

audio_effect_library_t AUDIO_EFFECT_LIBRARY_INFO_SYM = {
    .tag = AUDIO_EFFECT_LIBRARY_TAG,
    .version = EFFECT_LIBRARY_API_VERSION,
    .name = "VirtualBass",
    .implementor = "Amlogic",
    .create_effect = VirtualBassLib_Create,
    .release_effect = VirtualBassLib_Release,
    .get_descriptor = VirtualBassLib_GetDescriptor,
};

};//extern "C"

